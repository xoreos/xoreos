/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the Ogg Vorbis implementation found in ScummVM.

#ifndef SOUND_DECODERS_VORBIS_H
#define SOUND_DECODERS_VORBIS_H

#include "common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class RewindableAudioStream;

/**
 * Create a new RewindableAudioStream from the Ogg Vorbis data in the given stream.
 *
 * @param stream			the RewindableAudioStream from which to read the Ogg Vorbis data
 * @param disposeAfterUse	whether to delete the stream after use
 * @return	a new RewindableAudioStream, or NULL, if an error occured
 */
RewindableAudioStream *makeVorbisStream(
	Common::SeekableReadStream *stream,
	DisposeAfterUse::Flag disposeAfterUse);

} // End of namespace Sound

#endif
