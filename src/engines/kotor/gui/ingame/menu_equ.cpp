/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The ingame equipment menu.
 */

#include "src/aurora/talkman.h"

#include "src/graphics/graphics.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"

#include "src/engines/kotorbase/item.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotorbase/gui/inventoryitem.h"

#include "src/engines/kotor/gui/ingame/menu_equ.h"

namespace Engines {

namespace KotOR {

MenuEquipment::MenuEquipment(Console *console)
		: GUI(console),
		  _pc(0),
		  _selectedSlot(kInventorySlotBody),
		  _slotFixated(false) {
	load("equip");

	Odyssey::WidgetListBox *desc = getListBox("LB_DESC");
	if (!desc)
		throw Common::Exception("MenuEquipment: No desription listbox");

	desc->setInvisible(true);

	Odyssey::WidgetLabel *cantEquip = getLabel("LBL_CANTEQUIP");
	if (cantEquip)
		cantEquip->setInvisible(true);

	Odyssey::WidgetLabel *slotName = getLabel("LBL_SLOTNAME");
	if (slotName)
		slotName->setText(getSlotName(kInventorySlotBody));

	Odyssey::WidgetListBox *lbItems = getListBox("LB_ITEMS");
	if (lbItems) {
		lbItems->setItemWidgetFactoryFunction([](Engines::GUI &gui, const Common::UString &tag) { return new WidgetInventoryItem(gui, tag); });
		lbItems->setHideScrollbar(false);
		lbItems->setPadding(6);
		lbItems->setItemBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
		lbItems->setSoundSelectItem("gui_actuse");
		lbItems->createItemWidgets(5);
	}
}

void MenuEquipment::setPC(Creature *pc) {
	_pc = pc;

	fillEquipedItems();
	fillEquipableItemsList();
}

void MenuEquipment::show() {
	GUI::show();

	if (_selectedSlot != kInventorySlotInvalid)
		getSlotButton(_selectedSlot)->setHighlight(true);
}

void MenuEquipment::hide() {
	if (_selectedSlot != kInventorySlotInvalid)
		getSlotButton(_selectedSlot)->setHighlight(false);

	GUI::hide();
}

void MenuEquipment::callbackRun() {
	InventorySlot newSlot;

	if (getLabel("LBL_INV_IMPLANT")->isHovered())
		newSlot = kInventorySlotImplant;
	else if (getLabel("LBL_INV_HEAD")->isHovered())
		newSlot = kInventorySlotHead;
	else if (getLabel("LBL_INV_HANDS")->isHovered())
		newSlot = kInventorySlotHands;
	else if (getLabel("LBL_INV_ARM_L")->isHovered())
		newSlot = kInventorySlotLeftArm;
	else if (getLabel("LBL_INV_BODY")->isHovered())
		newSlot = kInventorySlotBody;
	else if (getLabel("LBL_INV_ARM_R")->isHovered())
		newSlot = kInventorySlotRightArm;
	else if (getLabel("LBL_INV_WEAP_L")->isHovered())
		newSlot = kInventorySlotLeftWeapon;
	else if (getLabel("LBL_INV_BELT")->isHovered())
		newSlot = kInventorySlotBelt;
	else if (getLabel("LBL_INV_WEAP_R")->isHovered())
		newSlot = kInventorySlotRightWeapon;
	else
		return;

	if (newSlot != _selectedSlot) {
		if (_selectedSlot != kInventorySlotInvalid)
			getSlotButton(_selectedSlot)->setHighlight(false);

		_selectedSlot = newSlot;
		getSlotButton(_selectedSlot)->setHighlight(true);
		getLabel("LBL_SLOTNAME")->setText(getSlotName(_selectedSlot));
		fillEquipableItemsList();
	}
}

void MenuEquipment::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (_slotFixated) {
		if (tag == "BTN_EQUIP") {
			int selectedIndex = getListBox("LB_ITEMS")->getSelectedIndex();
			Common::UString itemTag = selectedIndex > 0 ? _visibleItems[selectedIndex - 1] : "";
			_pc->equipItem(itemTag, _selectedSlot);

			fillEquipedItems();
			fillEquipableItemsList();

			fixateOnSlot(false);
			return;
		}
		if (tag == "BTN_BACK") {
			fixateOnSlot(false);
			return;
		}
	} else {
		if (tag == "BTN_BACK") {
			_returnCode = 1;
			return;
		}
		if (tag.beginsWith("LBL_INV_")) {
			fixateOnSlot(true);
			return;
		}
	}
}

void MenuEquipment::callbackKeyInput(const Events::Key &key, const Events::EventType &type) {
	if (type == Events::kEventKeyDown) {
		switch (key) {
			case Events::kKeyUp:
				getListBox("LB_ITEMS")->selectPreviousItem();
				break;
			case Events::kKeyDown:
				getListBox("LB_ITEMS")->selectNextItem();
				break;
			default:
				break;
		}
	}
}

void MenuEquipment::fillEquipedItems() {
	Common::UString implant = getEquipedItemIcon(kInventorySlotImplant);
	Common::UString head = getEquipedItemIcon(kInventorySlotHead);
	Common::UString hands = getEquipedItemIcon(kInventorySlotHands);
	Common::UString armL = getEquipedItemIcon(kInventorySlotLeftArm);
	Common::UString body = getEquipedItemIcon(kInventorySlotBody);
	Common::UString armR = getEquipedItemIcon(kInventorySlotRightArm);
	Common::UString weapL = getEquipedItemIcon(kInventorySlotLeftWeapon);
	Common::UString belt = getEquipedItemIcon(kInventorySlotBelt);
	Common::UString weapR = getEquipedItemIcon(kInventorySlotRightWeapon);

	getLabel("LBL_INV_IMPLANT")->setFill(implant.empty() ? "iimplant" : implant);
	getLabel("LBL_INV_HEAD")->setFill(head.empty() ? "ihead" : head);
	getLabel("LBL_INV_HANDS")->setFill(hands.empty() ? "ihands" : hands);
	getLabel("LBL_INV_ARM_L")->setFill(armL.empty() ? "iforearm_l" : armL);
	getLabel("LBL_INV_BODY")->setFill(body.empty() ? "iarmor" : body);
	getLabel("LBL_INV_ARM_R")->setFill(armR.empty() ? "iforearm_r" : armR);
	getLabel("LBL_INV_WEAP_L")->setFill(weapL.empty() ? "iweap_l" : weapL);
	getLabel("LBL_INV_BELT")->setFill(belt.empty() ? "ibelt" : belt);
	getLabel("LBL_INV_WEAP_R")->setFill(weapR.empty() ? "iweap_r" : weapR);
}

Common::UString MenuEquipment::getEquipedItemIcon(InventorySlot slot) const {
	Item *item = _pc->getEquipedItem(slot);
	return item ? item->getIcon() : "";
}

void MenuEquipment::fillEquipableItemsList() {
	Inventory &inv = _pc->getInventory();

	Odyssey::WidgetListBox *lbItems = getListBox("LB_ITEMS");
	lbItems->removeAllItems();
	lbItems->addItem("None|inone|1");

	_visibleItems.clear();

	const std::map<Common::UString, Inventory::ItemGroup> &invItems = inv.getItems();
	for (std::map<Common::UString, Inventory::ItemGroup>::const_iterator i = invItems.begin();
			i != invItems.end(); ++i) {
		try {
			Item item(i->second.tag);
			if (!item.isSlotEquipable(_selectedSlot))
				continue;

			lbItems->addItem(Common::UString::format("%s|%s|%u",
			                                         item.getName().c_str(),
			                                         item.getIcon().c_str(),
			                                         i->second.count));

			_visibleItems.push_back(i->second.tag);
		} catch (Common::Exception &e) {
			e.add("Failed to load item %s", i->second.tag.c_str());
			Common::printException(e, "WARNING: ");
		}
	}

	GfxMan.lockFrame();
	lbItems->refreshItemWidgets();
	GfxMan.unlockFrame();
}

InventorySlot MenuEquipment::getSlotByWidgetTag(const Common::UString &tag) const {
	if (tag == "LBL_INV_IMPLANT")
		return kInventorySlotImplant;
	if (tag == "LBL_INV_BODY")
		return kInventorySlotBody;
	if (tag == "LBL_INV_HANDS")
		return kInventorySlotHands;
	if (tag == "LBL_INV_ARM_R")
		return kInventorySlotRightArm;
	if (tag == "LBL_INV_BODY")
		return kInventorySlotBody;
	if (tag == "LBL_INV_ARM_L")
		return kInventorySlotLeftArm;
	if (tag == "LBL_INV_WEAP_R")
		return kInventorySlotRightWeapon;
	if (tag == "LBL_INV_BELT")
		return kInventorySlotBelt;
	if (tag == "LBL_INV_WEAP_L")
		return kInventorySlotLeftWeapon;

	return kInventorySlotInvalid;
}

Odyssey::WidgetButton *MenuEquipment::getSlotButton(InventorySlot slot) {
	switch (slot) {
		case kInventorySlotImplant:
			return getButton("BTN_INV_IMPLANT");
		case kInventorySlotHead:
			return getButton("BTN_INV_HEAD");
		case kInventorySlotHands:
			return getButton("BTN_INV_HANDS");
		case kInventorySlotRightArm:
			return getButton("BTN_INV_ARM_R");
		case kInventorySlotBody:
			return getButton("BTN_INV_BODY");
		case kInventorySlotLeftArm:
			return getButton("BTN_INV_ARM_L");
		case kInventorySlotRightWeapon:
			return getButton("BTN_INV_WEAP_R");
		case kInventorySlotBelt:
			return getButton("BTN_INV_BELT");
		case kInventorySlotLeftWeapon:
			return getButton("BTN_INV_WEAP_L");
		default:
			return 0;
	}
}

Common::UString MenuEquipment::getSlotName(InventorySlot slot) const {
	switch (slot) {
		case kInventorySlotImplant:
			return TalkMan.getString(31388); // Implant
		case kInventorySlotHead:
			return TalkMan.getString(31375); // Head
		case kInventorySlotHands:
			return TalkMan.getString(31383); // Hands
		case kInventorySlotRightArm:
			return TalkMan.getString(31377); // Right Arm
		case kInventorySlotBody:
			return TalkMan.getString(31380); // Body
		case kInventorySlotLeftArm:
			return TalkMan.getString(31376); // Left Arm
		case kInventorySlotRightWeapon:
			return TalkMan.getString(31379); // Right Weapon
		case kInventorySlotBelt:
			return TalkMan.getString(31382); // Belt
		case kInventorySlotLeftWeapon:
			return TalkMan.getString(31378); // Left Weapon
		default:
			return "";
	}
}

void MenuEquipment::fixateOnSlot(bool fixate) {
	_slotFixated = fixate;

	Odyssey::WidgetListBox *lbDesc = getListBox("LB_DESC");

	Odyssey::WidgetButton *btnInvImplant = getButton("BTN_INV_IMPLANT");
	Odyssey::WidgetButton *btnInvHead = getButton("BTN_INV_HEAD");
	Odyssey::WidgetButton *btnInvHands = getButton("BTN_INV_HANDS");
	Odyssey::WidgetButton *btnInvArmL = getButton("BTN_INV_ARM_L");
	Odyssey::WidgetButton *btnInvBody = getButton("BTN_INV_BODY");
	Odyssey::WidgetButton *btnInvArmR = getButton("BTN_INV_ARM_R");
	Odyssey::WidgetButton *btnInvWeapL = getButton("BTN_INV_WEAP_L");
	Odyssey::WidgetButton *btnInvBelt = getButton("BTN_INV_BELT");
	Odyssey::WidgetButton *btnInvWeapR = getButton("BTN_INV_WEAP_R");

	Odyssey::WidgetLabel *lblInvImplant = getLabel("LBL_INV_IMPLANT");
	Odyssey::WidgetLabel *lblInvHead = getLabel("LBL_INV_HEAD");
	Odyssey::WidgetLabel *lblInvHands = getLabel("LBL_INV_HANDS");
	Odyssey::WidgetLabel *lblInvArmL = getLabel("LBL_INV_ARM_L");
	Odyssey::WidgetLabel *lblInvBody = getLabel("LBL_INV_BODY");
	Odyssey::WidgetLabel *lblInvArmR = getLabel("LBL_INV_ARM_R");
	Odyssey::WidgetLabel *lblInvWeapL = getLabel("LBL_INV_WEAP_L");
	Odyssey::WidgetLabel *lblInvBelt = getLabel("LBL_INV_BELT");
	Odyssey::WidgetLabel *lblInvWeapR = getLabel("LBL_INV_WEAP_R");

	Odyssey::WidgetLabel *lblTxtBar = getLabel("LBL_TXTBAR");
	Odyssey::WidgetLabel *lblSlotName = getLabel("LBL_SLOTNAME");
	Odyssey::WidgetLabel *lblPortBord = getLabel("LBL_PORT_BORD");
	Odyssey::WidgetLabel *lblPortrait = getLabel("LBL_PORTRAIT");

	Odyssey::WidgetListBox *lbItems = getListBox("LB_ITEMS");
	Odyssey::WidgetButton *btnBack = getButton("BTN_BACK");

	lbDesc->setInvisible(!_slotFixated);

	btnInvImplant->setInvisible(_slotFixated);
	btnInvHead->setInvisible(_slotFixated);
	btnInvHands->setInvisible(_slotFixated);
	btnInvArmL->setInvisible(_slotFixated);
	btnInvBody->setInvisible(_slotFixated);
	btnInvArmR->setInvisible(_slotFixated);
	btnInvWeapL->setInvisible(_slotFixated);
	btnInvBelt->setInvisible(_slotFixated);
	btnInvWeapR->setInvisible(_slotFixated);

	lblInvImplant->setInvisible(_slotFixated);
	lblInvHead->setInvisible(_slotFixated);
	lblInvHands->setInvisible(_slotFixated);
	lblInvArmL->setInvisible(_slotFixated);
	lblInvBody->setInvisible(_slotFixated);
	lblInvArmR->setInvisible(_slotFixated);
	lblInvWeapL->setInvisible(_slotFixated);
	lblInvBelt->setInvisible(_slotFixated);
	lblInvWeapR->setInvisible(_slotFixated);

	lblTxtBar->setInvisible(_slotFixated);
	lblSlotName->setInvisible(_slotFixated);
	lblPortBord->setInvisible(_slotFixated);
	lblPortrait->setInvisible(_slotFixated);

	if (_slotFixated) {
		lbDesc->show();

		getSlotButton(_selectedSlot)->setHighlight(false);

		btnInvImplant->hide();
		btnInvHead->hide();
		btnInvHands->hide();
		btnInvArmL->hide();
		btnInvBody->hide();
		btnInvArmR->hide();
		btnInvWeapL->hide();
		btnInvBelt->hide();
		btnInvWeapR->hide();

		lblInvImplant->hide();
		lblInvHead->hide();
		lblInvHands->hide();
		lblInvArmL->hide();
		lblInvBody->hide();
		lblInvArmR->hide();
		lblInvWeapL->hide();
		lblInvBelt->hide();
		lblInvWeapR->hide();

		lblTxtBar->hide();
		lblSlotName->hide();
		lblPortBord->hide();
		lblPortrait->hide();

		lbItems->setItemSelectionEnabled(true);
		lbItems->setItemBorderColor(0.0f, 0.648438f, 0.968750f, 1.0f);
		lbItems->selectItemByWidgetTag("LB_ITEMS_ITEM_0");

		btnBack->setText(TalkMan.getString(1581));
	} else {
		lbDesc->hide();

		btnInvImplant->show();
		btnInvHead->show();
		btnInvHands->show();
		btnInvArmL->show();
		btnInvBody->show();
		btnInvArmR->show();
		btnInvWeapL->show();
		btnInvBelt->show();
		btnInvWeapR->show();

		getSlotButton(_selectedSlot)->setHighlight(true);

		lblInvImplant->show();
		lblInvHead->show();
		lblInvHands->show();
		lblInvArmL->show();
		lblInvBody->show();
		lblInvArmR->show();
		lblInvWeapL->show();
		lblInvBelt->show();
		lblInvWeapR->show();

		lblTxtBar->show();
		lblSlotName->show();
		lblPortBord->show();
		lblPortrait->show();

		lbItems->setItemSelectionEnabled(false);
		lbItems->setItemBorderColor(0.0f, 0.0f, 0.0f, 0.0f);

		btnBack->setText(TalkMan.getString(1582));
	}

	lbItems->refreshItemWidgets();
}

} // End of namespace KotOR

} // End of namespace Engines
