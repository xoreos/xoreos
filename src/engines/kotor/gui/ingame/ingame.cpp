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

IngameGUI::IngameGUI(Module &module, Console *console) {
	_hud.reset(new HUD(module, console));
}

void IngameGUI::show() {
	_hud->show();
}

void IngameGUI::hide() {
	_hud->hide();
}

void IngameGUI::setMinimap(const Common::UString &map, int northAxis,
                           float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
                           float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y) {

	_hud->setMinimap(map, northAxis, worldPt1X, worldPt1Y, worldPt2X, worldPt2Y, mapPt1X, mapPt1Y, mapPt2X, mapPt2Y);
}

void IngameGUI::setPosition(float x, float y) {
	_hud->setPosition(x, y);
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

void IngameGUI::showContainer() {
	_hud->showContainer();
}

void IngameGUI::setPartyLeader(Creature *creature) {
	_hud->setPartyLeader(creature);
}

void IngameGUI::setPartyMember1(Creature *creature) {
	_hud->setPartyMember1(creature);
}

void IngameGUI::setPartyMember2(Creature *creature) {
	_hud->setPartyMember2(creature);
}

void IngameGUI::addEvent(const Events::Event &event) {
	_hud->addEvent(event);
}

void IngameGUI::processEventQueue() {
	_hud->processEventQueue();
}

} // End of namespace KotOR

} // End of namespace Engines
