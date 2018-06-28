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
 *  The ingame HUD.
 */

#include <set>

#include "src/common/strutil.h"

#include "src/aurora/resman.h"

#include "src/graphics/windowman.h"
#include "src/graphics/graphics.h"

#include "src/engines/kotor/gui/ingame/hud.h"
#include "src/engines/aurora/kotorjadegui/label.h"
#include "src/engines/aurora/kotorjadegui/progressbar.h"

namespace Engines {

namespace KotOR {

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
	{ 1600, 1200, "mipc216x12" },
};

HUD::HUD(Module &module, Engines::Console *console)
		: GUI(console),
		  _module(&module),
		  _menu(module, console) {

	std::set<Resolution> availableRes;

	for (size_t i = 0; i < ARRAYSIZE(kResolution); i++)
		if (ResMan.hasResource(kResolution[i].gui, Aurora::kFileTypeGUI))
			availableRes.insert(kResolution[i]);

	const int wWidth  = WindowMan.getWindowWidth();
	const int wHeight = WindowMan.getWindowHeight();

	const Resolution *foundRes = 0;
	for (std::set<Resolution>::const_iterator it = availableRes.begin(); it != availableRes.end(); ++it)
		if (it->width == wWidth && it->height == wHeight)
			foundRes = &*it;

	if (!foundRes) {
		Common::UString resString;

		for (std::set<Resolution>::const_iterator it = availableRes.begin(); it != availableRes.end(); ++it) {
			if (!resString.empty())
				resString += ", ";

			resString += Common::UString::format("%dx%d", it->width, it->height);
		}

		warning("TODO: Add scaling for custom resolutions. Supported resolutions are %s", resString.c_str());
		return;
	}

	load(foundRes->gui);

	static const char * const kWidgets[] = {
		// Actions
		"LBL_HEALTHBG", "LBL_NAMEBG", "LBL_NAME", "BTN_TARGET0", "BTN_TARGET1", "BTN_TARGET2",
		"BTN_TARGETUP0", "BTN_TARGETUP1", "BTN_TARGETUP2",
		"BTN_TARGETDOWN0", "BTN_TARGETDOWN1", "BTN_TARGETDOWN2",
		// Action description
		"LBL_ACTIONDESC", "LBL_ACTIONDESCBG",
		// Combat
		"LBL_CMBTMODEMSG", "LBL_CMBTMSGBG", "LBL_COMBATBG1", "LBL_COMBATBG2", "LBL_COMBATBG3", "BTN_CLEARALL",
		// Modifiers
		"LBL_CMBTEFCTINC1", "LBL_CMBTEFCTINC2", "LBL_CMBTEFCTINC3",
		"LBL_CMBTEFCTRED1", "LBL_CMBTEFCTRED2", "LBL_CMBTEFCTRED3",
		"LBL_LEVELUP1", "LBL_LEVELUP2", "LBL_LEVELUP3",
		"LBL_LVLUPBG1", "LBL_LVLUPBG2", "LBL_LVLUPBG3",
		"LBL_DISABLE1", "LBL_DISABLE2", "LBL_DISABLE3",
		"LBL_DEBILATATED1", "LBL_DEBILATATED2", "LBL_DEBILATATED3",
		"LBL_PLOTXP", "LBL_STEALTHXP", "LBL_CASH", "LBL_JOURNAL",
		"LBL_LIGHTSHIFT", "LBL_DARKSHIFT", "LBL_ITEMRCVD", "LBL_ITEMLOST",
		// Health bar
		"PB_HEALTH"
	};

	// Make all the widgets invisible
	for (size_t i = 0; i < ARRAYSIZE(kWidgets); i++) {
		Widget *widget = getWidget(kWidgets[i]);
		if (widget)
			widget->setInvisible(true);
	}
}

void HUD::setReturnStrref(uint32 id) {
	_menu.setReturnStrref(id);
}

void HUD::setReturnQueryStrref(uint32 id) {
	_menu.setReturnQueryStrref(id);
}

void HUD::setReturnEnabled(bool enabled) {
	_menu.setReturnEnabled(enabled);
}

void HUD::setMinimap(const Common::UString &map, int northAxis,
                     float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
                     float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y) {

	WidgetLabel *mapView = getLabel("LBL_MAPVIEW");
	if (!mapView) {
		warning("No such GUI element \"LBL_MAPVIEW\"");
		return;
	}

	GfxMan.lockFrame();

	_minimap.reset(new Minimap(map, northAxis, mapPt1X, mapPt1Y, mapPt2X, mapPt2Y, worldPt1X, worldPt1Y, worldPt2X, worldPt2Y));
	mapView->setSubScene(_minimap.get());

	GfxMan.unlockFrame();
}

void HUD::setPosition(float x, float y) {
	if (_minimap)
		_minimap->setPosition(x, y);
}

void HUD::showContainer(Inventory &inv) {
	_container.reset(new ContainerMenu());
	_container->fillFromInventory(inv);

	if (sub(*_container, kStartCodeNone, true, false) == 1) {
		Inventory &partyInventory = _module->getPC()->getInventory();

		const std::map<Common::UString, InventoryItem> &items = inv.getItems();
		for (std::map<Common::UString, InventoryItem>::const_iterator i = items.begin();
				i != items.end(); ++i) {
			partyInventory.addItem(i->first, i->second.count);
		}

		inv.removeAllItems();
	}
}

void HUD::setPortrait(uint8 n, bool visible, const Common::UString &portrait) {
	WidgetLabel *labelBack = getLabel(Common::UString("LBL_BACK") + Common::composeString(n));
	if (labelBack)
		labelBack->setInvisible(!visible);

	WidgetLabel *labelChar = getLabel(Common::UString("LBL_CHAR") + Common::composeString(n));
	if (labelChar) {
		labelChar->setInvisible(!visible);
		labelChar->setFill(portrait);
	}

	WidgetProgressbar *vitals = getProgressbar(Common::UString("PB_VIT") + Common::composeString(n));
	if (vitals)
		vitals->setInvisible(!visible);
}

void HUD::setPartyLeader(Creature *creature) {
	setPortrait(1, creature != 0, creature ? creature->getPortrait() : "");
}

void HUD::setPartyMember1(Creature *creature) {
	setPortrait(2, creature != 0, creature ? creature->getPortrait() : "");
}

void HUD::setPartyMember2(Creature *creature) {
	setPortrait(3, creature != 0, creature ? creature->getPortrait() : "");
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
	_menu.showMenu(widget.getTag());
	sub(_menu);
}

} // End of namespace KotOR

} // End of namespace Engines
