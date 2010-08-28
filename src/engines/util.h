/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/util.h
 *  Generic Aurora engines utility functions.
 */

#ifndef ENGINES_UTIL_H
#define ENGINES_UTIL_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "sound/types.h"

#include "graphics/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {
	class GFFFile;
}

namespace Engines {

/** Play this video resource. */
void playVideo(const Common::UString &video);

/** Play this sound resource. */
Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType, bool loop = false);

/** Wait for a "long" amount of time, returning prematurely with true in the case of a requested quit. */
bool longDelay(uint32 ms);

/** Load a GFF file. */
void loadGFF(Aurora::GFFFile &gff, const Common::UString &name, Aurora::FileType type, uint32 id);
/** Load a GFF file. */
Aurora::GFFFile *loadGFF(const Common::UString &name, Aurora::FileType type, uint32 id);

/** Debug method to quickly dump a stream to disk. */
bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);
/** Debug method to quickly dump a resource to disk. */
bool dumpResource(const Common::UString &name, Aurora::FileType type, Common::UString file = "");

/** Functor for loading models. */
class ModelLoader {
public:
	virtual Graphics::Aurora::Model *operator()(const Common::UString &resref,
	                                            const Common::UString &texture = "") const = 0;
};

} // End of namespace Engines

#endif // ENGINES_UTIL_H
