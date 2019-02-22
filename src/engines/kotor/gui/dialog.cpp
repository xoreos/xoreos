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
 *  Dialog GUI for Star Wars: Knights of the Old Republic.
 */

#include "src/common/configman.h"

#include "src/events/types.h"

#include "src/graphics/windowman.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/satellitecamera.h"

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/scrollbar.h"

#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"

#include "src/engines/kotor/gui/dialog.h"

namespace Engines {

namespace KotOR {

DialogGUI::DialogGUI(KotORBase::Module &module) :
		KotORBase::DialogGUI(false),
		_module(module) {
}

void DialogGUI::makeLookAtPC(const Common::UString &tag) {
	KotORBase::Creature *pc = _module.getPC();
	if (!pc)
		return;

	KotORBase::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	// Only creatures should orient themselves to the pc.
	KotORBase::Creature *creature = KotORBase::ObjectContainer::toCreature(o);
	if (creature)
		creature->makeLookAt(pc);

	pc->makeLookAt(o);

	float x, y, z, a;
	pc->getOrientation(x, y, z, a);
	SatelliteCam.setYaw(Common::deg2rad(a - 15.0f));
}

void DialogGUI::playDefaultAnimations(const Common::UString &tag) {
	KotORBase::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	KotORBase::Creature *creature = KotORBase::ObjectContainer::toCreature(o);
	if (!creature)
		return;

	creature->playDefaultHeadAnimation();
	creature->playDefaultAnimation();
}

void DialogGUI::playTalkAnimations(const Common::UString &tag) {
	KotORBase::Object *o = _module.getCurrentArea()->getObjectByTag(tag);
	if (!o)
		return;

	KotORBase::Creature *creature = KotORBase::ObjectContainer::toCreature(o);
	if (!creature)
		return;

	creature->playAnimation("tlknorm", true, -1.0f);
	creature->playHeadAnimation("talk", true, -1.0f, 0.25f);
}

} // End of namespace KotOR

} // End of namespace Engines
