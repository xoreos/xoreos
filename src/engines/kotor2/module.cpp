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
 *  The context needed to run a Star Wars: Knights of the Old Republic II - The Sith Lords module.
 */

#include "src/engines/kotorbase/gui/loadscreen.h"

#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/creature.h"

#include "src/engines/kotor2/gui/dialog.h"

#include "src/engines/kotor2/gui/ingame/ingame.h"
#include "src/engines/kotor2/gui/ingame/partyselection.h"

#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

Module::Module(::Engines::Console &console) : KotORBase::Module(console) {
	_ingame.reset(new IngameGUI(*this));
	_dialog.reset(new DialogGUI(*this));
	_partySelection.reset(new PartySelectionGUI());

	loadTexturePack();
}

KotORBase::LoadScreen *Module::createLoadScreen(const Common::UString &name) const {
	return new KotORBase::LoadScreen(name);
}

KotORBase::Creature *Module::createCreature(const Aurora::GFF3Struct &creature) const {
	return new Creature(creature);
}

KotORBase::Creature *Module::createCreature() const {
	return new Creature();
}

KotORBase::Creature *Module::createCreature(const Common::UString &resRef) const {
	return new Creature(resRef);
}

KotORBase::CharacterGenerationInfo *Module::createCharGenInfo(const KotORBase::CharacterGenerationInfo &info) const {
	return new CharacterGenerationInfo(info);
}

} // End of namespace KotOR2

} // End of namespace Engines
