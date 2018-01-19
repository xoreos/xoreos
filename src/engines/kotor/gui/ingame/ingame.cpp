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
 *  The ingame GUI.
 */

#include "src/engines/kotor/gui/ingame/ingame.h"

namespace Engines {

namespace KotOR {

IngameGUI::IngameGUI(Module &UNUSED(module), Console *console) {
	_hud.reset(new HUD(console));
}

void IngameGUI::show() {
	_hud->show();
}

void IngameGUI::hide() {
	_hud->hide();
}

void IngameGUI::setReturnStrref(uint32 id) {
	_hud->setReturnStrref(id);
}

void IngameGUI::setReturnQueryStrref(uint32 id) {
	_hud->setReturnQueryStrref(id);
}

void IngameGUI::setReturnEnabled(bool enabled) {
	_hud->setReturnEnabled(enabled);
}

void IngameGUI::addEvent(const Events::Event &event) {
	_hud->addEvent(event);
}

void IngameGUI::processEventQueue() {
	_hud->processEventQueue();
}

} // End of namespace KotOR

} // End of namespace Engines
