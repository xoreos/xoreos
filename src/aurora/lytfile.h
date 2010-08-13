/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/lytfile.h
 *  Handling BioWare's LYTs (Layout files).
 */

#ifndef AURORA_LYTFILE_H
#define AURORA_LYTFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** An LYT, BioWare's Layout Format. */
class LYTFile {
public:
	struct Room {
		Common::UString model;
		float x, y, z;
	};

	struct DoorHook {
		Common::UString name;
		int unk0;
		float x, y, z;
		float unk1, unk2, unk3, unk4, unk5;
	};

	typedef std::vector<Room> RoomArray;
	typedef std::vector<DoorHook> DoorHookArray;

	LYTFile();
	~LYTFile();

	/** Clear all information. */
	void clear();

	/** Load a LYT file.
	 *
	 *  @param lyt A stream of an LYT file.
	 */
	void load(Common::SeekableReadStream &lyt);

	/** Get all rooms in this layout. */
	const RoomArray &getRooms() const;

	/** Get all door hooks in this layout. */
	const DoorHookArray &getDoorHooks() const;

	/** Get the file dependency in this layout. */
	Common::UString getFileDependency() const;

private:
	RoomArray _rooms;
	DoorHookArray _doorHooks;
	Common::UString _fileDependency;
};

} // End of namespace Aurora

#endif // AURORA_LYTFILE_H
