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

#include "src/graphics/windowman.h"

#include "src/engines/kotor/gui/ingame/hud.h"
#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

HUD::HUD(Engines::Console *console) : GUI(console) {
	unsigned int wWidth = WindowMan.getWindowWidth();
	unsigned int wHeight = WindowMan.getWindowHeight();

	if (wWidth == 800 && wHeight == 600)
		load("mipc28x6");
	else if (wWidth == 1024 && wHeight == 768)
		load("mipc210x7");
	else if (wWidth == 1280 && wHeight == 960)
		load("mipc212x9");
	else if (wWidth == 1600 && wHeight == 1200)
		load("mipc216x12");
	else {
		warning("TODO: Add scaling for custom resolutions. Supported resolutions are "
						"800x600, 1024x768, 1280x960 and 1600x1200");
		return;
	}

	// Make the action stuff invisible.
	getWidget("LBL_HEALTHBG")->setInvisible(true);
	getWidget("LBL_NAMEBG")->setInvisible(true);
	getWidget("LBL_NAME")->setInvisible(true);
	getWidget("BTN_TARGET0")->setInvisible(true);
	getWidget("BTN_TARGET1")->setInvisible(true);
	getWidget("BTN_TARGET2")->setInvisible(true);
	getWidget("BTN_TARGETUP0")->setInvisible(true);
	getWidget("BTN_TARGETUP1")->setInvisible(true);
	getWidget("BTN_TARGETUP2")->setInvisible(true);
	getWidget("BTN_TARGETDOWN0")->setInvisible(true);
	getWidget("BTN_TARGETDOWN1")->setInvisible(true);
	getWidget("BTN_TARGETDOWN2")->setInvisible(true);

	// Make the action description invisible
	getWidget("LBL_ACTIONDESC")->setInvisible(true);
	getWidget("LBL_ACTIONDESCBG")->setInvisible(true);

	// Whatever this combat stuff is about.
	getWidget("LBL_CMBTMODEMSG")->setInvisible(true);
	getWidget("LBL_CMBTMSGBG")->setInvisible(true);

	// Remove the modifiers.
	getWidget("LBL_CMBTEFCTINC1")->setInvisible(true);
	getWidget("LBL_CMBTEFCTINC2")->setInvisible(true);
	getWidget("LBL_CMBTEFCTINC3")->setInvisible(true);
	getWidget("LBL_CMBTEFCTRED1")->setInvisible(true);
	getWidget("LBL_CMBTEFCTRED2")->setInvisible(true);
	getWidget("LBL_CMBTEFCTRED3")->setInvisible(true);
	getWidget("LBL_LEVELUP1")->setInvisible(true);
	getWidget("LBL_LEVELUP2")->setInvisible(true);
	getWidget("LBL_LEVELUP3")->setInvisible(true);
	getWidget("LBL_LVLUPBG1")->setInvisible(true);
	getWidget("LBL_LVLUPBG2")->setInvisible(true);
	getWidget("LBL_LVLUPBG3")->setInvisible(true);
	getWidget("LBL_DISABLE1")->setInvisible(true);
	getWidget("LBL_DISABLE2")->setInvisible(true);
	getWidget("LBL_DISABLE3")->setInvisible(true);
	getWidget("LBL_DEBILATATED1")->setInvisible(true);
	getWidget("LBL_DEBILATATED2")->setInvisible(true);
	getWidget("LBL_DEBILATATED3")->setInvisible(true);

	// Make the modifiers invisible.
	getWidget("LBL_PLOTXP")->setInvisible(true);
	getWidget("LBL_STEALTHXP")->setInvisible(true);
	getWidget("LBL_CASH")->setInvisible(true);
	getWidget("LBL_JOURNAL")->setInvisible(true);
	getWidget("LBL_LIGHTSHIFT")->setInvisible(true);
	getWidget("LBL_DARKSHIFT")->setInvisible(true);
	getWidget("LBL_ITEMRCVD")->setInvisible(true);
	getWidget("LBL_ITEMLOST")->setInvisible(true);

	// Make the health bar invisible
	getWidget("PB_HEALTH")->setInvisible(true);
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

void HUD::initWidget(Engines::Widget &widget) {
	// Don't know what these two are doing, but they spawn over the complete screen blocking the 3d picking.
	if (widget.getTag() == "LBL_MAP")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_ARROW_MARGIN")
		widget.setInvisible(true);

	// TGuiPanel needs to be disabled in order to ensure, it does not block 3d picking.
	if (widget.getTag() == "TGuiPanel")
		widget.setInvisible(true);
}

void HUD::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_EQU") {
		_menu.showEquipment();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_INV") {
		_menu.showInventory();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_CHAR") {
		_menu.showCharacter();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_ABI") {
		_menu.showAbilities();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_MSG") {
		_menu.showMessages();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_JOU") {
		_menu.showJournal();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_MAP") {
		_menu.showMap();
		sub(_menu);
		return;
	}
	if (widget.getTag() == "BTN_OPT") {
		_menu.showOptions();
		sub(_menu);
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
