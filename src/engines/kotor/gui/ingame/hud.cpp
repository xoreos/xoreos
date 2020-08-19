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
 *  In-game HUD for Star Wars: Knights of the Old Republic.
 */

#include "src/common/strutil.h"

#include "src/aurora/resman.h"

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/progressbar.h"

#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/creature.h"

#include "src/engines/kotor/gui/ingame/hud.h"

namespace Engines {

namespace KotOR {

enum Position {
	kPositionUpperLeft,
	kPositionUpperRight,
	kPositionLowerLeft,
	kPositionLowerRight,
	kPositionUpperMid,
	kPositionLowerMid,
	kPositionNotSpecified
};

struct KnownWidget {
	const char *name;
	bool visible;
	Position position;
};

static const KnownWidget kKnownWidgets[] = {
	{ "LBL_MAPVIEW", true, kPositionUpperLeft },
	{ "LBL_MAPBORDER", true, kPositionUpperLeft },
	{ "LBL_ARROW", true, kPositionUpperLeft },
	{ "LBL_PLOTXP", false, kPositionUpperLeft },
	{ "LBL_STEALTHXP", false, kPositionUpperLeft },
	{ "LBL_CASH", false, kPositionUpperLeft },
	{ "LBL_JOURNAL", false, kPositionUpperLeft },
	{ "LBL_LIGHTSHIFT", false, kPositionUpperLeft },
	{ "LBL_DARKSHIFT", false, kPositionUpperLeft },
	{ "LBL_ITEMRCVD", false, kPositionUpperLeft },
	{ "LBL_ITEMLOST", false, kPositionUpperLeft },

	{ "LBL_MENUBG", true, kPositionUpperRight },
	{ "BTN_MSG", true, kPositionUpperRight },
	{ "BTN_ABI", true, kPositionUpperRight },
	{ "BTN_JOU", true, kPositionUpperRight },
	{ "BTN_INV", true, kPositionUpperRight },
	{ "BTN_CHAR", true, kPositionUpperRight },
	{ "BTN_EQU", true, kPositionUpperRight },
	{ "BTN_MAP", true, kPositionUpperRight },
	{ "BTN_OPT", true, kPositionUpperRight },

	{ "LBL_MOULDING1", true, kPositionLowerLeft },
	{ "TB_SOLO", true, kPositionLowerLeft },
	{ "TB_STEALTH", true, kPositionLowerLeft },
	{ "TB_PAUSE", true, kPositionLowerLeft },
	{ "LBL_CHAR1", false, kPositionLowerLeft },
	{ "LBL_CHAR2", false, kPositionLowerLeft },
	{ "LBL_CHAR3", false, kPositionLowerLeft },
	{ "LBL_DEBILATATED1", false, kPositionLowerLeft },
	{ "LBL_DEBILATATED2", false, kPositionLowerLeft },
	{ "LBL_DEBILATATED3", false, kPositionLowerLeft },
	{ "LBL_DISABLE1", false, kPositionLowerLeft },
	{ "LBL_DISABLE2", false, kPositionLowerLeft },
	{ "LBL_DISABLE3", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTRED1", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTRED2", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTRED3", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTINC1", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTINC2", false, kPositionLowerLeft },
	{ "LBL_CMBTEFCTINC3", false, kPositionLowerLeft },
	{ "LBL_BACK1", false, kPositionLowerLeft },
	{ "LBL_BACK2", false, kPositionLowerLeft },
	{ "LBL_BACK3", false, kPositionLowerLeft },
	{ "BTN_CHAR1", false, kPositionLowerLeft },
	{ "BTN_CHAR2", false, kPositionLowerLeft },
	{ "BTN_CHAR3", false, kPositionLowerLeft },
	{ "LBL_LVLUPBG1", false, kPositionLowerLeft },
	{ "LBL_LVLUPBG2", false, kPositionLowerLeft },
	{ "LBL_LVLUPBG3", false, kPositionLowerLeft },
	{ "LBL_LEVELUP1", false, kPositionLowerLeft },
	{ "LBL_LEVELUP2", false, kPositionLowerLeft },
	{ "LBL_LEVELUP3", false, kPositionLowerLeft },
	{ "PB_VIT1", false, kPositionLowerLeft },
	{ "PB_VIT2", false, kPositionLowerLeft },
	{ "PB_VIT3", false, kPositionLowerLeft },
	{ "PB_FORCE1", false, kPositionLowerLeft },
	{ "PB_FORCE2", false, kPositionLowerLeft },
	{ "PB_FORCE3", false, kPositionLowerLeft },

	{ "LBL_MOULDING3", true, kPositionLowerRight },
	{ "BTN_ACTIONDOWN0", true, kPositionLowerRight },
	{ "BTN_ACTIONDOWN1", true, kPositionLowerRight },
	{ "BTN_ACTIONDOWN2", true, kPositionLowerRight },
	{ "BTN_ACTIONDOWN3", true, kPositionLowerRight },
	{ "BTN_ACTIONUP0", true, kPositionLowerRight },
	{ "BTN_ACTIONUP1", true, kPositionLowerRight },
	{ "BTN_ACTIONUP2", true, kPositionLowerRight },
	{ "BTN_ACTIONUP3", true, kPositionLowerRight },
	{ "BTN_ACTION0", true, kPositionLowerRight },
	{ "BTN_ACTION1", true, kPositionLowerRight },
	{ "BTN_ACTION2", true, kPositionLowerRight },
	{ "BTN_ACTION3", true, kPositionLowerRight },
	{ "LBL_ACTION0", true, kPositionLowerRight },
	{ "LBL_ACTION1", true, kPositionLowerRight },
	{ "LBL_ACTION2", true, kPositionLowerRight },
	{ "LBL_ACTION3", true, kPositionLowerRight },
	{ "LBL_ACTIONDESC", false, kPositionLowerRight },
	{ "LBL_ACTIONDESCBG", false, kPositionLowerRight },

	{ "LBL_CMBTMSGBG", false, kPositionUpperMid },
	{ "LBL_CMBTMODEMSG", false, kPositionUpperMid },

	{ "LBL_COMBATBG1", false, kPositionLowerMid },
	{ "LBL_COMBATBG2", false, kPositionLowerMid },
	{ "LBL_COMBATBG3", false, kPositionLowerMid },
	{ "LBL_QUEUE0", false, kPositionLowerMid },
	{ "LBL_QUEUE1", false, kPositionLowerMid },
	{ "LBL_QUEUE2", false, kPositionLowerMid },
	{ "LBL_QUEUE3", false, kPositionLowerMid },
	{ "BTN_CLEARALL", false, kPositionLowerMid },
	{ "BTN_CLEARONE", false, kPositionLowerMid },
	{ "BTN_CLEARONE2", false, kPositionLowerMid },

	{ "PB_HEALTH", false, kPositionNotSpecified },
	{ "LBL_HEALTHBG", false, kPositionNotSpecified },
	{ "LBL_NAMEBG", false, kPositionNotSpecified },
	{ "LBL_NAME", false, kPositionNotSpecified },
	{ "BTN_TARGET0", false, kPositionNotSpecified },
	{ "BTN_TARGET1", false, kPositionNotSpecified },
	{ "BTN_TARGET2", false, kPositionNotSpecified },
	{ "BTN_TARGETUP0", false, kPositionNotSpecified },
	{ "BTN_TARGETUP1", false, kPositionNotSpecified },
	{ "BTN_TARGETUP2", false, kPositionNotSpecified },
	{ "BTN_TARGETDOWN0", false, kPositionNotSpecified },
	{ "BTN_TARGETDOWN1", false, kPositionNotSpecified },
	{ "BTN_TARGETDOWN2", false, kPositionNotSpecified },

	// TODO: Add xbox ui widgets.
};

struct Resolution {
	int width;
	int height;

	const char *gui;

	bool operator<(const Resolution &x) const {
		if (width == x.width)
			return height < x.height;

		return width < x.width;
	}
};

static const Resolution kResolution[] = {
	{  800,  600, "mipc28x6"   },
	{  800,  600, "mi8x6"      },
	{ 1024,  768, "mipc210x7"  },
	{ 1280,  960, "mipc212x9"  },
	{ 1280, 1024, "mipc212x10" },
	{ 1600, 1200, "mipc216x12" },
};

HUD::HUD(KotORBase::Module &module, Engines::Console *console) :
		KotORBase::HUD(module, console),
		_menu(module, console) {

	update(WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

void HUD::reset() {
	_minimapPointer = nullptr;
	KotORBase::HUD::reset();
}

void HUD::init() {
	_minimapPointer = getLabel("LBL_ARROW");
	KotORBase::HUD::init();
}

void HUD::setReturnStrref(uint32_t id) {
	_menu.setReturnStrref(id);
}

void HUD::setReturnQueryStrref(uint32_t id) {
	_menu.setReturnQueryStrref(id);
}

void HUD::setReturnEnabled(bool enabled) {
	_menu.setReturnEnabled(enabled);
}

void HUD::setMinimap(const Common::UString &map, int northAxis,
                     float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
                     float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y) {

	Odyssey::WidgetLabel *mapView = getLabel("LBL_MAPVIEW");
	if (!mapView) {
		warning("No such GUI element \"LBL_MAPVIEW\"");
		return;
	}

	GfxMan.lockFrame();

	_minimap = std::make_unique<Minimap>(map, northAxis, mapPt1X, mapPt1Y, mapPt2X, mapPt2Y, worldPt1X, worldPt1Y, worldPt2X, worldPt2Y);
	mapView->setSubScene(_minimap.get());

	GfxMan.unlockFrame();
}

void HUD::setPosition(float x, float y) {
	if (_minimap)
		_minimap->setPosition(x, y);
}

void HUD::setRotation(float angle) {
	switch (_minimap->getNorthAxis()) {
		case 0:
			_minimapPointer->setRotation(angle);
			break;
		case 3:
			_minimapPointer->setRotation(angle - 90.0f);
			break;
		default:
			warning("Unknown north axis");
	}
}

void HUD::showContainer(KotORBase::Inventory &inv) {
	_container = std::make_unique<ContainerMenu>();
	_container->fillFromInventory(inv);

	if (sub(*_container, kStartCodeNone, true, false) == 1) {
		KotORBase::Inventory &partyInventory = _module.getPC()->getInventory();

		const std::map<Common::UString, KotORBase::Inventory::ItemGroup> &items = inv.getItems();
		for (std::map<Common::UString, KotORBase::Inventory::ItemGroup>::const_iterator i = items.begin();
				i != items.end(); ++i) {
			partyInventory.addItem(i->first, i->second.count);
		}

		inv.removeAllItems();
	}
}

void HUD::setPortrait(uint8_t n, bool visible, const Common::UString &portrait) {
	Odyssey::WidgetLabel *labelBack = getLabel(Common::UString("LBL_BACK") + Common::composeString(n));
	if (labelBack)
		labelBack->setInvisible(!visible);

	Odyssey::WidgetLabel *labelChar = getLabel(Common::UString("LBL_CHAR") + Common::composeString(n));
	if (labelChar) {
		labelChar->setInvisible(!visible);
		labelChar->setFill(portrait);
	}

	Odyssey::WidgetProgressbar *vitals = getProgressbar(Common::UString("PB_VIT") + Common::composeString(n));
	if (vitals)
		vitals->setInvisible(!visible);

	if (visible && _visible) {
		if (labelBack)
			labelBack->show();
		if (labelChar)
			labelChar->show();
		if (vitals)
			vitals->show();
	} else {
		if (labelBack)
			labelBack->hide();
		if (labelChar)
			labelChar->hide();
		if (vitals)
			vitals->hide();
	}
}

void HUD::setPartyLeader(KotORBase::Creature *creature) {
	setPortrait(1, creature != 0, creature ? creature->getPortrait() : "");
}

void HUD::setPartyMember1(KotORBase::Creature *creature) {
	setPortrait(3, creature != 0, creature ? creature->getPortrait() : "");
}

void HUD::setPartyMember2(KotORBase::Creature *creature) {
	setPortrait(2, creature != 0, creature ? creature->getPortrait() : "");
}

void HUD::update(int width, int height) {
	std::set<Resolution> availableRes;

	for (size_t i = 0; i < ARRAYSIZE(kResolution); i++)
		if (ResMan.hasResource(kResolution[i].gui, Aurora::kFileTypeGUI))
			availableRes.insert(kResolution[i]);

	const int wWidth  = width;
	const int wHeight = height;

	const Resolution *foundRes = 0;
	for (std::set<Resolution>::const_iterator it = availableRes.begin(); it != availableRes.end(); ++it)
		if (it->width == wWidth && it->height == wHeight)
			foundRes = &*it;

	bool scale = false;
	if (!foundRes) {
		for (std::set<Resolution>::const_iterator it = availableRes.begin(); it != availableRes.end(); ++it) {
			if ((it->width == 800) && (it->height == 600)) {
				foundRes = &*it;
				break;
			}
		}
		if (!foundRes)
			throw Common::Exception("No gui with 800x600 resolution found.");
		scale = true;
	}

	reset();
	load(foundRes->gui);
	init();

	// Make all the widgets invisible and scale them if needed.
	for (size_t i = 0; i < ARRAYSIZE(kKnownWidgets); i++) {
		Widget *widget = getWidget(kKnownWidgets[i].name);
		if (!widget)
			continue;

		widget->setInvisible(!kKnownWidgets[i].visible);

		float x, y, z;
		if (scale) {
			switch (kKnownWidgets[i].position) {
				case kPositionUpperLeft:
					widget->getPosition(x, y, z);
					widget->setPosition(-wWidth/2 + (400 + x), wHeight/2 - (300 - y), z);
					break;
				case kPositionUpperRight:
					widget->getPosition(x, y, z);
					widget->setPosition(wWidth/2 - (400 - x), wHeight/2 - (300 - y), z);
					break;
				case kPositionUpperMid:
					widget->getPosition(x, y, z);
					widget->setPosition(x, wHeight/2 - (300 - y), z);
					break;
				case kPositionLowerLeft:
					widget->getPosition(x, y, z);
					widget->setPosition(-wWidth/2 + (400 + x), -wHeight/2 + (300 + y), z);
					break;
				case kPositionLowerRight:
					widget->getPosition(x, y, z);
					widget->setPosition(wWidth/2 - (400 - x), -wHeight/2 + (300 + y), z);
					break;
				case kPositionLowerMid:
					widget->getPosition(x, y, z);
					widget->setPosition(x, -wHeight/2 + (300 + y), z);
					break;
				default:
					break;
			}
		}
	}
}

void HUD::initWidget(Engines::Widget &widget) {
	// Don't know what these two are doing, but they spawn over the complete screen blocking the 3d picking.
	if (widget.getTag() == "LBL_MAP")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_ARROW_MARGIN")
		widget.setInvisible(true);

	// TGuiPanel needs to be disabled in order to ensure, it does not block 3d picking.
	if (widget.getTag() == "TGuiPanel")
		widget.setInvisible(true);

	// First disable all portraits
	if (widget.getTag().contains("LBL_CHAR"))
		widget.setInvisible(true);
	if (widget.getTag().contains("BTN_CHAR") && widget.getTag().size() == 9)
		widget.setInvisible(true);
	if (widget.getTag().contains("LBL_BACK"))
		widget.setInvisible(true);
	if (widget.getTag().contains("PB_VIT"))
		widget.setInvisible(true);
	if (widget.getTag().contains("PB_FORCE"))
		widget.setInvisible(true);
}

void HUD::callbackActive(Widget &widget) {
	KotORBase::HUD::callbackActive(widget);

	if (widget.getTag().beginsWith("BTN_TARGET"))
		return;

	if (widget.getTag() == "LBL_CHAR1") {
		_module.setPartyLeaderByIndex(0);
		return;
	}
	if (widget.getTag() == "LBL_CHAR2") {
		_module.setPartyLeaderByIndex(2);
		return;
	}
	if (widget.getTag() == "LBL_CHAR3") {
		_module.setPartyLeaderByIndex(1);
		return;
	}

	if (_menu.isMenuImplemented(widget.getTag())) {
		hideSelection();
		_menu.showMenu(widget.getTag());
		sub(_menu);
		_module.updateFrameTimestamp();
	}
}

void HUD::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight), int newWidth, int newHeight) {
	update(newWidth, newHeight);
}

} // End of namespace KotOR

} // End of namespace Engines
