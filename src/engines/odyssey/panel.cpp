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
 *  Panel widget for the Odyssey engine.
 */

#include "src/engines/odyssey/panel.h"

namespace Engines {

namespace Odyssey {

WidgetPanel::WidgetPanel(GUI &gui, const Common::UString &tag) : Widget(gui, tag) {
}

WidgetPanel::WidgetPanel(GUI &gui,
                         const Common::UString &tag,
                         const Common::UString &texture,
                         float x, float y, float w, float h) : Widget(gui, tag) {
	_width  = w;
	_height = h;

	Engines::Widget::setPosition(x, y, 0.0f);

	_quad.reset(new Graphics::Aurora::GUIQuad(texture, 0.0f, 0.0f, w, h));
	_quad->setPosition(x, y, 0.0f);
}

} // End of namespace Odyssey

} // End of namespace Engines
