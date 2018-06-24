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

#include "src/engines/aurora/kotorjadegui/button.h"
#include "src/engines/aurora/kotorjadegui/label.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"

#include "src/engines/kotor/item.h"
#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/gui/ingame/menu_equ.h"

namespace Engines {

namespace KotOR {

MenuEquipment::MenuEquipment(Console *console)
		: GUI(console),
		  _pc(0),
		  _selectedSlot(kEquipmentSlotBody),
		  _slotFixated(false) {
	load("equip");

	getListBox("LB_DESC")->setInvisible(true);
	getLabel("LBL_CANTEQUIP")->setInvisible(true);
	getLabel("LBL_SLOTNAME")->setText(getSlotName(kEquipmentSlotBody));

	WidgetListBox *lbItems = getListBox("LB_ITEMS");
	lbItems->setItemType(kLBItemTypeKotORInventory);
	lbItems->setHideScrollbar(false);
	lbItems->setPadding(6);
	lbItems->setItemBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
	lbItems->createItemWidgets(5);
}

void MenuEquipment::setPC(Creature *pc) {
	_pc = pc;

	fillEquipedItems();
	fillEquipableItemsList();
}

void MenuEquipment::show() {
	GUI::show();

	if (_selectedSlot != kEquipmentSlotNone)
		getSlotButton(_selectedSlot)->setHighlight(true);
}

void MenuEquipment::hide() {
	if (_selectedSlot != kEquipmentSlotNone)
		getSlotButton(_selectedSlot)->setHighlight(false);

	GUI::hide();
}

void MenuEquipment::callbackRun() {
	EquipmentSlot newSlot;

	if (getLabel("LBL_INV_IMPLANT")->isHovered())
		newSlot = kEquipmentSlotImplant;
	else if (getLabel("LBL_INV_HEAD")->isHovered())
		newSlot = kEquipmentSlotHead;
	else if (getLabel("LBL_INV_HANDS")->isHovered())
		newSlot = kEquipmentSlotHands;
	else if (getLabel("LBL_INV_ARM_L")->isHovered())
		newSlot = kEquipmentSlotArmL;
	else if (getLabel("LBL_INV_BODY")->isHovered())
		newSlot = kEquipmentSlotBody;
	else if (getLabel("LBL_INV_ARM_R")->isHovered())
		newSlot = kEquipmentSlotArmR;
	else if (getLabel("LBL_INV_WEAP_L")->isHovered())
		newSlot = kEquipmentSlotWeaponL;
	else if (getLabel("LBL_INV_BELT")->isHovered())
		newSlot = kEquipmentSlotBelt;
	else if (getLabel("LBL_INV_WEAP_R")->isHovered())
		newSlot = kEquipmentSlotWeaponR;
	else
		return;

	if (newSlot != _selectedSlot) {
		if (_selectedSlot != kEquipmentSlotNone)
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
	Common::UString implant = getEquipedItemIcon(kEquipmentSlotImplant);
	Common::UString head = getEquipedItemIcon(kEquipmentSlotHead);
	Common::UString hands = getEquipedItemIcon(kEquipmentSlotHands);
	Common::UString armL = getEquipedItemIcon(kEquipmentSlotArmL);
	Common::UString body = getEquipedItemIcon(kEquipmentSlotBody);
	Common::UString armR = getEquipedItemIcon(kEquipmentSlotArmR);
	Common::UString weapL = getEquipedItemIcon(kEquipmentSlotWeaponL);
	Common::UString belt = getEquipedItemIcon(kEquipmentSlotBelt);
	Common::UString weapR = getEquipedItemIcon(kEquipmentSlotWeaponR);

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

Common::UString MenuEquipment::getEquipedItemIcon(EquipmentSlot slot) const {
	Item *item = _pc->getEquipedItem(slot);
	return item ? item->getIcon() : "";
}

void MenuEquipment::fillEquipableItemsList() {
	Inventory &inv = _pc->getInventory();

	WidgetListBox *lbItems = getListBox("LB_ITEMS");
	lbItems->removeAllItems();
	lbItems->addItem("None|inone|1");

	_visibleItems.clear();

	const std::map<Common::UString, InventoryItem> &invItems = inv.getItems();
	for (std::map<Common::UString, InventoryItem>::const_iterator i = invItems.begin();
			i != invItems.end(); ++i) {
		try {
			Item item(i->second.tag);
			if ((item.getEquipableSlots() & _selectedSlot) == 0)
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

EquipmentSlot MenuEquipment::getSlotByWidgetTag(const Common::UString &tag) const {
	if (tag == "LBL_INV_IMPLANT")
		return kEquipmentSlotImplant;
	if (tag == "LBL_INV_BODY")
		return kEquipmentSlotBody;
	if (tag == "LBL_INV_HANDS")
		return kEquipmentSlotHands;
	if (tag == "LBL_INV_ARM_R")
		return kEquipmentSlotArmR;
	if (tag == "LBL_INV_BODY")
		return kEquipmentSlotBody;
	if (tag == "LBL_INV_ARM_L")
		return kEquipmentSlotArmL;
	if (tag == "LBL_INV_WEAP_R")
		return kEquipmentSlotWeaponR;
	if (tag == "LBL_INV_BELT")
		return kEquipmentSlotBelt;
	if (tag == "LBL_INV_WEAP_L")
		return kEquipmentSlotWeaponL;

	return kEquipmentSlotNone;
}

WidgetButton *MenuEquipment::getSlotButton(EquipmentSlot slot) {
	switch (slot) {
		case kEquipmentSlotImplant:
			return getButton("BTN_INV_IMPLANT");
		case kEquipmentSlotHead:
			return getButton("BTN_INV_HEAD");
		case kEquipmentSlotHands:
			return getButton("BTN_INV_HANDS");
		case kEquipmentSlotArmR:
			return getButton("BTN_INV_ARM_R");
		case kEquipmentSlotBody:
			return getButton("BTN_INV_BODY");
		case kEquipmentSlotArmL:
			return getButton("BTN_INV_ARM_L");
		case kEquipmentSlotWeaponR:
			return getButton("BTN_INV_WEAP_R");
		case kEquipmentSlotBelt:
			return getButton("BTN_INV_BELT");
		case kEquipmentSlotWeaponL:
			return getButton("BTN_INV_WEAP_L");
		default:
			return 0;
	}
}

Common::UString MenuEquipment::getSlotName(EquipmentSlot slot) const {
	switch (slot) {
		case kEquipmentSlotImplant:
			return TalkMan.getString(31388); // Implant
		case kEquipmentSlotHead:
			return TalkMan.getString(31375); // Head
		case kEquipmentSlotHands:
			return TalkMan.getString(31383); // Hands
		case kEquipmentSlotArmR:
			return TalkMan.getString(31377); // Right Arm
		case kEquipmentSlotBody:
			return TalkMan.getString(31380); // Body
		case kEquipmentSlotArmL:
			return TalkMan.getString(31376); // Left Arm
		case kEquipmentSlotWeaponR:
			return TalkMan.getString(31379); // Right Weapon
		case kEquipmentSlotBelt:
			return TalkMan.getString(31382); // Belt
		case kEquipmentSlotWeaponL:
			return TalkMan.getString(31378); // Left Weapon
		default:
			return "";
	}
}

void MenuEquipment::fixateOnSlot(bool fixate) {
	_slotFixated = fixate;

	WidgetListBox *lbDesc = getListBox("LB_DESC");

	WidgetButton *btnInvImplant = getButton("BTN_INV_IMPLANT");
	WidgetButton *btnInvHead = getButton("BTN_INV_HEAD");
	WidgetButton *btnInvHands = getButton("BTN_INV_HANDS");
	WidgetButton *btnInvArmL = getButton("BTN_INV_ARM_L");
	WidgetButton *btnInvBody = getButton("BTN_INV_BODY");
	WidgetButton *btnInvArmR = getButton("BTN_INV_ARM_R");
	WidgetButton *btnInvWeapL = getButton("BTN_INV_WEAP_L");
	WidgetButton *btnInvBelt = getButton("BTN_INV_BELT");
	WidgetButton *btnInvWeapR = getButton("BTN_INV_WEAP_R");

	WidgetLabel *lblInvImplant = getLabel("LBL_INV_IMPLANT");
	WidgetLabel *lblInvHead = getLabel("LBL_INV_HEAD");
	WidgetLabel *lblInvHands = getLabel("LBL_INV_HANDS");
	WidgetLabel *lblInvArmL = getLabel("LBL_INV_ARM_L");
	WidgetLabel *lblInvBody = getLabel("LBL_INV_BODY");
	WidgetLabel *lblInvArmR = getLabel("LBL_INV_ARM_R");
	WidgetLabel *lblInvWeapL = getLabel("LBL_INV_WEAP_L");
	WidgetLabel *lblInvBelt = getLabel("LBL_INV_BELT");
	WidgetLabel *lblInvWeapR = getLabel("LBL_INV_WEAP_R");

	WidgetLabel *lblTxtBar = getLabel("LBL_TXTBAR");
	WidgetLabel *lblSlotName = getLabel("LBL_SLOTNAME");
	WidgetLabel *lblPortBord = getLabel("LBL_PORT_BORD");
	WidgetLabel *lblPortrait = getLabel("LBL_PORTRAIT");

	WidgetListBox *lbItems = getListBox("LB_ITEMS");
	WidgetButton *btnBack = getButton("BTN_BACK");

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
