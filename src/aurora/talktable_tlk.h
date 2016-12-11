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
 *  Handling BioWare's TLK talk tables.
 */

#ifndef AURORA_TALKTABLE_TLK_H
#define AURORA_TALKTABLE_TLK_H

#include <vector>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/aurorafile.h"
#include "src/aurora/talktable.h"

namespace Aurora {

/** Loading BioWare's TLK talk tables.
 *
 *  See class TalkTable for a general overview how talk tables work.
 *
 *  Unlike TalkTable_GFF, a TLK talk table is its own simple binary
 *  format. It has a numerical, game-local ID of the language it
 *  contains, and stores a few more optional data points per string,
 *  like a reference to a voice-over file.
 *
 *  There are two versions of TLK files known and supported
 *  - V3.0, used by Neverwinter Nights, Neverwinter Nights 2, Knight of
 *    the Old Republic, Knight of the Old Republic II and The Witcher
 *  - V4.0, used by Jade Empire
 */
class TalkTable_TLK : public AuroraFile, public TalkTable {
public:
	/** Take over this stream and read a TLK out of it. */
	TalkTable_TLK(Common::SeekableReadStream *tlk, Common::Encoding encoding);
	~TalkTable_TLK();

	/** Return the language ID (ungendered) of the talk table. */
	uint32 getLanguageID() const;

	bool hasEntry(uint32 strRef) const;

	const Common::UString &getString     (uint32 strRef) const;
	const Common::UString &getSoundResRef(uint32 strRef) const;

	uint32 getSoundID(uint32 strRef) const;

	static uint32 getLanguageID(Common::SeekableReadStream &tlk);
	static uint32 getLanguageID(const Common::UString &file);


private:
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

	typedef std::vector<Entry> Entries;


	Common::ScopedPtr<Common::SeekableReadStream> _tlk;

	uint32 _languageID;

	mutable Entries _entries;

	void load();

	void readEntryTableV3(uint32 stringsOffset);
	void readEntryTableV4();

	void readString(Entry &entry) const;
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_TLK_H
