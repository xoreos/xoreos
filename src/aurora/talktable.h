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
 *  Base class for BioWare's talk tables.
 */

#ifndef AURORA_TALKTABLE_H
#define AURORA_TALKTABLE_H

#include "src/common/types.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Aurora {

/** Base class for BioWare's talk tables. */
class TalkTable {
public:
	virtual ~TalkTable();

	/** Return the language ID (ungendered) of the talk table. */
	virtual uint32 getLanguageID() const = 0;

	virtual bool hasEntry(uint32 strRef) const = 0;

	virtual const Common::UString &getString     (uint32 strRef) const = 0;
	virtual const Common::UString &getSoundResRef(uint32 strRef) const = 0;

	static TalkTable *load(Common::SeekableReadStream *tlk, uint32 languageID = 0xFFFFFFFF);


protected:
	TalkTable();
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_H
