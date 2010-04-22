/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ssffile.h
 *  Handling BioWare's SSFs (sound set file).
 */

#ifndef AURORA_SSFFILE_H
#define AURORA_SSFFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold a sound set. */
class SSFFile : public AuroraBase {
public:
	struct Sound {
		Common::UString fileName; ///< The name of the sound file to play.
		uint32          strRef;   ///< StrRef of the text to display.
	};

	typedef std::vector<Sound> SoundSet;

	SSFFile();
	~SSFFile();

	void clear();

	/** Load a SSF file.
	 *
	 *  @param ssf A stream of a SSF file.
	 */
	void load(Common::SeekableReadStream &ssf);

	const SoundSet &getSounds() const;

private:
	SoundSet _soundSet;

	void readEntries(Common::SeekableReadStream &ssf, uint32 offset);
	void readEntries1(Common::SeekableReadStream &ssf);
	void readEntries11(Common::SeekableReadStream &ssf);
};

} // End of namespace Aurora

#endif // AURORA_SSFFILE_H
