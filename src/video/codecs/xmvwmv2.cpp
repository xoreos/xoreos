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

#include <cassert>
#include <cstring>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/bitstream.h"
#include "src/common/huffman.h"

#include "src/graphics/yuv_to_rgb.h"

#include "src/graphics/images/surface.h"

#include "src/video/codecs/wmv2data.h"
#include "src/video/codecs/xmvwmv2.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

namespace Video {

static const uint32 kEscapeCodeLuma   = 119;
static const uint32 kEscapeCodeChroma = 119;

static const uint8 kSkipTypeNone = 0;
static const uint8 kSkipTypeMPEG = 1;
static const uint8 kSkipTypeRow  = 2;
static const uint8 kSkipTypeCol  = 3;


XMVWMV2Codec::CBP::CBP(uint32 cbp) {
	decode(cbp);
}

void XMVWMV2Codec::CBP::clear() {
	_cbp = 0;
}

bool XMVWMV2Codec::CBP::empty() const {
	return _cbp == 0;
}

bool XMVWMV2Codec::CBP::isSet(int block) const {
	assert((block >= 0) && (block < 6));

	return (_cbp & (1 << (5 - block))) != 0;
}

void XMVWMV2Codec::CBP::set(int block, bool coded) {
	_cbp &= ~(    1 << (5 - block));
	_cbp |=   coded << (5 - block);
}

void XMVWMV2Codec::CBP::decode(uint32 cbp) {
	_cbp = cbp & 0x3F;
}

void XMVWMV2Codec::CBP::decode(uint32 cbp, const CBP &topLeft, const CBP &top,
                                           const CBP &left) {
	const CBP x(cbp);

	set(0, ((topLeft.isSet(3) == top.isSet(2)) ? left.isSet(1) : top.isSet(2)) ^ x.isSet(0));
	set(1, ((top    .isSet(2) == top.isSet(3)) ?      isSet(0) : top.isSet(3)) ^ x.isSet(1));
	set(2, ((left   .isSet(1) ==     isSet(0)) ? left.isSet(3) :     isSet(0)) ^ x.isSet(2));
	set(3, ((        isSet(0) ==     isSet(1)) ?      isSet(2) :     isSet(1)) ^ x.isSet(3));
	set(4,                                                                       x.isSet(4));
	set(5,                                                                       x.isSet(5));
}


XMVWMV2Codec::DecodeContext::DecodeContext(Common::BitStream &b) : bits(b),
	hasACPerMacroBlock(false), hasACPrediction(false),
	acRLERunLength(0), acRLELevelLength(0) {

	decoderAC[0] = decoderAC[1] = 0;
	huffDC   [0] = huffDC   [1] = 0;

	for (int i = 0; i < 4; i++) {
		block[i].acQuantLeft = acQuantLeft[i >> 1];

		block[i].dcTopLeft = &dcTopLeft[i >> 1];

		block[i].decoderAC = &decoderAC[0];
		block[i].huffDC    = &huffDC   [0];

		block[i].dcEscapeCode = kEscapeCodeLuma;

		block[i].hasACCoeffs = false;
	}

	for (int i = 4; i < 6; i++) {
		block[i].acQuantLeft = acQuantLeft[i - 2];

		block[i].dcTopLeft = &dcTopLeft[i - 2];

		block[i].decoderAC = &decoderAC[1];
		block[i].huffDC    = &huffDC   [1];

		block[i].dcEscapeCode = kEscapeCodeChroma;

		block[i].hasACCoeffs = false;
	}
}

void XMVWMV2Codec::DecodeContext::setQScale(int32 qS) {
	qScale = qS;

	dcStepSize = 8;
	if (qScale > 4)
		dcStepSize = qScale / 2 + 6;

	defaultPredictor = (1024 + (dcStepSize / 2)) / dcStepSize;

	if (acQuantTop[0] && acQuantTop[1] && acQuantTop[2]) {
		std::memset(acQuantTop[0], 0, sizeof(int32) * block[0].planePitch);
		std::memset(acQuantTop[1], 0, sizeof(int32) * block[4].planePitch);
		std::memset(acQuantTop[2], 0, sizeof(int32) * block[5].planePitch);

		for (uint32 x = 0; x < (block[0].planePitch / block[0].blockPitch); x++) {
			acQuantTop[0][x * block[0].blockPitch + block[4].blockPitch] = defaultPredictor;
			acQuantTop[0][x * block[0].blockPitch                      ] = defaultPredictor;

			acQuantTop[1][x * block[4].blockPitch] = defaultPredictor;
			acQuantTop[2][x * block[5].blockPitch] = defaultPredictor;
		}
	}
}

void XMVWMV2Codec::DecodeContext::startRow() {
	for (int i = 0; i < 4; i++)
		block[i].acQuantTop = acQuantTop[0] + kBlockSize * (i & 1);

	for (int i = 4; i < 6; i++)
		block[i].acQuantTop = acQuantTop[i - 3];

	std::memset(acQuantLeft, 0, sizeof(int32) * kBlockSize * 4);

	acQuantLeft[0][0] = defaultPredictor;
	acQuantLeft[1][0] = defaultPredictor;
	acQuantLeft[2][0] = defaultPredictor;
	acQuantLeft[3][0] = defaultPredictor;

	dcTopLeft[0] = defaultPredictor;
	dcTopLeft[1] = 0;
	dcTopLeft[2] = defaultPredictor;
	dcTopLeft[3] = defaultPredictor;

	curCBP = rowCBP;

	cbpTopLeft.clear();
}

void XMVWMV2Codec::DecodeContext::finishRow() {
	for (int i = 0; i < 6; i++) {
		block[i].refPlane += block[i].planePitch * (block[i].blockPitch - 1);
		block[i].curPlane += block[i].planePitch * (block[i].blockPitch - 1);
	}
}

void XMVWMV2Codec::DecodeContext::startMacroBlock(uint32 cbp) {
	cbpTop = curCBP[0];

	curCBP[0].decode(cbp, cbpTopLeft, cbpTop, curCBP[-1]);

	cbpTopLeft = cbpTop;
}

void XMVWMV2Codec::DecodeContext::finishMacroBlock() {
	for (int i = 0; i < 6; i++) {
		block[i].refPlane   += block[i].blockPitch;
		block[i].curPlane   += block[i].blockPitch;
		block[i].acQuantTop += block[i].blockPitch;
	}

	curCBP++;
}


XMVWMV2Codec::XMVWMV2Codec(uint32 width, uint32 height,
                           Common::SeekableReadStream &extraData) :
	_width(width), _height(height), _currentFrame(0) {

	init();

	parseExtraData(extraData);
}

XMVWMV2Codec::~XMVWMV2Codec() {
}

void XMVWMV2Codec::init() {
	// Dimensions
	_mbCountWidth  = (_width  + kMacroBlockSize - 1) / kMacroBlockSize;
	_mbCountHeight = (_height + kMacroBlockSize - 1) / kMacroBlockSize;

	_lumaWidth  = _mbCountWidth  * kMacroBlockSize;
	_lumaHeight = _mbCountHeight * kMacroBlockSize;

	_chromaWidth  = _lumaWidth  / 2;
	_chromaHeight = _lumaHeight / 2;


	// Color planes
	_curPlanes[0].reset(new byte[_lumaWidth   * _lumaHeight  ]);
	_curPlanes[1].reset(new byte[_chromaWidth * _chromaHeight]);
	_curPlanes[2].reset(new byte[_chromaWidth * _chromaHeight]);
	_oldPlanes[0].reset(new byte[_lumaWidth   * _lumaHeight  ]);
	_oldPlanes[1].reset(new byte[_chromaWidth * _chromaHeight]);
	_oldPlanes[2].reset(new byte[_chromaWidth * _chromaHeight]);

	std::memset(_curPlanes[0].get(), 0, _lumaWidth   * _lumaHeight  );
	std::memset(_curPlanes[1].get(), 0, _chromaWidth * _chromaHeight);
	std::memset(_curPlanes[2].get(), 0, _chromaWidth * _chromaHeight);
	std::memset(_oldPlanes[0].get(), 0, _lumaWidth   * _lumaHeight  );
	std::memset(_oldPlanes[1].get(), 0, _chromaWidth * _chromaHeight);
	std::memset(_oldPlanes[2].get(), 0, _chromaWidth * _chromaHeight);


	// Coded block pattern
	_cbp.reset(new CBP[_mbCountWidth + 1]); // +1 border for the start of the row

	_huffCBP[0].reset(new Common::Huffman(wmv2HuffmanIMB));
	_huffCBP[1].reset(new Common::Huffman(wmv2HuffmanPMB[0]));
	_huffCBP[2].reset(new Common::Huffman(wmv2HuffmanPMB[1]));
	_huffCBP[3].reset(new Common::Huffman(wmv2HuffmanPMB[2]));


	// DC Huffman decoders
	_huffDC[0][0].reset(new Common::Huffman(wmv2HuffmanDC[0][0]));
	_huffDC[0][1].reset(new Common::Huffman(wmv2HuffmanDC[0][1]));
	_huffDC[1][0].reset(new Common::Huffman(wmv2HuffmanDC[1][0]));
	_huffDC[1][1].reset(new Common::Huffman(wmv2HuffmanDC[1][1]));


	// AC predictors
	_predAC[0].reset(new int32[_lumaWidth]);
	_predAC[1].reset(new int32[_chromaWidth]);
	_predAC[2].reset(new int32[_chromaWidth]);


	// AC decoders
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			const WMV2ACCoefficientTable *params = &wmv2AC[i][j];

			_decoderAC[i][j].parameters = params;
			_decoderAC[i][j].huffman.reset(new Common::Huffman(params->huffman));
		}
	}

	// Motion vectors
	for (int i = 0; i < 2; i++) {
		const WMV2MVTable *params = &wmv2MV[i];

		_decoderMV[i].parameters = params;
		_decoderMV[i].huffman.reset(new Common::Huffman(params->huffman));
	}
}

void XMVWMV2Codec::decodeFrame(Graphics::Surface &surface,
                               Common::SeekableReadStream &dataStream) {

	Common::BitStream32LEMSB bits(dataStream);
	DecodeContext            ctx(bits);

	initDecodeContext(ctx);

	// Is this a P-frame?
	bool isPFrame = ctx.bits.getBit() != 0;

	// "buffer-fullness", apparently not needed
	if (!isPFrame)
		ctx.bits.skip(7);

	// Quantizer scale
	int32 qScale = ctx.bits.getBits(5);

	ctx.setQScale(qScale);

	// Is this a J-Frame?
	bool isJFrame = false;
	if (!isPFrame && _hasJFrames)
		isJFrame = ctx.bits.getBit() != 0;

	// Decode the frame
	if      ( isJFrame)
		decodeJFrame(ctx);
	else if (!isPFrame)
		decodeIFrame(ctx);
	else
		decodePFrame(ctx);

	// Convert the YUV data we have to BGRA
	YUVToRGBMan.convert420(Graphics::YUVToRGBManager::kScaleITU,
			surface.getData(), surface.getWidth() * 4,
			_curPlanes[0].get(), _curPlanes[1].get(), _curPlanes[2].get(),
			_lumaWidth, _lumaHeight, _lumaWidth, _chromaWidth);

	// And swap the planes with the reference planes
	for (int i = 0; i < 3; i++)
		_oldPlanes[i].swap(_curPlanes[i]);

	_currentFrame++;
}

void XMVWMV2Codec::parseExtraData(Common::SeekableReadStream &extraData) {
	if (extraData.size() < 4)
		throw Common::Exception("XMVWMV2Codec::parseExtraData(): Extra data too small");

	uint32 data = extraData.readUint32LE();

	// Read the various decoder flags

	_hasMixedPelMC      = ( data       & 1) != 0;
	_hasLoopFilter      = ((data >> 1) & 1) != 0;
	_hasVarSizeTrans    = ((data >> 2) & 1) != 0;
	_hasJFrames         = ((data >> 3) & 1) != 0;
	_hasHybridMV        = ((data >> 4) & 1) != 0;
	_hasACPerMacroBlock = ((data >> 5) & 1) != 0;

	// Not used in I-Frames
	_sliceCount = (data >> 6) & 7;
	if (_sliceCount == 0)
		throw Common::Exception("XMVWMV2Codec::parseExtraData(): _sliceCount == 0");

	// Yet unsupported features
	if (_hasLoopFilter)
		warning("TODO: XMVWMV2 Loop filter");
}

void XMVWMV2Codec::initDecodeContext(DecodeContext &ctx) {
	ctx.acQuantTop[0] = _predAC[0].get();
	ctx.acQuantTop[1] = _predAC[1].get();
	ctx.acQuantTop[2] = _predAC[2].get();

	for (int i = 0; i < 4; i++) {
		const uint32 offset = kBlockSize * (i & 1) + kBlockSize * _lumaWidth * (i >> 1);

		ctx.block[i].refPlane = _oldPlanes[0].get() + offset;
		ctx.block[i].curPlane = _curPlanes[0].get() + offset;

		ctx.block[i].planePitch = _lumaWidth;
		ctx.block[i].blockPitch = kMacroBlockSize;
	}

	for (int i = 4; i < 6; i++) {
		ctx.block[i].refPlane = _oldPlanes[i - 3].get();
		ctx.block[i].curPlane = _curPlanes[i - 3].get();

		ctx.block[i].planePitch = _chromaWidth;
		ctx.block[i].blockPitch = kBlockSize;
	}

	std::memset(_cbp.get(), 0, sizeof(CBP) * (_mbCountWidth + 1));

	ctx.rowCBP = _cbp.get() + 1;
}

void XMVWMV2Codec::decodeIFrame(DecodeContext &ctx) {
	// Coefficients decoders

	// Do we have the AC Huffman table indices per macro block?
	if (_hasACPerMacroBlock)
		ctx.hasACPerMacroBlock = ctx.bits.getBit() != 0;

	// If not, read them now
	if (!ctx.hasACPerMacroBlock) {
		ctx.decoderAC[1] = &_decoderAC[1][getTrit(ctx.bits)];
		ctx.decoderAC[0] = &_decoderAC[0][getTrit(ctx.bits)];
	}

	// DC Huffman table index
	uint8 dcTableIndex = ctx.bits.getBit();

	ctx.huffDC[0] = _huffDC[0][dcTableIndex].get();
	ctx.huffDC[1] = _huffDC[1][dcTableIndex].get();


	// Decode the macro blocks, row-major order
	for (uint32 y = 0; y < _mbCountHeight; y++) {

		ctx.startRow();

		// Decode all macro blocks on the current row
		for (uint32 x = 0; x < _mbCountWidth; x++) {

			// CBP

			uint32 cbp = _huffCBP[0]->getSymbol(ctx.bits);

			ctx.startMacroBlock(cbp);

			// AC Coefficients

			ctx.hasACPrediction = ctx.bits.getBit() != 0;

			// Read the AC Huffman table indices for this macro block
			if (ctx.hasACPerMacroBlock && !ctx.curCBP[0].empty()) {
				uint32 index = getTrit(ctx.bits);

				ctx.decoderAC[0] = &_decoderAC[0][index];
				ctx.decoderAC[1] = &_decoderAC[1][index];
			}

			// Decode this macro block
			decodeIMacroBlock(ctx);

			ctx.finishMacroBlock();
		}

		ctx.finishRow();
	}


	// Loop filter
	if (_hasLoopFilter) {
		// TODO: Loop filter
	}
}

void XMVWMV2Codec::decodeJFrame(DecodeContext &UNUSED(ctx)) {
	warning("XMV: J-Frame %d", _currentFrame);

	// Just copy the reference planes for now
	std::memcpy(_curPlanes[0].get(), _oldPlanes[0].get(), _lumaWidth   * _lumaHeight  );
	std::memcpy(_curPlanes[1].get(), _oldPlanes[1].get(), _chromaWidth * _chromaHeight);
	std::memcpy(_curPlanes[2].get(), _oldPlanes[2].get(), _chromaWidth * _chromaHeight);
}

void XMVWMV2Codec::decodePFrame(DecodeContext &UNUSED(ctx)) {
	// Just copy the reference planes for now
	std::memcpy(_curPlanes[0].get(), _oldPlanes[0].get(), _lumaWidth   * _lumaHeight  );
	std::memcpy(_curPlanes[1].get(), _oldPlanes[1].get(), _chromaWidth * _chromaHeight);
	std::memcpy(_curPlanes[2].get(), _oldPlanes[2].get(), _chromaWidth * _chromaHeight);
}

void XMVWMV2Codec::decodeIMacroBlock(DecodeContext &ctx) {
	int32 dcTopLeftNext = ctx.block[1].acQuantTop[0];

	*ctx.block[3].dcTopLeft = ctx.block[0].acQuantLeft[0];

	for (int i = 0; i < 6; i++) {
		BlockContext &block = ctx.block[i];

		block.hasACCoeffs = ctx.curCBP[0].isSet(i);

		int32 dcTop = block.acQuantTop[0];

		decodeIBlock(ctx, block);

		*block.dcTopLeft = dcTop;
	}

	*ctx.block[0].dcTopLeft = dcTopLeftNext;
}

void XMVWMV2Codec::decodeIBlock(DecodeContext &ctx, BlockContext &block) {
	// Check which predictor we're using
	bool  isPredictedLeft = abs(*block.dcTopLeft - block.acQuantTop [0]) <=
	                        abs(*block.dcTopLeft - block.acQuantLeft[0]);
	int32 dcPredictor     = isPredictedLeft ? block.acQuantLeft[0] : block.acQuantTop[0];

	// Decode the DC differential
	int32 dcDiff = (*block.huffDC)->getSymbol(ctx.bits);
	assert(dcDiff >= 0);

	if (((uint32) dcDiff) == block.dcEscapeCode) {
		// Escaped value, directly encoded

		dcDiff = ctx.bits.getBits(8);
		if (dcDiff == 0)
			throw Common::Exception("XMVWMV2Codec::decodeIBlock(): Broken DC Magnitude");
	}

	// Read the DC diff sign
	if (dcDiff && ctx.bits.getBit())
		dcDiff = -dcDiff;

	// Init AC coefficient prediction
	const uint8 *zigZag = 0;
	if (ctx.hasACPrediction) {

		if (isPredictedLeft) {
			std::memset(block.acQuantTop, 0, sizeof(int32) * kBlockSize);

			zigZag = wmv2ZigZagVertical;
		} else {
			std::memset(block.acQuantLeft, 0, sizeof(int32) * kBlockSize);

			zigZag = wmv2ZigZagHorizontal;
		}

	} else {
		std::memset(block.acQuantTop , 0, sizeof(int32) * kBlockSize);
		std::memset(block.acQuantLeft, 0, sizeof(int32) * kBlockSize);

		zigZag = wmv2ZigZagNormal;
	}

	int32 dcQuantCoeff = dcPredictor + dcDiff;

	block.acQuantTop [0] = dcQuantCoeff;
	block.acQuantLeft[0] = dcQuantCoeff;

	int32 acReconCoeffs[kBlockSize * kBlockSize];
	std::memset(acReconCoeffs, 0, sizeof(acReconCoeffs));

	acReconCoeffs[0] = dcQuantCoeff * ctx.dcStepSize;

	int32 qScaleOdd = (ctx.qScale & 1) ? ctx.qScale : (ctx.qScale - 1);
	int32 qScale2   =  ctx.qScale * 2;


	// Decode AC coefficients (using run level encoding)
	if (block.hasACCoeffs) {
		assert(block.decoderAC && *block.decoderAC);
		assert((*block.decoderAC)->huffman && (*block.decoderAC)->parameters);

		      Common::Huffman        &acHuff  = *(*block.decoderAC)->huffman;
		const WMV2ACCoefficientTable &acTable = *(*block.decoderAC)->parameters;

		uint32 coeffCount = 1;

		bool done = false;
		while (!done) {
			uint32 run   = 0;
			 int32 level = 0;

			// Read the run and level table index
			uint32 index = acHuff.getSymbol(ctx.bits);
			if (index > acTable.escapeCode)
				throw Common::Exception("XMVWMV2Codec::decodeIBlock(): Broken AC index");

			// Read the run and level values, and determine if this is the last run
			if (index != acTable.escapeCode) {
				// Run and level from table

				run   = acTable.runTable  [index];
				level = acTable.levelTable[index];

				done = index >= acTable.lastRunIndex;

				if (ctx.bits.getBit())
					level = -level;

			} else if (ctx.bits.getBit()) {
				// Escape + 1: run and level from table + level delta

				index = acHuff.getSymbol(ctx.bits);

				run   = acTable.runTable  [index];
				level = acTable.levelTable[index];

				done = index >= acTable.lastRunIndex;

				if (done)
					level += acTable.levelDeltaTableLast[run];
				else
					level += acTable.levelDeltaTable    [run];

				if (ctx.bits.getBit())
					level = -level;

			} else if (ctx.bits.getBit()) {
				// Escape + 01: run and level from table + run delta

				index = acHuff.getSymbol(ctx.bits);

				run   = acTable.runTable  [index];
				level = acTable.levelTable[index];

				done = index >= acTable.lastRunIndex;

				if (done)
					run += acTable.runDeltaTableLast[level] + 1;
				else
					run += acTable.runDeltaTable    [level] + 1;

				if (ctx.bits.getBit())
					level = -level;

			} else {
				// Escape + 00: run and level directly encoded

				done = ctx.bits.getBit() != 0;

				if (ctx.acRLERunLength == 0) {

					if (ctx.qScale >= 8) {
						int  length = 0;
						bool end    = false;

						while (length < 6 && !end) {
							end = ctx.bits.getBit() != 0;
							length++;
						}

						ctx.acRLELevelLength = end ? (length + 1) : 8;

					} else {

						ctx.acRLELevelLength = ctx.bits.getBits(3);
						if (!ctx.acRLELevelLength)
							ctx.acRLELevelLength = 8 + ctx.bits.getBit();
					}

					ctx.acRLERunLength = 3 + ctx.bits.getBits(2);
				}

				bool levelSign;

				run       = ctx.bits.getBits(ctx.acRLERunLength);
				levelSign = ctx.bits.getBit() != 0;
				level     = ctx.bits.getBits(ctx.acRLELevelLength);

				if (levelSign)
					level = -level;
			}

			// Skip the run of 0s
			coeffCount += run;
			if (coeffCount >= (kBlockSize * kBlockSize))
				throw Common::Exception("XMVWMV2Codec::parseExtraData(): "
				                        "Overrun while deRLEing AC coefficients");

			// deZigZag the current coefficient position
			uint32 coeffIndex = zigZag[coeffCount];

			if        ((coeffIndex / kBlockSize) == 0) {
				// This is the first row

				block.acQuantTop[coeffIndex] += level;

			} else if ((coeffIndex % kBlockSize) == 0) {
				// This is the first column

				block.acQuantLeft[coeffIndex / kBlockSize] += level;

			} else {
				// Not first row/column

				if (level > 0)
					acReconCoeffs[coeffIndex] = qScale2 * level + qScaleOdd;
				else
					acReconCoeffs[coeffIndex] = qScale2 * level - qScaleOdd;
			}

			coeffCount++;
		}
	}

	// deQuantize and propagate the first row and column
	for (uint32 i = 1; i < kBlockSize; i++) {
		int32 acQuantCoeff = block.acQuantLeft[i];

		if      (acQuantCoeff == 0)
			acReconCoeffs[i * kBlockSize] = 0;
		else if (acQuantCoeff  > 0)
			acReconCoeffs[i * kBlockSize] = qScale2 * acQuantCoeff + qScaleOdd;
		else
			acReconCoeffs[i * kBlockSize] = qScale2 * acQuantCoeff - qScaleOdd;

		acQuantCoeff = block.acQuantTop[i];

		if      (acQuantCoeff == 0)
			acReconCoeffs[i] = 0;
		else if (acQuantCoeff  > 0)
			acReconCoeffs[i] = qScale2 * acQuantCoeff + qScaleOdd;
		else
			acReconCoeffs[i] = qScale2 * acQuantCoeff - qScaleOdd;
	}

	// And run the IDCT over the block
	IDCTPut(block.curPlane, acReconCoeffs, block.planePitch);
}

void XMVWMV2Codec::IDCTPut(byte *dest, int32 *block, uint32 pitch) {
	IDCT(block);

	for (uint32 i = 0; i < 8; i++, dest += pitch, block += 8)
		for (uint32 j = 0; j < 8; j++)
			dest[j] = CLIP(block[j], 0, 255);
}

void XMVWMV2Codec::IDCT(int32 *block) {
	for (int i = 0; i < 64; i += 8)
		IDCTRow(block + i);

	for (int i = 0; i < 8; i++)
		IDCTCol(block + i);
}

#define W0 2048
#define W1 2841 /* 2048*sqrt (2)*cos (1*pi/16) */
#define W2 2676 /* 2048*sqrt (2)*cos (2*pi/16) */
#define W3 2408 /* 2048*sqrt (2)*cos (3*pi/16) */
#define W4 2048 /* 2048*sqrt (2)*cos (4*pi/16) */
#define W5 1609 /* 2048*sqrt (2)*cos (5*pi/16) */
#define W6 1108 /* 2048*sqrt (2)*cos (6*pi/16) */
#define W7  565 /* 2048*sqrt (2)*cos (7*pi/16) */

void XMVWMV2Codec::IDCTRow(int32 *b) {
	// Step 1
	int a1 = (W1 * b[1]) + (W7 * b[7]);
	int a7 = (W7 * b[1]) - (W1 * b[7]);
	int a5 = (W5 * b[5]) + (W3 * b[3]);
	int a3 = (W3 * b[5]) - (W5 * b[3]);
	int a2 = (W2 * b[2]) + (W6 * b[6]);
	int a6 = (W6 * b[2]) - (W2 * b[6]);
	int a0 = (W0 * b[0]) + (W0 * b[4]);
	int a4 = (W0 * b[0]) - (W0 * b[4]);

	// Step 2
	int s1 = (181 * (a1 - a5 + a7 - a3) + 128) >> 8; // 1, 3, 5, 7,
	int s2 = (181 * (a1 - a5 - a7 + a3) + 128) >> 8;

	// Step 3
	b[0] = (a0 + a2 + a1 + a5 + (1 << 7)) >> 8;
	b[1] = (a4 + a6    + s1   + (1 << 7)) >> 8;
	b[2] = (a4 - a6    + s2   + (1 << 7)) >> 8;
	b[3] = (a0 - a2 + a7 + a3 + (1 << 7)) >> 8;
	b[4] = (a0 - a2 - a7 - a3 + (1 << 7)) >> 8;
	b[5] = (a4 - a6    - s2   + (1 << 7)) >> 8;
	b[6] = (a4 + a6    - s1   + (1 << 7)) >> 8;
	b[7] = (a0 + a2 - a1 - a5 + (1 << 7)) >> 8;
}

void XMVWMV2Codec::IDCTCol(int32 *b) {
	// Step 1, with extended precision
	int a1 = ((W1 * b[8 * 1]) + (W7 * b[8 * 7]) + 4) >> 3;
	int a7 = ((W7 * b[8 * 1]) - (W1 * b[8 * 7]) + 4) >> 3;
	int a5 = ((W5 * b[8 * 5]) + (W3 * b[8 * 3]) + 4) >> 3;
	int a3 = ((W3 * b[8 * 5]) - (W5 * b[8 * 3]) + 4) >> 3;
	int a2 = ((W2 * b[8 * 2]) + (W6 * b[8 * 6]) + 4) >> 3;
	int a6 = ((W6 * b[8 * 2]) - (W2 * b[8 * 6]) + 4) >> 3;
	int a0 = ((W0 * b[8 * 0]) + (W0 * b[8 * 4])    ) >> 3;
	int a4 = ((W0 * b[8 * 0]) - (W0 * b[8 * 4])    ) >> 3;

	// Step 2
	int s1 = (181 * (a1 - a5 + a7 - a3) + 128) >> 8;
	int s2 = (181 * (a1 - a5 - a7 + a3) + 128) >> 8;

	// Step 3
	b[8 * 0] = (a0 + a2 + a1 + a5 + (1 << 13)) >> 14;
	b[8 * 1] = (a4 + a6    + s1   + (1 << 13)) >> 14;
	b[8 * 2] = (a4 - a6    + s2   + (1 << 13)) >> 14;
	b[8 * 3] = (a0 - a2 + a7 + a3 + (1 << 13)) >> 14;

	b[8 * 4] = (a0 - a2 - a7 - a3 + (1 << 13)) >> 14;
	b[8 * 5] = (a4 - a6    - s2   + (1 << 13)) >> 14;
	b[8 * 6] = (a4 + a6    - s1   + (1 << 13)) >> 14;
	b[8 * 7] = (a0 + a2 - a1 - a5 + (1 << 13)) >> 14;
}

uint8 XMVWMV2Codec::getTrit(Common::BitStream &bits) {
	// 0 -> 0;  10 -> 1;  11 -> 2

	uint8 n = bits.getBit();
	if (n == 0)
		return n;

	return bits.getBit() + 1;
}

} // End of namespace Video
