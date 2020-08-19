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
 *  Creation and loading of saved games for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_SAVEDGAME_H
#define ENGINES_KOTORBASE_SAVEDGAME_H

#include <memory>
#include "src/common/ustring.h"

#include "src/aurora/erffile.h"
#include "src/aurora/gff3file.h"

namespace Engines {

namespace KotORBase {

class CharacterGenerationInfo;

class SavedGame {
public:
	/** Load saved game from a specified directory.
	 *
	 * @param loadSav if true load all saved state, otherwise load menu info only
	 */
	SavedGame(const Common::UString &dir, bool loadSav);
	virtual ~SavedGame();

	const Common::UString &getName() const;
	const Common::UString &getModuleName() const;
	uint32_t getTimePlayed() const;

	bool isPCLoaded() const;

	virtual CharacterGenerationInfo *createCharGenInfo() = 0;

protected:
	Common::UString _name;
	Common::UString _moduleName;
	uint32_t _timePlayed;
	uint8_t _pcGender;
	float _pcPosition[3];
	bool _pcLoaded;

private:
	void load(const Common::UString &dir, bool loadSav);

	void fillFromNFO(const Aurora::GFF3File &gff);
	void fillFromSAV(const Aurora::ERFFile &erf, const Common::UString &moduleName);
	void fillFromModuleSAV(const Aurora::ERFFile &erf);
	void fillFromModuleIFO(const Aurora::GFF3File &gff);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_SAVEDGAME_H
