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

#ifndef ENGINES_KOTOR2_MODULE_H
#define ENGINES_KOTOR2_MODULE_H

#include "src/engines/kotorbase/module.h"

namespace Engines {

namespace KotORBase {
	class IngameGUI;
	class DialogGUI;
	class PartySelectionGUI;
	class LoadScreen;
}

namespace KotOR2 {

class Module : public KotORBase::Module {
public:
	Module(::Engines::Console &console);

	// GUI creation

	KotORBase::IngameGUI *createIngameGUI();
	KotORBase::DialogGUI *createDialogGUI();
	KotORBase::PartySelectionGUI *createPartySelectionGUI();
	KotORBase::LoadScreen *createLoadScreen(const Common::UString &name);

	// Object creation

	KotORBase::Creature *createCreature() const;
	KotORBase::Creature *createCreature(const Common::UString &resRef) const;
	KotORBase::Creature *createCreature(const Aurora::GFF3Struct &creature) const;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_MODULE_H
