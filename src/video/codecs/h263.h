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
 *  h.263 video codec.
 */

#ifndef VIDEO_CODECS_H263_H
#define VIDEO_CODECS_H263_H

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef ENABLE_XVIDCORE

namespace Common {
class SeekableReadStream;
} // End of namespace Common

namespace Video {

class Codec;

/**
 * Create a Codec capable of decoding h.263 frames
 */
Codec *makeH263Codec(int width, int height, Common::SeekableReadStream &extraData);

} // End of namespace Video

#endif // ENABLE_XVIDCORE

#endif // VIDEO_CODECS_H263_H
