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
 *  WMV2 video codec, XMV variant.
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

#ifndef VIDEO_CODECS_XMVWMV2_H
#define VIDEO_CODECS_XMVWMV2_H

#include <memory>

#include "src/common/types.h"

#include "src/video/codecs/codec.h"

namespace Common {
	class BitStream;
	class Huffman;
}

namespace Video {

struct WMV2ACCoefficientTable;
struct WMV2MVTable;

class XMVWMV2Codec : public Codec {
public:
	XMVWMV2Codec(uint32_t width, uint32_t height, Common::SeekableReadStream &extraData);
	~XMVWMV2Codec();

	void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &dataStream);

private:
	static const uint32_t kMacroBlockSize = 16; ///< Size of a macro block.
	static const uint32_t kBlockSize      =  8; ///< Size of a block.

	/** Which block pattern are coded? */
	class CBP {
	public:
		CBP(uint32_t cbp = 0);

		void clear();

		bool empty() const;
		bool isSet(int block) const;

		void set(int block, bool coded);

		void decode(uint32_t cbp);
		void decode(uint32_t cbp, const CBP &topLeft, const CBP &top, const CBP &left);

	private:
		uint8_t _cbp;
	};

	/** Decoders for DCT AC coefficients. */
	struct ACDecoder {
		std::unique_ptr<Common::Huffman> huffman;
		const WMV2ACCoefficientTable *parameters;
	};

	/** Decoder for motion vectors. */
	struct MVDecoder {
		std::unique_ptr<Common::Huffman> huffman;
		const WMV2MVTable *parameters;
	};

	/** Context for decoding a block. */
	struct BlockContext {
		const byte *refPlane;
		      byte *curPlane;

		int32_t *acQuantTop;
		int32_t *acQuantLeft;

		int32_t *dcTopLeft;

		ACDecoder       **decoderAC;
		Common::Huffman **huffDC;

		uint32_t dcEscapeCode;

		bool hasACCoeffs;

		uint32_t planePitch;
		uint32_t blockPitch;
	};

	/** Context for decoding a frame. */
	struct DecodeContext {
		Common::BitStream &bits;

		int32_t qScale;
		int32_t dcStepSize;
		int32_t defaultPredictor;

		int32_t *acQuantTop[3];
		int32_t  acQuantLeft[4][kBlockSize];

		int32_t dcTopLeft[4];

		ACDecoder       *decoderAC[2];
		Common::Huffman *huffDC   [2];

		CBP *rowCBP;
		CBP *curCBP;

		CBP cbpTopLeft;
		CBP cbpTop;

		bool hasACPerMacroBlock;
		bool hasACPrediction;

		uint32_t acRLERunLength;
		uint32_t acRLELevelLength;

		BlockContext block[6];


		DecodeContext(Common::BitStream &b);

		/** Set the quantizer scale and calculate the DC step size and default predictor. */
		void setQScale(int32_t qS);

		void startRow();
		void finishRow();

		void startMacroBlock(uint32_t cbp);
		void finishMacroBlock();
	};


	// Dimensions

	uint32_t _width;  ///< Width of a frame.
	uint32_t _height; ///< Height of a frame.

	uint32_t _lumaWidth;  ///< Width of the luma portion of a decoded frame.
	uint32_t _lumaHeight; ///< Height of the luma portion of a decoded frame.

	uint32_t _chromaWidth;  ///< Width of the chroma portion of a decoded frame.
	uint32_t _chromaHeight; ///< Height of the chroma portion of a decoded frame.

	uint32_t _mbCountWidth;  ///< Width of a frame in macro blocks.
	uint32_t _mbCountHeight; ///< Height of a frame in macro blocks.

	// Color planes

	std::unique_ptr<byte[]> _curPlanes[3]; ///< The 3 color planes, YUV, current frame.
	std::unique_ptr<byte[]> _oldPlanes[3]; ///< The 3 color planes, YUV, last frame.

	// Decoder flags

	bool _hasMixedPelMC;      ///< Does the video have mixed pel motion compensation?
	bool _hasLoopFilter;      ///< Does the video use the loop filter?
	bool _hasVarSizeTrans;    ///< Does the video have variable sized transforms?
	bool _hasJFrames;         ///< Does the video have j-frames (IntraX8)?
	bool _hasHybridMV;        ///< Does the video have hybrid motion vectors?
	bool _hasACPerMacroBlock; ///< Are the AC selected per macro block instead of per frame?

	uint8_t _sliceCount; ///< Number of slices per frame.

	// CBP

	std::unique_ptr<CBP[]>           _cbp;        ///< Coded block pattern, previous row.
	std::unique_ptr<Common::Huffman> _huffCBP[4]; ///< Huffman codes for coded block pattern.

	// DCT DC coefficients

	/** Huffman code for DCT DC coefficients, [luma/chroma][low/high motion]. */
	std::unique_ptr<Common::Huffman> _huffDC[2][2];

	// DCT AC coefficients

	std::unique_ptr<int32_t[]> _predAC[3]; ///< AC predictors, previous row.

	/** Decoders for DCT AC coefficients [luma/chroma][low motion/high motion/MPEG4]. */
	ACDecoder _decoderAC[2][3];

	// Motion vectors

	/** Huffman code for the motion vectors [low/high motion]. */
	MVDecoder _decoderMV[2];

	uint32_t _currentFrame;


	/** Init the decoder. */
	void init();

	/** Parse the extra data containing video encoding properties. */
	void parseExtraData(Common::SeekableReadStream &extraData);

	void initDecodeContext(DecodeContext &ctx);

	// Frame type decoders

	/** Decode an I-Frame (intra frame). */
	void decodeIFrame(DecodeContext &ctx);
	/** Decode a J-Frame (intra X8 frame). */
	void decodeJFrame(DecodeContext &ctx);
	/** Decode a P-Frame (inter frame). */
	void decodePFrame(DecodeContext &ctx);

	// Block decoders

	/** Decode an I-Frame (intra frame) macro block. */
	void decodeIMacroBlock(DecodeContext &ctx);

	/** Decode an I-Frame (intra frame) block. */
	void decodeIBlock(DecodeContext &ctx, BlockContext &block);

	/** Decode a "tri-state". */
	static uint8_t getTrit(Common::BitStream &bits);

	// IDCT

	void IDCTPut(byte *dest, int32_t *block, uint32_t pitch);
	void IDCT(int32_t *block);
	void IDCTRow(int32_t *b);
	void IDCTCol(int32_t *b);
};

} // End of namespace Video

#endif // VIDEO_CODECS_XMVWMV2_H
