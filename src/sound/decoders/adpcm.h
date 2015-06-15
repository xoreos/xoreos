/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

// Largely based on the ADPCM implementation found in ScummVM.

/** @file
 *  Decoding ADPCM (Adaptive Differential Pulse Code Modulation).
 */

#ifndef SOUND_DECODERS_ADPCM_H
#define SOUND_DECODERS_ADPCM_H

#include "src/common/readstream.h"

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
 * @param stream            The SeekableReadStream from which to read the ADPCM data.
 * @param disposeAfterUse   Whether to delete the stream after use.
 * @param size              How many bytes to read from the stream (0 = all).
 * @param type              The compression type used.
 * @param rate              The sampling rate.
 * @param channels          The number of channels.
 * @param blockAlign        Block alignment ???
 *
 * @return A new RewindableAudioStream, or 0, if an error occured.
 */
RewindableAudioStream *makeADPCMStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse,
	uint32 size, ADPCMTypes type,
	int rate,
	int channels,
	uint32 blockAlign = 0);

} // End of namespace Sound

#endif // SOUND_DECODERS_ADPCM_H
