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
 *  Handling BioWare's SSFs (sound set file).
 */

#ifndef AURORA_SSFFILE_H
#define AURORA_SSFFILE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold a sound set.
 *
 *  A sound set contains a simple array of strings and sound resource
 *  names for common creature sounds, like fighting noises, being
 *  hurt, calling for medical help, etc.
 *
 *  Which index is used for which situation differs from game to game.
 *
 *  SSF files are used in these four games: Neverwinter Nights,
 *  Neverwinter Nights 2, Knights of the Old Republic and Knights
 *  of the Old Republic II.
 */
class SSFFile : public AuroraFile {
public:
	/** A sound in the sound set. */
	struct Sound {
		Common::UString fileName; ///< The name of the sound file to play.
		uint32          strRef;   ///< StrRef of the text to display.
	};

	SSFFile(Common::SeekableReadStream &ssf);
	SSFFile(const Common::UString &ssf);
	~SSFFile();

	const Sound &getSound(uint32 index) const;

private:
	enum Version {
		kVersion10_NWN,
		kVersion11_NWN2,
		kVersion11_KotOR
	};

	typedef std::vector<Sound> SoundSet;

	Sound _emptySound;
	SoundSet _sounds;

	void load(Common::SeekableReadStream &ssf);

	/** Read the header of an SSF file and determine the version. */
	Version readSSFHeader(Common::SeekableReadStream &ssf, size_t &entryCount, size_t &offEntryTable);

	/** Read the data entries, depending on the version. */
	void readEntries(Common::SeekableReadStream &ssf, Version version, size_t offset);
	/** Read the data entries of the NWN versions. */
	void readEntriesNWN(Common::SeekableReadStream &ssf, size_t soundFileLen);
	/** Read the data entries of the KotOR version. */
	void readEntriesKotOR(Common::SeekableReadStream &ssf);
};

} // End of namespace Aurora

#endif // AURORA_SSFFILE_H
