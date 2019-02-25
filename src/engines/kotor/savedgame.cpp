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
 *  Creation and loading of saved games for Star Wars: Knights of the Old Republic.
 */

#include "src/engines/kotor/savedgame.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

SavedGame::SavedGame(const Common::UString &dir, bool loadSav) :
		KotORBase::SavedGame(dir, loadSav) {
}

KotORBase::CharacterGenerationInfo *SavedGame::createCharGenInfo() {
	switch (_pcGender) {
		case KotORBase::kGenderFemale:
			return CharacterGenerationInfo::createRandomFemaleSoldier();
		default:
			return CharacterGenerationInfo::createRandomMaleSoldier();
	}
}

} // End of namespace KotOR

} // End of namespace Engines
