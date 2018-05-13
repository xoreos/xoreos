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
 *  Creation and loading of KotOR saved games.
 */

#ifndef ENGINES_KOTOR_SAVEDGAME_H
#define ENGINES_KOTOR_SAVEDGAME_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/erffile.h"
#include "src/aurora/gff3file.h"

#include "src/engines/kotor/creature.h"

namespace Engines {

namespace KotOR {

class SavedGame {
public:
	/** Load saved game from a specified directory.
	 *
	 * @param loadSav if true load all saved state, otherwise load
	 *        menu info only
	 */
	static SavedGame *load(const Common::UString &dir, bool loadSav = false);

	SavedGame();
	const Common::UString &getName() const;
	const Common::UString &getModuleName() const;
	uint32 getTimePlayed() const;
	Creature *getPC();
	bool isPCLoaded() const;
private:
	static void fillFromNFO(const Aurora::GFF3File &gff, SavedGame *save);

	static void fillFromSAV(const Aurora::ERFFile &erf,
			const Common::UString &moduleName, SavedGame *save);

	static void fillFromModuleSAV(const Aurora::ERFFile &erf, SavedGame *save);
	static void fillFromModuleIFO(const Aurora::GFF3File &gff, SavedGame *save);

	Common::UString _name;
	Common::UString _moduleName;
	uint32 _timePlayed;
	uint8 _pcGender;
	float _pcPosition[3];
	bool _pcLoaded;
	Creature *_pc;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_SAVEDGAME_H
