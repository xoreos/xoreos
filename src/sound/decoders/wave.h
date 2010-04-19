/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the Wave implementation found in ScummVM.

#ifndef SOUND_DECODERS_WAVE_H
#define SOUND_DECODERS_WAVE_H

#include "common/types.h"

namespace Common { class SeekableReadStream; }

namespace Sound {

class RewindableAudioStream;

/**
 * Try to load a WAVE from the given seekable stream and create an AudioStream
 * from that data. Currently this function supports uncompressed
 * raw PCM data, MS IMA ADPCM and MS ADPCM (uses makeADPCMStream internally).
 *
 * This function uses loadWAVFromStream() internally.
 *
 * @param stream			the SeekableReadStream from which to read the WAVE data
 * @param disposeAfterUse	whether to delete the stream after use
 * @return	a new RewindableAudioStream, or NULL, if an error occured
 */
RewindableAudioStream *makeWAVStream(
	Common::SeekableReadStream *stream,
	DisposeAfterUse::Flag disposeAfterUse);

} // End of namespace Sound

#endif
