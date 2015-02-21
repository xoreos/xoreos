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
 *  Sound decoding utility functions.
 */

#ifndef SOUND_DECODERS_UTIL_H
#define SOUND_DECODERS_UTIL_H

#include "src/common/types.h"
#include "src/common/util.h"

namespace Sound {

// Convert one float sample into a int16 sample
static inline int16 floatToInt16(float src) {
	return (int16) CLIP<int>((int) floor(src + 0.5), -32768, 32767);
}

// Convert planar float samples into interleaved int16 samples
static inline void floatToInt16Interleave(int16 *dst, const float **src,
                                          uint32 length, uint8 channels) {
	if (channels == 2) {
		for (uint32 i = 0; i < length; i++) {
			dst[2 * i    ] = floatToInt16(src[0][i]);
			dst[2 * i + 1] = floatToInt16(src[1][i]);
		}
	} else {
		for (uint8 c = 0; c < channels; c++)
			for (uint32 i = 0, j = c; i < length; i++, j += channels)
				dst[j] = floatToInt16(src[c][i]);
	}
}

} // End of namespace Sound

#endif // SOUND_DECODERS_UTIL_H
