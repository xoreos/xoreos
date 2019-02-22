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
 *  A KotOR2 GUI.
 */

#include "src/graphics/windowman.h"

#include "src/engines/odyssey/widget.h"

#include "src/engines/kotor2/gui/gui.h"

namespace Engines {

namespace KotOR2 {

GUI::GUI(::Engines::Console *console) : KotORBase::GUI(console) {
}

void GUI::initWidget(Widget &widget) {
	Odyssey::Widget &kotorWidget = static_cast<Odyssey::Widget &>(widget);

	float wWidth = WindowMan.getWindowWidth();
	float wHeight = WindowMan.getWindowHeight();

	if (widget.getTag() == "TGuiPanel") {
		kotorWidget.setWidth(wWidth);
		kotorWidget.setHeight(wHeight);
	} else {
		float x, y, z;
		kotorWidget.getPosition(x, y, z);

		x *= ((wWidth / 2.0f) / 400.0f);
		y *= ((wHeight / 2.0f) / 300.0f);

		kotorWidget.setPosition(std::floor(x), std::floor(y), z);

		float w, h;
		w = kotorWidget.getWidth();
		h = kotorWidget.getHeight();

		w *= (wWidth / 800.0f);
		h *= (wHeight / 600.0f);

		kotorWidget.setWidth(std::floor(w));
		kotorWidget.setHeight(std::floor(h));
	}
}

} // End of namespace KotOR2

} // End of namespace Engines
