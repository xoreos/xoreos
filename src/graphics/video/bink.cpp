/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/bink.cpp
 *  Decoding RAD Game Tools' Bink videos.
 */

#include <cmath>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/bitstream.h"
#include "common/huffman.h"

#include "graphics/util.h"

#include "graphics/video/bink.h"
#include "graphics/video/binkdata.h"

#include "events/events.h"

static const uint32 kBIKfID = MKID_BE('BIKf');
static const uint32 kBIKgID = MKID_BE('BIKg');
static const uint32 kBIKhID = MKID_BE('BIKh');
static const uint32 kBIKiID = MKID_BE('BIKi');

static const uint32 kVideoFlagAlpha = 0x00100000;

// Number of bits used to store first DC value in bundle
static const uint32 kDCStartBits = 11;

namespace Graphics {

Bink::Bink(Common::SeekableReadStream *bink) : _bink(bink), _curFrame(0) {
	assert(_bink);

	for (int i = 0; i < 16; i++)
		_huffman[i] = 0;

	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].countLength = 0;

		_bundles[i].huffman.index = 0;
		for (int j = 0; j < 16; j++)
			_bundles[i].huffman.symbols[j] = j;

		_bundles[i].data     = 0;
		_bundles[i].dataEnd  = 0;
		_bundles[i].curDec   = 0;
		_bundles[i].curPtr   = 0;
	}

	for (int i = 0; i < 16; i++) {
		_colHighHuffman[i].index = 0;
		for (int j = 0; j < 16; j++)
			_colHighHuffman[i].symbols[j] = j;
	}

	for (int i = 0; i < 4; i++) {
		_curPlanes[i] = 0;
		_oldPlanes[i] = 0;
	}

	load();
	addToQueue();
}

Bink::~Bink() {
	for (int i = 0; i < 4; i++) {
		delete[] _curPlanes[i];
		delete[] _oldPlanes[i];
	}

	deinitBundles();

	for (int i = 0; i < 16; i++)
		delete _huffman[i];

	delete _bink;
}

bool Bink::gotTime() const {
	uint32 curTime = EventMan.getTimestamp();
	uint32 frameTime = ((uint64) (_curFrame * 1000 * ((uint64) _fpsDen))) / _fpsNum;
	if ((curTime - _startTime + 11) < frameTime)
		return true;

	return false;
}

void Bink::processData() {
	uint32 curTime = EventMan.getTimestamp();

	if (!_started) {
		_startTime     = curTime;
		_lastFrameTime = curTime;
		_started       = true;
	}

	uint32 frameTime = ((uint64) (_curFrame * 1000 * ((uint64) _fpsDen))) / _fpsNum;
	if ((curTime - _startTime) < frameTime)
		return;

	if (_curFrame >= _frames.size()) {
		_finished = true;
		return;
	}

	VideoFrame &frame = _frames[_curFrame];

	if (!_bink->seek(frame.offset))
		throw Common::Exception(Common::kSeekError);

	uint32 frameSize = frame.size;

	for (std::vector<AudioTrack>::iterator audio = _audioTracks.begin(); audio != _audioTracks.end(); ++audio) {
		uint32 audioPacketLength = _bink->readUint32LE();

		frameSize -= 4;

		if (frameSize < audioPacketLength)
			throw Common::Exception("Audio packet too big for the frame");

		if (audioPacketLength >= 4) {
			audio->sampleCount = _bink->readUint32LE();

			audio->bits = new Common::BitStream32LE(*_bink, (audioPacketLength - 4) * 8);

			audioPacket(*audio);

			delete audio->bits;
			audio->bits = 0;

			frameSize -= audioPacketLength;
		}
	}

	frame.bits = new Common::BitStream32LE(*_bink, frameSize * 8);

	videoPacket(frame);

	delete frame.bits;
	frame.bits = 0;

	_needCopy = true;

	warning("Frame %d / %d", _curFrame, (int) _frames.size());

	_curFrame++;
}

void Bink::yuva2bgra() {
	assert(_data && _curPlanes[0] && _curPlanes[1] && _curPlanes[2] && _curPlanes[3]);

	const byte *planeY = _curPlanes[0];
	const byte *planeU = _curPlanes[1];
	const byte *planeV = _curPlanes[2];
	const byte *planeA = _curPlanes[3];
	byte *data = _data + (_height - 1) * _pitch * 4;
	for (uint32 y = 0; y < _height; y++) {
		byte *rowData = data;

		for (uint32 x = 0; x < _width; x++, rowData += 4) {
			const byte y = planeY[x];
			const byte u = planeU[x >> 1];
			const byte v = planeV[x >> 1];

			byte r = 0, g = 0, b = 0;
			YUV2RGB(y, u, v, r, g, b);

			rowData[0] = b;
			rowData[1] = g;
			rowData[2] = r;
			rowData[3] = planeA[x];
		}

		data   -= _pitch * 4;
		planeY += _width;
		planeA += _width;

		if ((y & 1) == 1) {
			planeU += _width >> 1;
			planeV += _width >> 1;
		}
	}
}

void Bink::audioPacket(AudioTrack &audio) {
}

void Bink::videoPacket(VideoFrame &video) {
	assert(video.bits);

	if (_hasAlpha) {
		if (_id == kBIKiID)
			video.bits->skip(32);

		decodePlane(video, 3, false);
	}

	if (_id == kBIKiID)
		video.bits->skip(32);

	for (int i = 0; i < 3; i++) {
		int planeIdx = ((i == 0) || !_swapPlanes) ? i : (i ^ 3);

		decodePlane(video, planeIdx, i != 0);

		if (video.bits->pos() >= video.bits->size())
			break;
	}

	// Convert the YUVA data we have to BGRA
	yuva2bgra();

	// And swap the planes with the reference planes
	for (int i = 0; i < 4; i++)
		SWAP(_curPlanes[i], _oldPlanes[i]);
}

void Bink::decodePlane(VideoFrame &video, int planeIdx, bool isChroma) {

	uint32 blockWidth  = isChroma ? ((_width  + 15) >> 4) : ((_width  + 7) >> 3);
	uint32 blockHeight = isChroma ? ((_height + 15) >> 4) : ((_height + 7) >> 3);
	uint32 width       = isChroma ?  (_width        >> 1) :   _width;
	uint32 height      = isChroma ?  (_height       >> 1) :   _height;

	DecodeContext ctx;

	ctx.video     = &video;
	ctx.planeIdx  = planeIdx;
	ctx.destStart = _curPlanes[planeIdx];
	ctx.destEnd   = _curPlanes[planeIdx] + width * height;
	ctx.prevStart = _oldPlanes[planeIdx];
	ctx.prevEnd   = _oldPlanes[planeIdx] + width * height;
	ctx.pitch     = width;

	for (int i = 0; i < 64; i++) {
		ctx.coordMap[i] = (i & 7) + (i >> 3) * ctx.pitch;

		ctx.coordScaledMap1[i] = ((i & 7) * 2 + 0) + (((i >> 3) * 2 + 0) * ctx.pitch);
		ctx.coordScaledMap2[i] = ((i & 7) * 2 + 1) + (((i >> 3) * 2 + 0) * ctx.pitch);
		ctx.coordScaledMap3[i] = ((i & 7) * 2 + 0) + (((i >> 3) * 2 + 1) * ctx.pitch);
		ctx.coordScaledMap4[i] = ((i & 7) * 2 + 1) + (((i >> 3) * 2 + 1) * ctx.pitch);
	}

	// const uint8_t *scan;
	// DECLARE_ALIGNED(16, DCTELEM, block[64]);
	// DECLARE_ALIGNED(16, uint8_t, ublock[64]);

	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].countLength = _bundles[i].countLengths[isChroma ? 1 : 0];

		readBundle(video, (Source) i);
	}

	for (ctx.blockY = 0; ctx.blockY < blockHeight; ctx.blockY++) {
		readBlockTypes  (video, _bundles[kSourceBlockTypes]);
		readBlockTypes  (video, _bundles[kSourceSubBlockTypes]);
		readColors      (video, _bundles[kSourceColors]);
		readPatterns    (video, _bundles[kSourcePattern]);
		readMotionValues(video, _bundles[kSourceXOff]);
		readMotionValues(video, _bundles[kSourceYOff]);
		readDCS         (video, _bundles[kSourceIntraDC], kDCStartBits, false);
		readDCS         (video, _bundles[kSourceInterDC], kDCStartBits, true);
		readRuns        (video, _bundles[kSourceRun]);

		ctx.dest = ctx.destStart + 8 * ctx.blockY * ctx.pitch;
		ctx.prev = ctx.prevStart + 8 * ctx.blockY * ctx.pitch;

		for (ctx.blockX = 0; ctx.blockX < blockWidth; ctx.blockX++, ctx.dest += 8, ctx.prev += 8) {
			BlockType blockType = (BlockType) getBundleValue(kSourceBlockTypes);

			// warning("%d.%d.%d: %d (%d)", planeIdx, by, bx, blockType, video.bits->pos());

			// 16x16 block type on odd line means part of the already decoded block, so skip it
			if ((ctx.blockY & 1) && (blockType == kBlockScaled)) {
				ctx.blockX += 1;
				ctx.dest   += 8;
				ctx.prev   += 8;
				continue;
			}

			switch (blockType) {
				case kBlockSkip:
					blockSkip(ctx);
					break;

				case kBlockScaled:
					blockScaled(ctx);
					break;

				case kBlockMotion:
					blockMotion(ctx);
					break;

				case kBlockRun:
					blockRun(ctx);
					break;

				case kBlockResidue:
					blockResidue(ctx);
					break;

				case kBlockIntra:
					blockIntra(ctx);
					break;

				case kBlockFill:
					blockFill(ctx);
					break;

				case kBlockInter:
					blockInter(ctx);
					break;

				case kBlockPattern:
					blockPattern(ctx);
					break;

				case kBlockRaw:
					blockRaw(ctx);
					break;

				default:
					throw Common::Exception("Unknown block type: %d", blockType);
			}

		}

	}

	if (video.bits->pos() & 0x1F) // next plane data starts at 32-bit boundary
		video.bits->skip(32 - (video.bits->pos() & 0x1F));

}

void Bink::readBundle(VideoFrame &video, Source source) {
	if (source == kSourceColors) {
		for (int i = 0; i < 16; i++)
			readHuffman(video, _colHighHuffman[i]);

		_colLastVal = 0;
	}

	if ((source != kSourceIntraDC) && (source != kSourceInterDC))
		readHuffman(video, _bundles[source].huffman);

	_bundles[source].curDec = _bundles[source].data;
	_bundles[source].curPtr = _bundles[source].data;
}

void Bink::readHuffman(VideoFrame &video, Huffman &huffman) {
	huffman.index = video.bits->getBits(4);

	if (huffman.index == 0) {
		// The first tree always gives raw nibbles

		for (int i = 0; i < 16; i++)
			huffman.symbols[i] = i;

		return;
	}

	byte hasSymbol[16];

	if (video.bits->getBit()) {
		// Symbol selection

		memset(hasSymbol, 0, 16);

		uint8 length = video.bits->getBits(3);
		for (int i = 0; i <= length; i++) {
			huffman.symbols[i] = video.bits->getBits(4);
			hasSymbol[huffman.symbols[i]] = 1;
		}

		for (int i = 0; i < 16; i++)
			if (hasSymbol[i] == 0)
				huffman.symbols[++length] = i;

		return;
	}

	// Symbol shuffling

	byte tmp1[16], tmp2[16];
	byte *in = tmp1, *out = tmp2;

	uint8 depth = video.bits->getBits(2);

	for (int i = 0; i < 16; i++)
		in[i] = i;

	for (int i = 0; i <= depth; i++) {
		int size = 1 << i;

		for (int j = 0; j < 16; j += (size << 1))
			mergeHuffmanSymbols(video, out + j, in + j, size);

		SWAP(in, out);
	}

	memcpy(huffman.symbols, in, 16);
}

void Bink::mergeHuffmanSymbols(VideoFrame &video, byte *dst, const byte *src, int size) {
	const byte *src2  = src + size;
	int         size2 = size;

	do {
		if (!video.bits->getBit()) {
			*dst++ = *src++;
			size--;
		} else {
			*dst++ = *src2++;
			size2--;
		}

	} while (size && size2);

	while (size--)
		*dst++ = *src++;
	while (size2--)
		*dst++ = *src2++;
}

void Bink::load() {
	_id = _bink->readUint32BE();
	if ((_id != kBIKfID) && (_id != kBIKgID) && (_id != kBIKhID) && (_id != kBIKiID))
		throw Common::Exception("Unknown Bink FourCC %04X", _id);

	uint32 fileSize         = _bink->readUint32LE() + 8;
	uint32 frameCount       = _bink->readUint32LE();
	uint32 largestFrameSize = _bink->readUint32LE();

	if (largestFrameSize > fileSize)
		throw Common::Exception("Largest frame size greater than file size");

	_bink->skip(4);

	uint32 width  = _bink->readUint32LE();
	uint32 height = _bink->readUint32LE();

	createData(width, height);

	_fpsNum = _bink->readUint32LE();
	_fpsDen = _bink->readUint32LE();

	if ((_fpsNum == 0) || (_fpsDen == 0))
		throw Common::Exception("Invalid FPS (%d/%d)", _fpsNum, _fpsDen);

	_videoFlags = _bink->readUint32LE();

	uint32 audioTrackCount = _bink->readUint32LE();
	if (audioTrackCount > 0) {
		_audioTracks.resize(audioTrackCount);

		_bink->skip(4 * audioTrackCount);

		// Reading audio track properties
		for (std::vector<AudioTrack>::iterator it = _audioTracks.begin(); it != _audioTracks.end(); ++it) {
			it->sampleRate  = _bink->readUint16LE();
			it->flags       = _bink->readUint16LE();
			it->sampleCount = 0;
			it->bits        = 0;
		}

		_bink->skip(4 * audioTrackCount);
	}

	// Reading video frame properties
	_frames.resize(frameCount);
	for (uint32 i = 0; i < frameCount; i++) {
		_frames[i].offset   = _bink->readUint32LE();
		_frames[i].keyFrame = _frames[i].offset & 1;

		_frames[i].offset &= ~1;

		if (i != 0)
			_frames[i - 1].size = _frames[i].offset - _frames[i - 1].offset;

		_frames[i].bits = 0;
	}

	_frames[frameCount - 1].size = _bink->size() - _frames[frameCount - 1].offset;

	_hasAlpha   = _videoFlags & kVideoFlagAlpha;
	_swapPlanes = (_id == kBIKhID) || (_id == kBIKiID); // BIKh and BIKi swap the chroma planes

	// Give the planes a bit extra space
	width  = _width  + 32;
	height = _height + 32;

	_curPlanes[0] = new byte[ width       *  height      ]; // Y
	_curPlanes[1] = new byte[(width >> 1) * (height >> 1)]; // U, 1/4 resolution
	_curPlanes[2] = new byte[(width >> 1) * (height >> 1)]; // V, 1/4 resolution
	_curPlanes[3] = new byte[ width       *  height      ]; // A
	_oldPlanes[0] = new byte[ width       *  height      ]; // Y
	_oldPlanes[1] = new byte[(width >> 1) * (height >> 1)]; // U, 1/4 resolution
	_oldPlanes[2] = new byte[(width >> 1) * (height >> 1)]; // V, 1/4 resolution
	_oldPlanes[3] = new byte[ width       *  height      ]; // A

	// Initialize the video with solid black
	memset(_curPlanes[0],   0,  width       *  height      );
	memset(_curPlanes[1],   0, (width >> 1) * (height >> 1));
	memset(_curPlanes[2],   0, (width >> 1) * (height >> 1));
	memset(_curPlanes[3], 255,  width       *  height      );
	memset(_oldPlanes[0],   0,  width       *  height      );
	memset(_oldPlanes[1],   0, (width >> 1) * (height >> 1));
	memset(_oldPlanes[2],   0, (width >> 1) * (height >> 1));
	memset(_oldPlanes[3], 255,  width       *  height      );

	initBundles();
	initHuffman();
}

void Bink::initBundles() {
	uint32 bw     = (_width  + 7) >> 3;
	uint32 bh     = (_height + 7) >> 3;
	uint32 blocks = bw * bh;

	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].data    = new byte[blocks * 64];
		_bundles[i].dataEnd = _bundles[i].data + blocks * 64;
	}

	uint32 cbw[2] = { (_width + 7) >> 3, (_width  + 15) >> 4 };
	uint32 cw [2] = {  _width          ,  _width        >> 1 };

	// Calculate the lengths of an element count in bits
	for (int i = 0; i < 2; i++) {
		int width = MAX<uint32>(cw[i], 8);

		_bundles[kSourceBlockTypes   ].countLengths[i] = log2((width  >> 3)    + 511) + 1;
		_bundles[kSourceSubBlockTypes].countLengths[i] = log2((width  >> 4)    + 511) + 1;
		_bundles[kSourceColors       ].countLengths[i] = log2((width  >> 3)*64 + 511) + 1;
		_bundles[kSourceIntraDC      ].countLengths[i] = log2((width  >> 3)    + 511) + 1;
		_bundles[kSourceInterDC      ].countLengths[i] = log2((width  >> 3)    + 511) + 1;
		_bundles[kSourceXOff         ].countLengths[i] = log2((width  >> 3)    + 511) + 1;
		_bundles[kSourceYOff         ].countLengths[i] = log2((width  >> 3)    + 511) + 1;
		_bundles[kSourcePattern      ].countLengths[i] = log2((cbw[i] << 3)    + 511) + 1;
		_bundles[kSourceRun          ].countLengths[i] = log2((width  >> 3)*48 + 511) + 1;
	}
}

void Bink::deinitBundles() {
	for (int i = 0; i < kSourceMAX; i++)
		delete[] _bundles[i].data;
}

void Bink::initHuffman() {
	for (int i = 0; i < 16; i++)
		_huffman[i] = new Common::Huffman(binkHuffmanLengths[i][15], 16, binkHuffmanCodes[i], binkHuffmanLengths[i]);
}

byte Bink::getHuffmanSymbol(VideoFrame &video, Huffman &huffman) {
	return huffman.symbols[_huffman[huffman.index]->getSymbol(*video.bits)];
}

int32 Bink::getBundleValue(Source source) {
	if ((source < kSourceXOff) || (source == kSourceRun))
		return *_bundles[source].curPtr++;

	if ((source == kSourceXOff) || (source == kSourceYOff))
		return (int8) *_bundles[source].curPtr++;

	int16 ret = *((int16 *) _bundles[source].curPtr);

	_bundles[source].curPtr += 2;

	return ret;
}

uint32 Bink::readBundleCount(VideoFrame &video, Bundle &bundle) {
	if (!bundle.curDec || (bundle.curDec > bundle.curPtr))
		return 0;

	uint32 n = video.bits->getBits(bundle.countLength);
	if (n == 0)
		bundle.curDec = 0;

	return n;
}

void Bink::blockSkip(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *prev = ctx.prev;

	for (int j = 0; j < 8; j++, dest += ctx.pitch, prev += ctx.pitch)
		memcpy(dest, prev, 8);
}

void Bink::blockScaledRun(DecodeContext &ctx) {
	const uint8 *scan = binkPatterns[ctx.video->bits->getBits(4)];

	int i = 0;
	do {
		int run = getBundleValue(kSourceRun) + 1;

		i += run;
		if (i > 64)
			throw Common::Exception("Run went out of bounds");

		if (ctx.video->bits->getBit()) {

			byte v = getBundleValue(kSourceColors);
			for (int j = 0; j < run; j++, scan++)
				ctx.dest[ctx.coordScaledMap1[*scan]] =
				ctx.dest[ctx.coordScaledMap2[*scan]] =
				ctx.dest[ctx.coordScaledMap3[*scan]] =
				ctx.dest[ctx.coordScaledMap4[*scan]] = v;

		} else
			for (int j = 0; j < run; j++, scan++)
				ctx.dest[ctx.coordScaledMap1[*scan]] =
				ctx.dest[ctx.coordScaledMap2[*scan]] =
				ctx.dest[ctx.coordScaledMap3[*scan]] =
				ctx.dest[ctx.coordScaledMap4[*scan]] = getBundleValue(kSourceColors);

	} while (i < 63);

	if (i == 63)
		ctx.dest[ctx.coordScaledMap1[*scan]] =
		ctx.dest[ctx.coordScaledMap2[*scan]] =
		ctx.dest[ctx.coordScaledMap3[*scan]] =
		ctx.dest[ctx.coordScaledMap4[*scan]] = getBundleValue(kSourceColors);
}

void Bink::blockScaledIntra(DecodeContext &ctx) {
	// c->dsp.clear_block(block);
	getBundleValue(kSourceIntraDC); //block[0] = get_value(c, BINK_SRC_INTRA_DC);
	readDCTCoeffs(*ctx.video, 0, 0, 1); // read_dct_coeffs(gb, block, c->scantable.permutated, 1);
	// c->dsp.idct(block);
	// c->dsp.put_pixels_nonclamped(block, ublock, 8);
}

void Bink::blockScaledFill(DecodeContext &ctx) {
	byte v = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 16; i++, dest += ctx.pitch)
		memset(dest, v, 16);
}

void Bink::blockScaledPattern(DecodeContext &ctx) {
	byte col[2];

	for (int i = 0; i < 2; i++)
		col[i] = getBundleValue(kSourceColors);

	byte *dest1 = ctx.dest;
	byte *dest2 = ctx.dest + ctx.pitch;
	for (int j = 0; j < 8; j++, dest1 += (ctx.pitch << 1) - 16, dest2 += (ctx.pitch << 1) - 16) {
		byte v = getBundleValue(kSourcePattern);

		for (int i = 0; i < 8; i++, v >>= 1)
			dest1[0] = dest1[1] = dest2[0] = dest2[1] = col[v & 1];
	}
}

void Bink::blockScaledRaw(DecodeContext &ctx) {
	byte row[8];

	byte *dest1 = ctx.dest;
	byte *dest2 = ctx.dest + ctx.pitch;
	for (int j = 0; j < 8; j++, dest1 += (ctx.pitch << 1) - 16, dest2 += (ctx.pitch << 1) - 16) {
		memcpy(row, _bundles[kSourceColors].curPtr, 8);

		for (int i = 0; i < 8; i++, dest1 += 2, dest2 += 2)
			dest1[0] = dest1[1] = dest2[0] = dest2[1] = row[i];

		_bundles[kSourceColors].curPtr += 8;
	}
}

void Bink::blockScaled(DecodeContext &ctx) {
	BlockType blockType = (BlockType) getBundleValue(kSourceSubBlockTypes);
	// warning("blockScaled: %d", blockType);

	switch (blockType) {
		case kBlockRun:
			blockScaledRun(ctx);
			break;

		case kBlockIntra:
			blockScaledIntra(ctx);
			break;

		case kBlockFill:
			blockScaledFill(ctx);
			break;

		case kBlockPattern:
			blockScaledPattern(ctx);
			break;

		case kBlockRaw:
			blockScaledRaw(ctx);
			break;

		default:
			throw Common::Exception("Invalid 16x16 block type: %d\n", blockType);
	}

	ctx.blockX += 1;
	ctx.dest   += 8;
	ctx.prev   += 8;
}

void Bink::blockMotion(DecodeContext &ctx) {
	int8 xOff = getBundleValue(kSourceXOff);
	int8 yOff = getBundleValue(kSourceYOff);

	byte *dest = ctx.dest;
	byte *prev = ctx.prev + yOff * ((int32) ctx.pitch) + xOff;
	if ((prev < ctx.prevStart) || (prev > ctx.prevEnd))
		throw Common::Exception("Copy out of bounds (%d | %d)", ctx.blockX * 8 + xOff, ctx.blockY * 8 + yOff);

	for (int j = 0; j < 8; j++, dest += ctx.pitch, prev += ctx.pitch)
		memcpy(dest, prev, 8);
}

void Bink::blockRun(DecodeContext &ctx) {
	const uint8 *scan = binkPatterns[ctx.video->bits->getBits(4)];

	int i = 0;
	do {
		int run = getBundleValue(kSourceRun) + 1;

		i += run;
		if (i > 64)
			throw Common::Exception("Run went out of bounds");

		if (ctx.video->bits->getBit()) {

			byte v = getBundleValue(kSourceColors);
			for (int j = 0; j < run; j++)
				ctx.dest[ctx.coordMap[*scan++]] = v;

		} else
			for (int j = 0; j < run; j++)
				ctx.dest[ctx.coordMap[*scan++]] = getBundleValue(kSourceColors);

	} while (i < 63);

	if (i == 63)
		ctx.dest[ctx.coordMap[*scan++]] = getBundleValue(kSourceColors);
}

void Bink::blockResidue(DecodeContext &ctx) {
	int8 xOff = getBundleValue(kSourceXOff);
	int8 yOff = getBundleValue(kSourceYOff);

	// ref = prev + xoff + yoff * stride;

	// if (ref < ref_start || ref > ref_end)
		// throw Common::Exception("Copy out of bounds (%d | %d)", bx * 8 + xOff, by * 8 + yOff);

	// c->dsp.put_pixels_tab[1][0](dest, ref, stride, 8);
	// c->dsp.clear_block(block);

	byte v = ctx.video->bits->getBits(7);

	readResidue(*ctx.video, 0, v); // read_residue(gb, block, v);
	// c->dsp.add_pixels8(dest, block, stride);
}

void Bink::blockIntra(DecodeContext &ctx) {
	// c->dsp.clear_block(block);

	getBundleValue(kSourceIntraDC); // block[0] = get_value(c, BINK_SRC_INTRA_DC);

	readDCTCoeffs(*ctx.video, 0, 0, 1); // read_dct_coeffs(gb, block, c->scantable.permutated, 1);

	// c->dsp.idct_put(dest, stride, block);
}

void Bink::blockFill(DecodeContext &ctx) {
	byte v = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 8; i++, dest += ctx.pitch)
		memset(dest, v, 8);
}

void Bink::blockInter(DecodeContext &ctx) {
	int8 xOff = getBundleValue(kSourceXOff);
	int8 yOff = getBundleValue(kSourceYOff);

	// ref = prev + xoff + yoff * stride;
	// c->dsp.put_pixels_tab[1][0](dest, ref, stride, 8);
	// c->dsp.clear_block(block);

	getBundleValue(kSourceInterDC); // block[0] = get_value(c, BINK_SRC_INTER_DC);

	readDCTCoeffs(*ctx.video, 0, 0, 0); // read_dct_coeffs(gb, block, c->scantable.permutated, 0);

	// c->dsp.idct_add(dest, stride, block);
}

void Bink::blockPattern(DecodeContext &ctx) {
	byte col[2];

	for (int i = 0; i < 2; i++)
		col[i] = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 8; i++, dest += ctx.pitch - 8) {
		byte v = getBundleValue(kSourcePattern);

		for (int j = 0; j < 8; j++, v >>= 1)
			*dest++ = col[v & 1];
	}
}

void Bink::blockRaw(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *data = _bundles[kSourceColors].curPtr;
	for (int i = 0; i < 8; i++, dest += ctx.pitch, data += 8)
		memcpy(dest, data, 8);

	_bundles[kSourceColors].curPtr += 64;
}

void Bink::readRuns(VideoFrame &video, Bundle &bundle) {
	uint32 n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Run value went out of bounds");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		memset(bundle.curDec, v, n);
		bundle.curDec += n;

	} else
		while (bundle.curDec < decEnd)
			*bundle.curDec++ = getHuffmanSymbol(video, bundle.huffman);
}

void Bink::readMotionValues(VideoFrame &video, Bundle &bundle) {
	uint32 n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many motion values");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		if (v) {
			int sign = -video.bits->getBit();
			v = (v ^ sign) - sign;
		}

		memset(bundle.curDec, v, n);

		bundle.curDec += n;
		return;
	}

	do {
		byte v = getHuffmanSymbol(video, bundle.huffman);

		if (v) {
			int sign = -video.bits->getBit();
			v = (v ^ sign) - sign;
		}

		*bundle.curDec++ = v;

	} while (bundle.curDec < decEnd);
}

const uint8 rleLens[4] = { 4, 8, 12, 32 };
void Bink::readBlockTypes(VideoFrame &video, Bundle &bundle) {
	uint32 n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many block type values");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		memset(bundle.curDec, v, n);

		bundle.curDec += n;
		return;
	}

	byte last = 0;
	do {

		byte v = getHuffmanSymbol(video, bundle.huffman);

		if (v < 12) {
			last = v;
			*bundle.curDec++ = v;
		} else {
			int run = rleLens[v - 12];

			memset(bundle.curDec, last, run);

			bundle.curDec += run;
		}

	} while (bundle.curDec < decEnd);
}

void Bink::readPatterns(VideoFrame &video, Bundle &bundle) {
	uint32 n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many pattern values");

	byte v;
	while (bundle.curDec < decEnd) {
		v  = getHuffmanSymbol(video, bundle.huffman);
		v |= getHuffmanSymbol(video, bundle.huffman) << 4;
		*bundle.curDec++ = v;
	}
}


void Bink::readColors(VideoFrame &video, Bundle &bundle) {
	uint32 n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many color values");

	if (video.bits->getBit()) {
		_colLastVal = getHuffmanSymbol(video, _colHighHuffman[_colLastVal]);

		byte v;
		v = getHuffmanSymbol(video, bundle.huffman);
		v = (_colLastVal << 4) | v;

		if (_id != kBIKiID) {
			int sign = ((int8) v) >> 7;
			v = ((v & 0x7F) ^ sign) - sign;
			v += 0x80;
		}

		memset(bundle.curDec, v, n);
		bundle.curDec += n;

		return;
	}

	while (bundle.curDec < decEnd) {
		_colLastVal = getHuffmanSymbol(video, _colHighHuffman[_colLastVal]);

		byte v;
		v = getHuffmanSymbol(video, bundle.huffman);
		v = (_colLastVal << 4) | v;

		if (_id != kBIKiID) {
			int sign = ((int8) v) >> 7;
			v = ((v & 0x7F) ^ sign) - sign;
			v += 0x80;
		}
		*bundle.curDec++ = v;
	}
}

void Bink::readDCS(VideoFrame &video, Bundle &bundle, int startBits, bool hasSign) {
	uint32 length = readBundleCount(video, bundle);
	if (length == 0)
		return;

	int16 *dest = (int16 *) bundle.curDec;

	int32 v = video.bits->getBits(startBits - (hasSign ? 1 : 0));
	if (v && hasSign) {
		int sign = -video.bits->getBit();
		v = (v ^ sign) - sign;
	}

	*dest++ = v;
	length--;

	for (uint32 i = 0; i < length; i += 8) {
		uint32 length2 = MIN<uint32>(length - i, 8);

		byte bSize = video.bits->getBits(4);

		if (bSize) {

			for (uint32 j = 0; j < length2; j++) {
				int16 v2 = video.bits->getBits(bSize);
				if (v2) {
					int sign = -video.bits->getBit();
					v2 = (v2 ^ sign) - sign;
				}

				v += v2;
				*dest++ = v;

				if ((v < -32768) || (v > 32767))
					throw Common::Exception("DC value went out of bounds: %d", v);
			}

		} else
			for (uint32 j = 0; j < length2; j++)
				*dest++ = v;
	}

	bundle.curDec = (byte *) dest;
}

/** Reads 8x8 block of DCT coefficients. */
void Bink::readDCTCoeffs(VideoFrame &video, void *block, void *scan, int isIntra) {
	int coefCount = 0;
	int coefIdx[64];

	int listStart = 64;
	int listEnd   = 64;

	int coefList[128];      int modeList[128];
	coefList[listEnd] = 4;  modeList[listEnd++] = 0;
	coefList[listEnd] = 24; modeList[listEnd++] = 0;
	coefList[listEnd] = 44; modeList[listEnd++] = 0;
	coefList[listEnd] = 1;  modeList[listEnd++] = 3;
	coefList[listEnd] = 2;  modeList[listEnd++] = 3;
	coefList[listEnd] = 3;  modeList[listEnd++] = 3;

	int bits = video.bits->getBits(4) - 1;
	for (int mask = 1 << bits; bits >= 0; mask >>= 1, bits--) {
		int listPos = listStart;

		while (listPos < listEnd) {

			if (!(modeList[listPos] | coefList[listPos]) || !video.bits->getBit()) {
				listPos++;
				continue;
			}

			int ccoef = coefList[listPos];
			int mode  = modeList[listPos];

			switch (mode) {
			case 0:
				coefList[listPos] = ccoef + 4;
				modeList[listPos] = 1;
			case 2:
				if (mode == 2) {
					coefList[listPos]   = 0;
					modeList[listPos++] = 0;
				}
				for (int i = 0; i < 4; i++, ccoef++) {
					if (video.bits->getBit()) {
						coefList[--listStart] = ccoef;
						modeList[  listStart] = 3;
					} else {
						int t;
						if (!bits) {
							t = 1 - (video.bits->getBit() << 1);
						} else {
							t = video.bits->getBits(bits) | mask;

							int sign = -video.bits->getBit();
							t = (t ^ sign) - sign;
						}
						//block[scan[ccoef]] = t;
						coefIdx[coefCount++] = ccoef;
					}
				}
				break;

			case 1:
				modeList[listPos] = 2;
				for (int i = 0; i < 3; i++) {
					ccoef += 4;
					coefList[listEnd]   = ccoef;
					modeList[listEnd++] = 2;
				}
				break;

			case 3:
				int t;
				if (!bits) {
					t = 1 - (video.bits->getBit() << 1);
				} else {
					t = video.bits->getBits(bits) | mask;

					int sign = -video.bits->getBit();
					t = (t ^ sign) - sign;
				}
				// block[scan[ccoef]]   = t;
				coefIdx[coefCount++] = ccoef;
				coefList[listPos]    = 0;
				modeList[listPos++]  = 0;
				break;
			}
		}
	}

	uint8 quantIdx = video.bits->getBits(4);
	const uint32 *quant = isIntra ? binkIntraQuant[quantIdx] : binkInterQuant[quantIdx];
	//block[0] = (block[0] * quant[0]) >> 11;

	for (int i = 0; i < coefCount; i++) {
		int idx = coefIdx[i];
		//block[scan[idx]] = (block[scan[idx]] * quant[idx]) >> 11;
	}

}

/** Reads 8x8 block with residue after motion compensation. */
void Bink::readResidue(VideoFrame &video, void *block, int masksCount) {
	int nzCoeff[64];
	int nzCoeffCount = 0;

	int listStart = 64;
	int listEnd   = 64;

	int coefList[128];      int modeList[128];
	coefList[listEnd] =  4; modeList[listEnd++] = 0;
	coefList[listEnd] = 24; modeList[listEnd++] = 0;
	coefList[listEnd] = 44; modeList[listEnd++] = 0;
	coefList[listEnd] =  0; modeList[listEnd++] = 2;

	for (int mask = 1 << video.bits->getBits(3); mask; mask >>= 1) {

		for (int i = 0; i < nzCoeffCount; i++) {
			if (!video.bits->getBit())
				continue;
			/*
			if (block[nzCoeff[i]] < 0)
				block[nzCoeff[i]] -= mask;
			else
				block[nzCoeff[i]] += mask;
			*/
			masksCount--;
			if (masksCount < 0)
				return;
		}

		int listPos = listStart;
		while (listPos < listEnd) {

			if (!(coefList[listPos] | modeList[listPos]) || !video.bits->getBit()) {
				listPos++;
				continue;
			}

			int ccoef = coefList[listPos];
			int mode  = modeList[listPos];

			switch (mode) {
			case 0:
				coefList[listPos] = ccoef + 4;
				modeList[listPos] = 1;
			case 2:
				if (mode == 2) {
					coefList[listPos]   = 0;
					modeList[listPos++] = 0;
				}

				for (int i = 0; i < 4; i++, ccoef++) {
					if (video.bits->getBit()) {
						coefList[--listStart] = ccoef;
						modeList[  listStart] = 3;
					} else {
						nzCoeff[nzCoeffCount++] = binkScan[ccoef];

						int sign = -video.bits->getBit();
						//block[bink_scan[ccoef]] = (mask ^ sign) - sign;

						masksCount--;
						if (masksCount < 0)
							return;
					}
				}
				break;

			case 1:
				modeList[listPos] = 2;
				for (int i = 0; i < 3; i++) {
					ccoef += 4;
					coefList[listEnd]   = ccoef;
					modeList[listEnd++] = 2;
				}
				break;

			case 3:
				nzCoeff[nzCoeffCount++] = binkScan[ccoef];

				int sign = -video.bits->getBit();
				//block[bink_scan[ccoef]] = (mask ^ sign) - sign;

				coefList[listPos]   = 0;
				modeList[listPos++] = 0;
				masksCount--;
				if (masksCount < 0)
					return;
				break;
			}
		}
	}
}

} // End of namespace Graphics
