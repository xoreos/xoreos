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
 *  VP8/VP9 codec class.
 */

#ifndef VIDEO_CODECS_VPX_H
#define VIDEO_CODECS_VPX_H

namespace Video {

class Codec;

/**
 * Create a codec capable of decoding VP8 frames
 *
 * @return a new Codec, or NULL if an error occurred
 */
Codec *makeVP8Decoder();

/**
 * Create a codec capable of decoding VP9 frames
 *
 * @return a new Codec, or NULL if an error occurred
 */
Codec *makeVP9Decoder();

} // End of namespace Video

#endif
