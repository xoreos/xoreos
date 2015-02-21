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
 *  Handling BioWare's TLKs (talk tables).
 */

#ifndef AURORA_TALKTABLE_H
#define AURORA_TALKTABLE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold string resoures. */
class TalkTable : public AuroraBase {
public:
	/** The entries' flags. */
	enum EntryFlags {
		kFlagTextPresent        = (1 << 0),
		kFlagSoundPresent       = (1 << 1),
		kFlagSoundLengthPresent = (1 << 2)
	};

	/** A talk resource entry. */
	struct Entry {
		Common::UString text;
		uint32 offset;
		uint32 length;

		// V3
		uint32 flags;
		Common::UString soundResRef;
		uint32 volumeVariance; // Unused
		uint32 pitchVariance; // Unused
		float soundLength; // In seconds

		// V4
		uint32 soundID;
	};

	typedef std::vector<Entry> EntryList;

	TalkTable(Common::SeekableReadStream *tlk);
	~TalkTable();

	/** Return the language ID (ungendered) of the talk table. */
	uint getLanguageID() const;

	/** Get an entry.
	 *
	 *  @param strRef a handle to a string (index).
	 *  @return 0 if strRef is invalid, otherwise the Entry from the list.
	 */
	const Entry *getEntry(uint32 strRef);

private:
	Common::SeekableReadStream *_tlk;

	uint32 _stringsOffset;

	uint32 _language;

	EntryList _entryList;

	void load();

	void readEntryTableV3();
	void readEntryTableV4();
	void readString(Entry &entry);
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_H
