/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the MP3 implementation found in ScummVM.

/** @file sound/decoders/mp3.h
 *  Decoding MP3 (MPEG-1 Audio Layer 3).
 */

#ifndef SOUND_DECODERS_MP3_H
#define SOUND_DECODERS_MP3_H

#include "common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class AudioStream;
class RewindableAudioStream;

/**
 * Create a new SeekableAudioStream from the MP3 data in the given stream.
 * Allows for seeking (which is why we require a SeekableReadStream).
 *
 * @param stream			the SeekableReadStream from which to read the MP3 data
 * @param disposeAfterUse	whether to delete the stream after use
 * @return	a new SeekableAudioStream, or NULL, if an error occured
 */
RewindableAudioStream *makeMP3Stream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse);

} // End of namespace Sound

#endif
