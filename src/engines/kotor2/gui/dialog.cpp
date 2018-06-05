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
 *  Conversation/cutscene GUI for Star Wars: Knights of the Old
 *  Republic II: The Sith Lords.
 */

#include "src/aurora/dlgfile.cpp"

#include "src/common/maths.h"

#include "src/sound/sound.h"

#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/area.h"
#include "src/engines/kotor2/creature.h"

#include "src/engines/kotor2/gui/dialog.h"

namespace Engines {

namespace KotOR2 {

DialogGUI::DialogGUI(KotOR2::Module &module)
		: KotOR::DialogGUIBase(true),
		  _module(module) {
}

void DialogGUI::makeLookAtPC(const Common::UString &tag) {
	Creature *pc = _module.getPC();
	if (!pc)
		return;

	KotOR2::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	float ox, oy, oz;
	o->getPosition(ox, oy, oz);

	float px, py, pz;
	pc->getPosition(px, py, pz);

	float dx = px - ox;
	float dy = py - oy;

	o->setOrientation(0.0f, 0.0f, 1.0f, Common::rad2deg(std::atan2(dy, dx)) - 90.0f);
}

void DialogGUI::playDefaultAnimations(const Common::UString &tag) {
	KotOR2::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	Creature *creature = ObjectContainer::toCreature(o);
	if (!creature)
		return;

	creature->playDefaultHeadAnimation();
	creature->playDefaultAnimation();
}

void DialogGUI::playTalkAnimations(const Common::UString &tag) {
	KotOR2::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	Creature *creature = ObjectContainer::toCreature(o);
	if (!creature)
		return;

	creature->playAnimation("tlknorm", true, -1.0f);
	creature->playHeadAnimation("talk", true, -1.0f, 0.25f);
}

} // End of namespace KotOR2

} // End of namespace Engines
