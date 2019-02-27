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

#include "src/engines/kotor2/gui/ingame/ingame.h"

namespace Engines {

namespace KotOR2 {

IngameGUI::IngameGUI(KotORBase::Module &module, Console *console) {
	_hud.reset(new HUD(module, console));
}

void IngameGUI::show() {
	_hud->show();
}

void IngameGUI::hide() {
	_hud->hide();
}

void IngameGUI::setMinimap(const Common::UString &UNUSED(map), int UNUSED(northAxis),
	                       float UNUSED(worldPt1X), float UNUSED(worldPt1Y), float UNUSED(worldPt2X), float UNUSED(worldPt2Y),
	                       float UNUSED(mapPt1X), float UNUSED(mapPt1Y), float UNUSED(mapPt2X), float UNUSED(mapPt2Y)) {
}

void IngameGUI::setPosition(float UNUSED(x), float UNUSED(y)) {
}

void IngameGUI::setRotation(float UNUSED(angle)) {
}

void IngameGUI::setReturnStrref(uint32 UNUSED(id)) {
}

void IngameGUI::setReturnQueryStrref(uint32 UNUSED(id)) {
}

void IngameGUI::setReturnEnabled(bool UNUSED(enabled)) {
}

void IngameGUI::showContainer(KotORBase::Inventory &UNUSED(inv)) {
}

void IngameGUI::setPartyLeader(KotORBase::Creature *UNUSED(creature)) {
}

void IngameGUI::setPartyMember1(KotORBase::Creature *UNUSED(creature)) {
}

void IngameGUI::setPartyMember2(KotORBase::Creature *UNUSED(creature)) {
}

KotORBase::Object *IngameGUI::getHoveredObject() const {
	return 0;
}

KotORBase::Object *IngameGUI::getTargetObject() const {
	return 0;
}

void IngameGUI::setHoveredObject(KotORBase::Object *UNUSED(object)) {
}

void IngameGUI::setTargetObject(KotORBase::Object *UNUSED(object)) {
}

void IngameGUI::updateSelection() {
}

void IngameGUI::hideSelection() {
}

void IngameGUI::resetSelection() {
}

void IngameGUI::addEvent(const Events::Event &event) {
	_hud->addEvent(event);
}

void IngameGUI::processEventQueue() {
	_hud->processEventQueue();
}

} // End of namespace KotOR

} // End of namespace Engines
