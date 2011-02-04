/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/talktable.h
 *  Handling BioWare's TLKs (talk tables).
 */

#ifndef AURORA_TALKTABLE_H
#define AURORA_TALKTABLE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold string resoures. */
class TalkTable : public AuroraBase {
public:
	/** The entries' flags. */
	enum EntryFlags {
		kTextPresent = (1 << 0),
		kSoundPresent = (1 << 1),
		kSoundLengthPresent = (1 << 2)
	};

	/** A talk resource entry. */
	struct Entry {
		EntryFlags flags;
		Common::UString soundResRef;
		uint32 volumeVariance; // Unused
		uint32 pitchVariance; // Unused
		uint32 offset;
		uint32 length;
		float soundLength; // In seconds
		Common::UString text;
	};

	typedef std::vector<Entry> EntryList;

	TalkTable();
	~TalkTable();

	/** Clear all string information. */
	void clear();

	/** Load a talk table.
	 *
	 *  @param tlk A stream of a talk table.
	 */
	void load(Common::SeekableReadStream &tlk);

	/** Get an entry.
	 *
	 *  @param strRef a handle to a string (index).
	 *  @return 0 if strRef is invalid, otherwise the Entry from the list.
	 */
	const Entry *getEntry(uint32 strRef) const;

private:
	EntryList _entryList;

	void readEntryTable(Common::SeekableReadStream &tlk);
	void readStrings(Common::SeekableReadStream &tlk, uint32 dataOffset);
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_H
