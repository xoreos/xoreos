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
 *  File for creating random names in the character generation.
 */

#ifndef AURORA_LTRFILE_H
#define AURORA_LTRFILE_H

#include <vector>

#include "src/common/ustring.h"
#include "src/common/readstream.h"

#include "src/aurora/aurorafile.h"

namespace Aurora {

/**
 * LTR File, which is used when generating player names.
 *
 * Information about the file structure was taken from
 * https://github.com/mtijanic/nwn-misc
 */
class LTRFile : public AuroraFile {
public:
	LTRFile(const Common::UString &ltr);
	LTRFile(Common::SeekableReadStream &stream);

	/** Get the letter count utilized by the ltr file. */
	size_t getLetterCount() const;

	/** Generate a random name from the ltr file. */
	Common::UString generateRandomName(size_t maxLetters) const;

private:
	void load(Common::SeekableReadStream &stream);

	struct LetterSet {
		std::vector<float> start;
		std::vector<float> mid;
		std::vector<float> end;
	};

	std::vector<char> _alphabet;
	size_t _letterCount;

	LetterSet                           _singleLetters;
	std::vector<LetterSet>              _doubleLetters;
	std::vector<std::vector<LetterSet>> _trippleLetters;

	static void readLetterSet(LetterSet &letters, size_t count, Common::SeekableReadStream &stream);
};

} // End of namespace Aurora

#endif // AURORA_LTRFILE_H
