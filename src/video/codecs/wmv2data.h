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
 *  Static data used for decoding WMV2 videos.
 */

/* Based on the WMV2 implementation in FFmpeg (<https://ffmpeg.org/)>,
 * which is released under the terms of version 2 or later of the GNU
 * Lesser General Public License.
 *
 * The original copyright notes in the file libavcodec/wmv2dec.c reads as follows:
 *
 * Copyright (c) 2002 The FFmpeg Project
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef VIDEO_CODECS_WMV2DATA_H
#define VIDEO_CODECS_WMV2DATA_H

#include "src/common/types.h"
#include "src/common/huffman.h"

namespace Video {

/** Parameters for decoding the DCT AC coefficients. */
struct WMV2ACCoefficientTable {
	/** Huffman decoder for the AC coefficients. */
	const Common::HuffmanTable &huffman;
	/** Escape code that signifies a special run level encoding. */
	uint32_t escapeCode;

	/** Index of the last run/level pair. */
	uint32_t lastRunIndex;

	// Run tables
	const uint8_t *runTable;          ///< Table for run values.
	const uint8_t *runDeltaTable;     ///< Table for run delta values.
	const uint8_t *runDeltaTableLast; ///< Table for run delta values, last run.

	// Level tables
	const uint8_t *levelTable;          ///< Table for level values.
	const uint8_t *levelDeltaTable;     ///< Table for level delta values.
	const uint8_t *levelDeltaTableLast; ///< Table for level delta values, last level.
};

/** Parameters for decoding the motion vectors. */
struct WMV2MVTable {
	uint32_t count; ///< Number of motion vectors.

	/** Huffman decoder for the motion vectors. */
	const Common::HuffmanTable &huffman;

	const uint8_t *diffX; ///< X difference.
	const uint8_t *diffY; ///< Y difference.
};


/** I-Frame macroblock block pattern. */
extern const Common::HuffmanTable wmv2HuffmanIMB;

/** DCT DC coefficients [luma/chroma][low/high motion]. */
extern const Common::HuffmanTable wmv2HuffmanDC[2][2];

/** DCT AC cofficients [luma/chroma][low motion/high motion/MPEG4]. */
extern const WMV2ACCoefficientTable wmv2AC[2][3];

/** P-Frame macroblock block pattern [high/mid/low rate]. */
extern const Common::HuffmanTable wmv2HuffmanPMB[3];

/** Motion vectors [low/high motion]. */
extern const WMV2MVTable wmv2MV[2];


/** I-Frame coefficient zig-zag scantable, horizontal. */
extern const uint8_t wmv2ZigZagHorizontal[64];
/** I-Frame coefficient zig-zag scantable, vertical. */
extern const uint8_t wmv2ZigZagVertical[64];
/** I-Frame coefficient zig-zag scantable, normal. */
extern const uint8_t wmv2ZigZagNormal[64];

/** P-Frame coefficient zig-zag scantable, 8x8. */
extern const uint8_t wmv2ZigZag8x8[64];
/** P-Frame coefficient zig-zag scantable, 8x4. */
extern const uint8_t wmv2ZigZag8x4[32];
/** P-Frame coefficient zig-zag scantable, 4x8. */
extern const uint8_t wmv2ZigZag4x8[32];

} // End of namespace Video

#endif // VIDEO_CODECS_WMV2DATA_H
