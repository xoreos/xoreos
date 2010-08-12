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

#include "sound/types.h"

namespace Engines {

/** Play this video resource. */
void playVideo(const Common::UString &video);

/** Play this sound resource. */
Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType, bool loop = false);

/** Wait for a "long" amount of time, returning prematurely with true in the case of a requested quit. */
bool longDelay(uint32 ms);

/** Debug method to quickly dump a stream to disk. */
bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);

} // End of namespace Engines

#endif // ENGINES_UTIL_H
