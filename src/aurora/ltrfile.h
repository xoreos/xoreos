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
#include "src/common/scopedptr.h"
#include "src/common/readstream.h"

#include "src/aurora/aurorafile.h"

namespace Aurora {

/**
 * LTR File, which is used when generating player names.
 *
 * informations about the file structure are taken from
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
		Common::ScopedArray<float> start;
		Common::ScopedArray<float> mid;
		Common::ScopedArray<float> end;
	};

	Common::ScopedArray<char> _alphabet;
	byte _letterCount;

	LetterSet                                            _singleLetters;
	Common::ScopedArray<LetterSet>                       _doubleLetters;
	Common::ScopedArray<Common::ScopedArray<LetterSet> > _tripleLetters;
};

} // End of namespace Aurora

#endif // AURORA_LTRFILE_H
