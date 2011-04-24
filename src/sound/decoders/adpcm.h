/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Largely based on the ADPCM implementation found in ScummVM.

/** @file sound/decoders/adpcm.h
 *  Decoding ADPCM (Adaptive Differential Pulse Code Modulation).
 */

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
	kADPCMMSIma,    // Microsoft IMA ADPCM
	kADPCMMS,       // Microsoft ADPCM
	kADPCMApple     // Apple QuickTime IMA ADPCM
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
	bool disposeAfterUse,
	uint32 size, ADPCMTypes type,
	int rate,
	int channels,
	uint32 blockAlign = 0);

}

#endif
