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
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"

#include "src/engines/kotorbase/gui/inventoryitem.h"

#include "src/engines/kotor/gui/ingame/menu_equ.h"

namespace Engines {

namespace KotOR {

MenuEquipment::MenuEquipment(KotORBase::Module &module, Console *console) :
		KotORBase::MenuBase(module, console),
		_selectedSlot(KotORBase::kInventorySlotBody),
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
		slotName->setText(getSlotName(KotORBase::kInventorySlotBody));

	Odyssey::WidgetListBox *lbItems = getListBox("LB_ITEMS");
	if (lbItems) {
		lbItems->setItemWidgetFactoryFunction([](Engines::GUI &gui, const Common::UString &tag) { return new KotORBase::WidgetInventoryItem(gui, tag); });
		lbItems->setHideScrollbar(false);
		lbItems->setPadding(6);
		lbItems->setItemBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
		lbItems->setSoundSelectItem("gui_actuse");
		lbItems->createItemWidgets(5);
	}
}

void MenuEquipment::update() {
	MenuBase::update();
	updatePartyLeader("LBL_PORTRAIT");

	fillEquipedItems();
	fillEquipableItemsList();
}

void MenuEquipment::show() {
	GUI::show();

	if (_selectedSlot != KotORBase::kInventorySlotInvalid)
		getSlotButton(_selectedSlot)->setHighlight(true);
}

void MenuEquipment::hide() {
	if (_selectedSlot != KotORBase::kInventorySlotInvalid)
		getSlotButton(_selectedSlot)->setHighlight(false);

	GUI::hide();
}

void MenuEquipment::callbackRun() {
	KotORBase::InventorySlot newSlot;

	if (getLabel("LBL_INV_IMPLANT")->isHovered())
		newSlot = KotORBase::kInventorySlotImplant;
	else if (getLabel("LBL_INV_HEAD")->isHovered())
		newSlot = KotORBase::kInventorySlotHead;
	else if (getLabel("LBL_INV_HANDS")->isHovered())
		newSlot = KotORBase::kInventorySlotHands;
	else if (getLabel("LBL_INV_ARM_L")->isHovered())
		newSlot = KotORBase::kInventorySlotLeftArm;
	else if (getLabel("LBL_INV_BODY")->isHovered())
		newSlot = KotORBase::kInventorySlotBody;
	else if (getLabel("LBL_INV_ARM_R")->isHovered())
		newSlot = KotORBase::kInventorySlotRightArm;
	else if (getLabel("LBL_INV_WEAP_L")->isHovered())
		newSlot = KotORBase::kInventorySlotLeftWeapon;
	else if (getLabel("LBL_INV_BELT")->isHovered())
		newSlot = KotORBase::kInventorySlotBelt;
	else if (getLabel("LBL_INV_WEAP_R")->isHovered())
		newSlot = KotORBase::kInventorySlotRightWeapon;
	else
		return;

	if (newSlot != _selectedSlot) {
		if (_selectedSlot != KotORBase::kInventorySlotInvalid)
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

			KotORBase::Creature *pc = _module->getPC();
			KotORBase::Creature *partyLeader = _module->getPartyLeader();
			partyLeader->equipItem(itemTag, _selectedSlot, pc->getCreatureInfo());
			_module->getCurrentArea()->addToObjectMap(partyLeader);

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

	MenuBase::callbackActive(widget);
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
	Common::UString implant = getEquipedItemIcon(KotORBase::kInventorySlotImplant);
	Common::UString head = getEquipedItemIcon(KotORBase::kInventorySlotHead);
	Common::UString hands = getEquipedItemIcon(KotORBase::kInventorySlotHands);
	Common::UString armL = getEquipedItemIcon(KotORBase::kInventorySlotLeftArm);
	Common::UString body = getEquipedItemIcon(KotORBase::kInventorySlotBody);
	Common::UString armR = getEquipedItemIcon(KotORBase::kInventorySlotRightArm);
	Common::UString weapL = getEquipedItemIcon(KotORBase::kInventorySlotLeftWeapon);
	Common::UString belt = getEquipedItemIcon(KotORBase::kInventorySlotBelt);
	Common::UString weapR = getEquipedItemIcon(KotORBase::kInventorySlotRightWeapon);

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

Common::UString MenuEquipment::getEquipedItemIcon(KotORBase::InventorySlot slot) const {
	KotORBase::Item *item = _module->getPartyLeader()->getEquipedItem(slot);
	return item ? item->getIcon() : "";
}

void MenuEquipment::fillEquipableItemsList() {
	KotORBase::Inventory &inv = _module->getPC()->getInventory();

	Odyssey::WidgetListBox *lbItems = getListBox("LB_ITEMS");
	lbItems->removeAllItems();
	lbItems->addItem("None|inone|1");

	_visibleItems.clear();

	for (const auto &i : inv.getItems()) {
		try {
			KotORBase::Item item(i.second.tag);
			if (!item.isSlotEquipable(_selectedSlot))
				continue;

			if (_selectedSlot == KotORBase::kInventorySlotLeftWeapon) {
				KotORBase::Creature *partyLeader = _module->getPartyLeader();
				KotORBase::Item *rightWeapon = partyLeader->getEquipedItem(KotORBase::kInventorySlotRightWeapon);
				if (!rightWeapon || (item.getWeaponWield() != rightWeapon->getWeaponWield()))
					continue;
			}

			lbItems->addItem(Common::String::format("%s|%s|%u",
			                                         item.getName().c_str(),
			                                         item.getIcon().c_str(),
			                                         i.second.count));

			_visibleItems.push_back(i.second.tag);
		} catch (Common::Exception &e) {
			e.add("Failed to load item \"%s\"", i.second.tag.c_str());
			Common::printException(e, "WARNING: ");
		}
	}

	GfxMan.lockFrame();
	lbItems->refreshItemWidgets();
	GfxMan.unlockFrame();
}

KotORBase::InventorySlot MenuEquipment::getSlotByWidgetTag(const Common::UString &tag) const {
	if (tag == "LBL_INV_IMPLANT")
		return KotORBase::kInventorySlotImplant;
	if (tag == "LBL_INV_BODY")
		return KotORBase::kInventorySlotBody;
	if (tag == "LBL_INV_HANDS")
		return KotORBase::kInventorySlotHands;
	if (tag == "LBL_INV_ARM_R")
		return KotORBase::kInventorySlotRightArm;
	if (tag == "LBL_INV_BODY")
		return KotORBase::kInventorySlotBody;
	if (tag == "LBL_INV_ARM_L")
		return KotORBase::kInventorySlotLeftArm;
	if (tag == "LBL_INV_WEAP_R")
		return KotORBase::kInventorySlotRightWeapon;
	if (tag == "LBL_INV_BELT")
		return KotORBase::kInventorySlotBelt;
	if (tag == "LBL_INV_WEAP_L")
		return KotORBase::kInventorySlotLeftWeapon;

	return KotORBase::kInventorySlotInvalid;
}

Odyssey::WidgetButton *MenuEquipment::getSlotButton(KotORBase::InventorySlot slot) {
	switch (slot) {
		case KotORBase::kInventorySlotImplant:
			return getButton("BTN_INV_IMPLANT");
		case KotORBase::kInventorySlotHead:
			return getButton("BTN_INV_HEAD");
		case KotORBase::kInventorySlotHands:
			return getButton("BTN_INV_HANDS");
		case KotORBase::kInventorySlotRightArm:
			return getButton("BTN_INV_ARM_R");
		case KotORBase::kInventorySlotBody:
			return getButton("BTN_INV_BODY");
		case KotORBase::kInventorySlotLeftArm:
			return getButton("BTN_INV_ARM_L");
		case KotORBase::kInventorySlotRightWeapon:
			return getButton("BTN_INV_WEAP_R");
		case KotORBase::kInventorySlotBelt:
			return getButton("BTN_INV_BELT");
		case KotORBase::kInventorySlotLeftWeapon:
			return getButton("BTN_INV_WEAP_L");
		default:
			return 0;
	}
}

Common::UString MenuEquipment::getSlotName(KotORBase::InventorySlot slot) const {
	switch (slot) {
		case KotORBase::kInventorySlotImplant:
			return TalkMan.getString(31388); // Implant
		case KotORBase::kInventorySlotHead:
			return TalkMan.getString(31375); // Head
		case KotORBase::kInventorySlotHands:
			return TalkMan.getString(31383); // Hands
		case KotORBase::kInventorySlotRightArm:
			return TalkMan.getString(31377); // Right Arm
		case KotORBase::kInventorySlotBody:
			return TalkMan.getString(31380); // Body
		case KotORBase::kInventorySlotLeftArm:
			return TalkMan.getString(31376); // Left Arm
		case KotORBase::kInventorySlotRightWeapon:
			return TalkMan.getString(31379); // Right Weapon
		case KotORBase::kInventorySlotBelt:
			return TalkMan.getString(31382); // Belt
		case KotORBase::kInventorySlotLeftWeapon:
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
