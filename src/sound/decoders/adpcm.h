/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the ADPCM implementation found in ScummVM.

#ifndef SOUND_DECODERS_ADPCM_H
#define SOUND_DECODERS_ADPCM_H

#include "common/stream.h"

namespace Sound {

class RewindableAudioStream;

// There are several types of ADPCM encoding, only some are supported here
// For all the different encodings, refer to:
// http://wiki.multimedia.cx/index.php?title=Category:ADPCM_Audio_Codecs
// Usually, if the audio stream we're trying to play has the FourCC header
// string intact, it's easy to discern which encoding is used
enum ADPCMTypes {
	kADPCMMSIma,	// Microsoft IMA ADPCM
	kADPCMMS		// Microsoft ADPCM
};

/**
 * Takes an input stream containing ADPCM compressed sound data and creates
 * an RewindableAudioStream from that.
 *
 * @param stream            the SeekableReadStream from which to read the ADPCM data
 * @param disposeAfterUse   whether to delete the stream after use
 * @param size              how many bytes to read from the stream (0 = all)
 * @param type              the compression type used
 * @param rate              the sampling rate
 * @param channels          the number of channels
 * @param blockAlign        block alignment ???
 * @return   a new RewindableAudioStream, or NULL, if an error occured
 */
RewindableAudioStream *makeADPCMStream(
    Common::SeekableReadStream *stream,
    DisposeAfterUse::Flag disposeAfterUse,
    uint32 size, ADPCMTypes type,
    int rate,
    int channels,
    uint32 blockAlign = 0);

}

#endif
