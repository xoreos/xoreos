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

#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/gui/widgets/kotorwidget.h"
#include "src/engines/kotor2/gui/ingame/hud.h"

namespace Engines {

namespace KotOR2 {

HUD::HUD(Module &UNUSED(module), ::Engines::Console *console) : GUI(console) {
	unsigned int wWidth = WindowMan.getWindowWidth();
	unsigned int wHeight = WindowMan.getWindowHeight();
	if (wWidth == 800 && wHeight == 600)
		load("mipc28x6_p");
	else {
		warning("TODO: Add scaling for custom resolutions. The supported resolution is 800x600");
		return;
	}
}

void HUD::initWidget(Widget &widget) {
	// Don't know what these two are doing, but they spawn over the complete screen blocking the 3d picking.
	if (widget.getTag() == "LBL_MAP")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_ARROW_MARGIN")
		widget.setInvisible(true);

	// Make the action stuff invisible.
	if (widget.getTag().contains("TARGET"))
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_NAMEBG")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_NAME")
		widget.setInvisible(true);
	if (widget.getTag().contains("HEALTH"))
		widget.setInvisible(true);
	if (widget.getTag().contains("ACTIONDESC"))
		widget.setInvisible(true);

	// Make all combat related stuff invisible.
	if (widget.getTag().contains("CMBTM") || widget.getTag().contains("COMBAT"))
		widget.setInvisible(true);
	if (widget.getTag().contains("CLEAR"))
		widget.setInvisible(true);

	// Disable all portrait modifiers.
	if (widget.getTag().contains("EFCT"))
		widget.setInvisible(true);
	if (widget.getTag().contains("LVLUP"))
		widget.setInvisible(true);
	if (widget.getTag().contains("LEVELUP"))
		widget.setInvisible(true);
	if (widget.getTag().contains("DISABLE"))
		widget.setInvisible(true);
	if (widget.getTag().contains("DEBILATATED"))
		widget.setInvisible(true);

	// Make the event markers invisible.
	if (widget.getTag() == "LBL_PLOTXP")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_STEALTHXP")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_CASH")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_JOURNAL")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_LIGHTSHIFT")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_DARKSHIFT")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_ITEMLOST")
		widget.setInvisible(true);
	if (widget.getTag() == "LBL_ITEMRCVD")
		widget.setInvisible(true);

	// TGuiPanel needs to be disabled in order to ensure, it does not block 3d picking.
	if (widget.getTag() == "TGuiPanel")
		widget.setInvisible(true);
}

} // End of namespace KotOR2

} // End of namespace Engines
