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

// Largely based on the AAC implementation found in ScummVM.

/** @file sound/decoders/aac.h
 *  Decoding AAC.
 */

#ifndef SOUND_AAC_H
#define SOUND_AAC_H

#include "common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class Codec;

/**
 * Create a new Codec for decoding AAC data of an MPEG-4 file in the given stream.
 *
 * @note This should *only* be called by our QuickTime/MPEG-4 decoder since it relies
 *       on the MPEG-4 extra data. If you want to decode a file using AAC, go use
 *       makeQuickTimeStream() instead!
 * @param extraData         the SeekableReadStream from which to read the AAC extra data
 * @param disposeExtraData  whether to delete the extra data stream after use
 * @return  a new Codec, or NULL, if an error occurred
 */
Codec *makeAACDecoder(
	Common::SeekableReadStream *extraData,
	bool disposeExtraData = false);

} // End of namespace Sound

#endif // SOUND_AAC_H
