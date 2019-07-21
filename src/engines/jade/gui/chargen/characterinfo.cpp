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
 *  Info collected throughout the character generation process.
 */

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/engines/jade/gui/chargen/characterinfo.h"

namespace Engines {

namespace Jade {

std::vector<CharacterInfo> CharacterInfo::getDefaultCharacterInfos() {
	std::vector<CharacterInfo> characters;

	// Read all players information from the 2DA table and generate info objects from it.
	const Aurora::TwoDAFile &players = TwoDAReg.get2DA("players");
	characters.resize(players.getRowCount());
	for (size_t i = 0; i < players.getRowCount(); ++i) {
		const Aurora::TwoDARow &row = players.getRow(i);

		characters[i]._name = TalkMan.getString(row.getInt("name"));
		characters[i]._defaultName = characters[i]._name;

		characters[i]._body = row.getInt("body");
		characters[i]._mind = row.getInt("mind");
		characters[i]._spirit = row.getInt("spirit");

		characters[i]._appearenceIndex = row.getInt("appearanceindex");
	}

	return characters;
}

const Common::UString &CharacterInfo::getName() const {
	return _name;
}

const Common::UString &CharacterInfo::getDefaultName() const {
	return _defaultName;
}

size_t CharacterInfo::getAppearance() const {
	return _appearenceIndex;
}

void CharacterInfo::setName(const Common::UString &name) {
	_name = name;
}

} // End of namespace Jade

} // End of namespace Engines
