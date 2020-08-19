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

/** @file
 *  Decoding ADPCM (Adaptive Differential Pulse Code Modulation).
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#ifndef SOUND_DECODERS_ADPCM_H
#define SOUND_DECODERS_ADPCM_H

#include "src/common/readstream.h"

namespace Sound {

class PacketizedAudioStream;
class RewindableAudioStream;

// There are several types of ADPCM encoding, only some are supported here
// For all the different encodings, refer to:
// http://wiki.multimedia.cx/index.php?title=Category:ADPCM_Audio_Codecs
// Usually, if the audio stream we're trying to play has the FourCC header
// string intact, it's easy to discern which encoding is used
enum ADPCMTypes {
	kADPCMMSIma,    // Microsoft IMA ADPCM
	kADPCMMS,       // Microsoft ADPCM
	kADPCMApple,    // Apple QuickTime IMA ADPCM
	kADPCMXbox      // Microsoft Xbox ADPCM
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
 * @return A new RewindableAudioStream, or 0, if an error occurred.
 */
RewindableAudioStream *makeADPCMStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse,
	uint32_t size, ADPCMTypes type,
	int rate,
	int channels,
	uint32_t blockAlign = 0);

/**
 * Creates a PacketizedAudioStream that will automatically queue
 * packets as individual AudioStreams like returned by makeADPCMStream.
 *
 * Due to the ADPCM types not necessarily supporting stateless
 * streaming, OKI and DVI are not supported by this function
 * and will return NULL.
 *
 * @param type              the compression type used
 * @param rate              the sampling rate
 * @param channels          the number of channels
 * @param blockAlign        block alignment ???
 * @return The new PacketizedAudioStream or NULL, if the type isn't supported.
 */
PacketizedAudioStream *makePacketizedADPCMStream(
	ADPCMTypes type,
	int rate,
	int channels,
	uint32_t blockAlign = 0);

} // End of namespace Sound

#endif // SOUND_DECODERS_ADPCM_H
