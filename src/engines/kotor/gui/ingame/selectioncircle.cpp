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
 *  The circle visible when selecting objects
 */

#include "src/engines/kotor/gui/ingame/selectioncircle.h"

namespace Engines {

namespace KotOR {

SelectionCircle::SelectionCircle() : _inactive(new Graphics::Aurora::GUIQuad("friendlyreticle", 0.0f, 0.0f, 64.0f, 64.0f)) {

}

void SelectionCircle::show() {
	_inactive->show();
}

void SelectionCircle::hide() {
	_inactive->hide();
}

void SelectionCircle::setPosition(float x, float y) {
	_inactive->setPosition(x - 32, y - 32);
}

} // End of namespace KotOR

} // End of namespace Engines
