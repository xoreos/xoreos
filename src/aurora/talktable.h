/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_TALKTABLE_H
#define AURORA_TALKTABLE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold string resoures. */
class TalkTable {
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
		std::string soundResRef;
		uint32 volumeVariance; // Unused
		uint32 pitchVariance; // Unused
		uint32 offset;
		uint32 length;
		float soundLength; // In seconds
		std::string text;
	};

	typedef std::vector<Entry> EntryList;

	TalkTable();
	~TalkTable();

	/** Clear all string information. */
	void clear();

	/** Load a talk table.
	 *
	 *  @param  stream A stream of talk table.
	 *  @return true if loading was successful, false otherwise.
	 */
	bool load(Common::SeekableReadStream &stream);

	/** Get an entry.
	 *
	 *  @param stringRef a handle to a string (index).
	 *  @return 0 if stringRef is invalid, otherwise the Entry from the list.
	 */
	const Entry *getEntry(uint32 stringRef) const;

private:
	uint32 _version; ///< The version of the file.
	EntryList _entryList;
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_H
