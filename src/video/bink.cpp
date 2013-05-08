/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Based quite heavily on the Bink decoder found in FFmpeg.
// Many thanks to Kostya Shishkov for doing the hard work.

/** @file video/bink.cpp
 *  Decoding RAD Game Tools' Bink videos.
 */

#include <cmath>

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/file.h"
#include "common/ustring.h"
#include "common/bitstream.h"
#include "common/huffman.h"
#include "common/rdft.h"
#include "common/dct.h"

#include "graphics/util.h"
#include "graphics/yuv_to_rgb.h"

#include "graphics/images/surface.h"

#include "sound/decoders/util.h"

#include "video/bink.h"
#include "video/binkdata.h"

#include "events/events.h"

static const uint32 kBIKfID = MKTAG('B', 'I', 'K', 'f');
static const uint32 kBIKgID = MKTAG('B', 'I', 'K', 'g');
static const uint32 kBIKhID = MKTAG('B', 'I', 'K', 'h');
static const uint32 kBIKiID = MKTAG('B', 'I', 'K', 'i');

static const uint32 kVideoFlagAlpha = 0x00100000;

static const uint16 kAudioFlagDCT    = 0x1000;
static const uint16 kAudioFlagStereo = 0x2000;

// Number of bits used to store first DC value in bundle
static const uint32 kDCStartBits = 11;

namespace Video {

Bink::VideoFrame::VideoFrame() : bits(0) {
}

Bink::VideoFrame::~VideoFrame() {
	delete bits;
}


Bink::AudioTrack::AudioTrack() : bits(0), bands(0), rdft(0), dct(0) {
}

Bink::AudioTrack::~AudioTrack() {
	delete bits;

	delete[] bands;

	delete rdft;
	delete dct;
}


Bink::Bink(Common::SeekableReadStream *bink) : _bink(bink), _disableAudio(false),
	_curFrame(0), _audioTrack(0) {

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
}

Bink::~Bink() {
	VideoDecoder::deinit();

	for (int i = 0; i < 4; i++) {
		delete[] _curPlanes[i];
		delete[] _oldPlanes[i];
	}

	deinitBundles();

	for (int i = 0; i < 16; i++)
		delete _huffman[i];

	delete _bink;
}

uint32 Bink::getTimeToNextFrame() const {
	if (!_started)
		return 0;

	uint32 curTime   = EventMan.getTimestamp() - _startTime;
	uint32 frameTime = ((uint64) (_curFrame * 1000 * ((uint64) _fpsDen))) / _fpsNum;

	// We need the next frame now
	if (frameTime <= curTime)
		return 0;

	// We need the next frame later
	return frameTime - curTime;
}

void Bink::startVideo() {
	uint32 curTime = EventMan.getTimestamp();

	_startTime     = curTime;
	_lastFrameTime = curTime;
	_started       = true;
}

void Bink::processData() {
	if (getTimeToNextFrame() > 0)
		return;

	if (_curFrame >= _frames.size()) {
		finish();
		return;
	}

	VideoFrame &frame = _frames[_curFrame];

	if (!_bink->seek(frame.offset))
		throw Common::Exception(Common::kSeekError);

	uint32 frameSize = frame.size;

	for (uint32 i = 0; i < _audioTracks.size(); i++) {
		AudioTrack &audio = _audioTracks[i];

		uint32 audioPacketLength = _bink->readUint32LE();

		frameSize -= 4;

		if (frameSize < audioPacketLength)
			throw Common::Exception("Audio packet too big for the frame");

		if (audioPacketLength >= 4) {
			uint32 audioPacketStart = _bink->pos();
			uint32 audioPacketEnd   = _bink->pos() + audioPacketLength;

			if (i == _audioTrack) {
				// Only play one audio track

				//                  Number of samples in bytes
				audio.sampleCount = _bink->readUint32LE() / (2 * audio.channels);

				audio.bits =
					new Common::BitStream32LELSB(new Common::SeekableSubReadStream(_bink,
					    audioPacketStart + 4, audioPacketEnd), true);

				audioPacket(audio);

				delete audio.bits;
				audio.bits = 0;
			}

			_bink->seek(audioPacketEnd);

			frameSize -= audioPacketLength;
		}
	}

	uint32 videoPacketStart = _bink->pos();
	uint32 videoPacketEnd   = _bink->pos() + frameSize;

	frame.bits =
		new Common::BitStream32LELSB(new Common::SeekableSubReadStream(_bink,
		    videoPacketStart, videoPacketEnd), true);

	videoPacket(frame);

	delete frame.bits;
	frame.bits = 0;

	_needCopy = true;

	_curFrame++;
}

void Bink::audioPacket(AudioTrack &audio) {
	if (_disableAudio)
		return;

	int outSize = audio.frameLen * audio.channels;
	while (!_disableAudio && (audio.bits->pos() < audio.bits->size())) {
		int16 *out = new int16[outSize];
		memset(out, 0, outSize * 2);

		audioBlock(audio, out);

		if (_disableAudio) {
			delete[] out;
			return;
		}

		queueSound((const byte *) out, audio.blockSize * 2);

		if (audio.bits->pos() & 0x1F) // next data block starts at a 32-byte boundary
			audio.bits->skip(32 - (audio.bits->pos() & 0x1F));
	}
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
	assert(_surface && _curPlanes[0] && _curPlanes[1] && _curPlanes[2] && _curPlanes[3]);
	YUVToRGBMan.convert420(Graphics::YUVToRGBManager::kScaleITU,
			_surface->getData(), _surface->getWidth() * 4,
			_curPlanes[0], _curPlanes[1], _curPlanes[2], _curPlanes[3],
			_width, _height, _width, _width >> 1);

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

	initVideo(width, height);

	_fpsNum = _bink->readUint32LE();
	_fpsDen = _bink->readUint32LE();

	if ((_fpsNum == 0) || (_fpsDen == 0))
		throw Common::Exception("Invalid FPS (%d/%d)", _fpsNum, _fpsDen);

	_videoFlags = _bink->readUint32LE();

	uint32 audioTrackCount = _bink->readUint32LE();

	if (audioTrackCount > 1) {
		warning("More than one audio track found. Using the first one");

		_audioTrack = 0;
	}

	if (audioTrackCount > 0) {
		_audioTracks.reserve(audioTrackCount);

		_bink->skip(4 * audioTrackCount);

		// Reading audio track properties
		for (uint32 i = 0; i < audioTrackCount; i++) {
			AudioTrack track;

			track.sampleRate = _bink->readUint16LE();
			track.flags      = _bink->readUint16LE();

			_audioTracks.push_back(track);

			initAudioTrack(_audioTracks[i]);
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

	if (_audioTrack < _audioTracks.size()) {
		const AudioTrack &audio = _audioTracks[_audioTrack];

		initSound(audio.outSampleRate, audio.outChannels, true);
	}
}

void Bink::initAudioTrack(AudioTrack &audio) {
	audio.sampleCount = 0;
	audio.bits        = 0;

	audio.channels = ((audio.flags & kAudioFlagStereo) != 0) ? 2 : 1;
	audio.codec    = ((audio.flags & kAudioFlagDCT   ) != 0) ? kAudioCodecDCT : kAudioCodecRDFT;

	if (audio.channels > kAudioChannelsMax)
		throw Common::Exception("Too many audio channels: %d", audio.channels);

	uint32 frameLenBits;
	// Calculate frame length
	if      (audio.sampleRate < 22050)
		frameLenBits =  9;
	else if (audio.sampleRate < 44100)
		frameLenBits = 10;
	else
		frameLenBits = 11;

	audio.frameLen = 1 << frameLenBits;

	audio.outSampleRate = audio.sampleRate;
	audio.outChannels   = audio.channels;

	if (audio.codec  == kAudioCodecRDFT) {
		// RDFT audio already interleaves the samples correctly

		if (audio.channels == 2)
			frameLenBits++;

		audio.sampleRate *= audio.channels;
		audio.frameLen   *= audio.channels;
		audio.channels    = 1;
	}

	audio.overlapLen = audio.frameLen / 16;
	audio.blockSize  = (audio.frameLen - audio.overlapLen) * audio.channels;
	audio.root       = 2.0 / sqrt((float) audio.frameLen);

	uint32 sampleRateHalf = (audio.sampleRate + 1) / 2;

	// Calculate number of bands
	for (audio.bandCount = 1; audio.bandCount < 25; audio.bandCount++)
		if (sampleRateHalf <= binkCriticalFreqs[audio.bandCount - 1])
			break;

	audio.bands = new uint32[audio.bandCount + 1];

	// Populate bands
	audio.bands[0] = 1;
	for (uint32 i = 1; i < audio.bandCount; i++)
		audio.bands[i] = binkCriticalFreqs[i - 1] * (audio.frameLen / 2) / sampleRateHalf;
	audio.bands[audio.bandCount] = audio.frameLen / 2;

	audio.first = true;

	for (uint8 i = 0; i < audio.channels; i++)
		audio.coeffsPtr[i] = audio.coeffs + i * audio.frameLen;

	audio.codec = ((audio.flags & kAudioFlagDCT) != 0) ? kAudioCodecDCT : kAudioCodecRDFT;

	if      (audio.codec == kAudioCodecRDFT)
		audio.rdft = new Common::RDFT(frameLenBits, Common::RDFT::DFT_C2R);
	else if (audio.codec == kAudioCodecDCT)
		audio.dct  = new Common::DCT(frameLenBits, Common::DCT::DCT_III);
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

		_bundles[kSourceBlockTypes   ].countLengths[i] = Common::intLog2((width  >> 3)    + 511) + 1;
		_bundles[kSourceSubBlockTypes].countLengths[i] = Common::intLog2((width  >> 4)    + 511) + 1;
		_bundles[kSourceColors       ].countLengths[i] = Common::intLog2((width  >> 3)*64 + 511) + 1;
		_bundles[kSourceIntraDC      ].countLengths[i] = Common::intLog2((width  >> 3)    + 511) + 1;
		_bundles[kSourceInterDC      ].countLengths[i] = Common::intLog2((width  >> 3)    + 511) + 1;
		_bundles[kSourceXOff         ].countLengths[i] = Common::intLog2((width  >> 3)    + 511) + 1;
		_bundles[kSourceYOff         ].countLengths[i] = Common::intLog2((width  >> 3)    + 511) + 1;
		_bundles[kSourcePattern      ].countLengths[i] = Common::intLog2((cbw[i] << 3)    + 511) + 1;
		_bundles[kSourceRun          ].countLengths[i] = Common::intLog2((width  >> 3)*48 + 511) + 1;
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

void Bink::blockScaledSkip(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *prev = ctx.prev;

	for (int j = 0; j < 16; j++, dest += ctx.pitch, prev += ctx.pitch)
		memcpy(dest, prev, 16);
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
	int16 block[64];
	memset(block, 0, 64 * sizeof(int16));

	block[0] = getBundleValue(kSourceIntraDC);

	readDCTCoeffs(*ctx.video, block, true);

	IDCT(block);

	int16 *src   = block;
	byte  *dest1 = ctx.dest;
	byte  *dest2 = ctx.dest + ctx.pitch;
	for (int j = 0; j < 8; j++, dest1 += (ctx.pitch << 1) - 16, dest2 += (ctx.pitch << 1) - 16, src += 8) {

		for (int i = 0; i < 8; i++, dest1 += 2, dest2 += 2)
			dest1[0] = dest1[1] = dest2[0] = dest2[1] = src[i];

	}
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

		for (int i = 0; i < 8; i++, dest1 += 2, dest2 += 2, v >>= 1)
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
			throw Common::Exception("Invalid 16x16 block type: %d", blockType);
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
	blockMotion(ctx);

	byte v = ctx.video->bits->getBits(7);

	int16 block[64];
	memset(block, 0, 64 * sizeof(int16));

	readResidue(*ctx.video, block, v);

	byte  *dst = ctx.dest;
	int16 *src = block;
	for (int i = 0; i < 8; i++, dst += ctx.pitch, src += 8)
		for (int j = 0; j < 8; j++)
			dst[j] += src[j];
}

void Bink::blockIntra(DecodeContext &ctx) {
	int16 block[64];
	memset(block, 0, 64 * sizeof(int16));

	block[0] = getBundleValue(kSourceIntraDC);

	readDCTCoeffs(*ctx.video, block, true);

	IDCTPut(ctx, block);
}

void Bink::blockFill(DecodeContext &ctx) {
	byte v = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 8; i++, dest += ctx.pitch)
		memset(dest, v, 8);
}

void Bink::blockInter(DecodeContext &ctx) {
	blockMotion(ctx);

	int16 block[64];
	memset(block, 0, 64 * sizeof(int16));

	block[0] = getBundleValue(kSourceInterDC);

	readDCTCoeffs(*ctx.video, block, false);

	IDCTAdd(ctx, block);
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

/* WORKAROUND: This fixes the NWN2 WotC logo.
 * [cf. ffmpeg 47b71eea099b3fe2c7e16644878ad9b7067974e3] */
static inline int16 dequant(int16 in, uint32 quant, bool dc) {
	/* Note: multiplication is unsigned but we want signed shift
	 * otherwise clipping breaks.
	 *
	 * TODO: The official decoder does not use clipping at all
	 * but instead uses the full 32-bit result.
	 * However clipping at least gets rid of the case that a
	 * half-black half-white intra block gets black and white swapped
	 * and should cause at most minor differences (except for DC).
	 */

	int32 res = ((int32) (in * quant)) >> 11;
	if (!dc)
		res = CLIP(res, -32768, 32767);

	return res;
}

/** Reads 8x8 block of DCT coefficients. */
void Bink::readDCTCoeffs(VideoFrame &video, int16 *block, bool isIntra) {
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
						block[binkScan[ccoef]] = t;
						coefIdx[coefCount++]   = ccoef;
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
				block[binkScan[ccoef]] = t;
				coefIdx[coefCount++]   = ccoef;
				coefList[listPos]      = 0;
				modeList[listPos++]    = 0;
				break;
			}
		}
	}

	uint8 quantIdx = video.bits->getBits(4);
	const uint32 *quant = isIntra ? binkIntraQuant[quantIdx] : binkInterQuant[quantIdx];
	block[0] = dequant(block[0], quant[0], true);

	for (int i = 0; i < coefCount; i++) {
		int idx = coefIdx[i];
		block[binkScan[idx]] = dequant(block[binkScan[idx]], quant[idx], false);
	}

}

/** Reads 8x8 block with residue after motion compensation. */
void Bink::readResidue(VideoFrame &video, int16 *block, int masksCount) {
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
			if (block[nzCoeff[i]] < 0)
				block[nzCoeff[i]] -= mask;
			else
				block[nzCoeff[i]] += mask;
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
						block[binkScan[ccoef]] = (mask ^ sign) - sign;

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
				block[binkScan[ccoef]] = (mask ^ sign) - sign;

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

float Bink::getFloat(AudioTrack &audio) {
	int power = audio.bits->getBits(5);

	float f = ldexpf(audio.bits->getBits(23), power - 23);

	if (audio.bits->getBit())
		f = -f;

	return f;
}

void Bink::audioBlock(AudioTrack &audio, int16 *out) {
	if      (audio.codec == kAudioCodecDCT)
		audioBlockDCT (audio);
	else if (audio.codec == kAudioCodecRDFT)
		audioBlockRDFT(audio);

	Sound::floatToInt16Interleave(out, (const float **) audio.coeffsPtr,
	                              audio.frameLen, audio.channels);

	if (!audio.first) {
		int count = audio.overlapLen * audio.channels;
		int shift = Common::intLog2(count);
		for (int i = 0; i < count; i++) {
			out[i] = (audio.prevCoeffs[i] * (count - i) + out[i] * i) >> shift;
		}
	}

	memcpy(audio.prevCoeffs, out + audio.blockSize, audio.overlapLen * audio.channels * sizeof(*out));

	audio.first = false;
}

void Bink::audioBlockDCT(AudioTrack &audio) {
	audio.bits->skip(2);

	for (uint8 i = 0; i < audio.channels; i++) {
		float *coeffs = audio.coeffsPtr[i];

		readAudioCoeffs(audio, coeffs);

		coeffs[0] /= 0.5;

		audio.dct->calc(coeffs);

		for (uint32 j = 0; j < audio.frameLen; j++)
			coeffs[j] *= (audio.frameLen / 2.0);
	}

}

void Bink::audioBlockRDFT(AudioTrack &audio) {
	for (uint8 i = 0; i < audio.channels; i++) {
		float *coeffs = audio.coeffsPtr[i];

		readAudioCoeffs(audio, coeffs);

		audio.rdft->calc(coeffs);
	}
}

static const uint8 rleLengthTab[16] = {
	2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 32, 64
};

void Bink::readAudioCoeffs(AudioTrack &audio, float *coeffs) {
	coeffs[0] = getFloat(audio) * audio.root;
	coeffs[1] = getFloat(audio) * audio.root;

	float quant[25];

	for (uint32 i = 0; i < audio.bandCount; i++) {
		int value = audio.bits->getBits(8);

		//                               0.066399999 / log10(M_E)
		quant[i] = expf(MIN(value, 95) * 0.15289164787221953823f) * audio.root;
	}

	float q = 0.0;

	// Find band (k)
	int k;
	for (k = 0; audio.bands[k] < 1; k++)
		q = quant[k];

	// Parse coefficients
	uint32 i = 2;
	while (i < audio.frameLen) {

		uint32 j = 0;
		if (audio.bits->getBit())
			j = i + rleLengthTab[audio.bits->getBits(4)] * 8;
		else
			j = i + 8;

		j = MIN(j, audio.frameLen);

		int width = audio.bits->getBits(4);
		if (width == 0) {

			memset(coeffs + i, 0, (j - i) * sizeof(*coeffs));
			i = j;
			while (audio.bands[k] * 2 < i)
				q = quant[k++];

		} else {

			while (i < j) {
				if (audio.bands[k] * 2 == i)
					q = quant[k++];

				int coeff = audio.bits->getBits(width);
				if (coeff) {

					if (audio.bits->getBit())
						coeffs[i] = -q * coeff;
					else
						coeffs[i] =  q * coeff;

				} else {
					coeffs[i] = 0.0;
				}
				i++;
			}

		}

	}

}

#define A1  2896 /* (1/sqrt(2))<<12 */
#define A2  2217
#define A3  3784
#define A4 -5352

#define IDCT_TRANSFORM(dest,s0,s1,s2,s3,s4,s5,s6,s7,d0,d1,d2,d3,d4,d5,d6,d7,munge,src) {\
    const int a0 = (src)[s0] + (src)[s4]; \
    const int a1 = (src)[s0] - (src)[s4]; \
    const int a2 = (src)[s2] + (src)[s6]; \
    const int a3 = (A1*((src)[s2] - (src)[s6])) >> 11; \
    const int a4 = (src)[s5] + (src)[s3]; \
    const int a5 = (src)[s5] - (src)[s3]; \
    const int a6 = (src)[s1] + (src)[s7]; \
    const int a7 = (src)[s1] - (src)[s7]; \
    const int b0 = a4 + a6; \
    const int b1 = (A3*(a5 + a7)) >> 11; \
    const int b2 = ((A4*a5) >> 11) - b0 + b1; \
    const int b3 = (A1*(a6 - a4) >> 11) - b2; \
    const int b4 = ((A2*a7) >> 11) + b3 - b1; \
    (dest)[d0] = munge(a0+a2   +b0); \
    (dest)[d1] = munge(a1+a3-a2+b2); \
    (dest)[d2] = munge(a1-a3+a2+b3); \
    (dest)[d3] = munge(a0-a2   -b4); \
    (dest)[d4] = munge(a0-a2   +b4); \
    (dest)[d5] = munge(a1-a3+a2-b3); \
    (dest)[d6] = munge(a1+a3-a2-b2); \
    (dest)[d7] = munge(a0+a2   -b0); \
}
/* end IDCT_TRANSFORM macro */

#define MUNGE_NONE(x) (x)
#define IDCT_COL(dest,src) IDCT_TRANSFORM(dest,0,8,16,24,32,40,48,56,0,8,16,24,32,40,48,56,MUNGE_NONE,src)

#define MUNGE_ROW(x) (((x) + 0x7F)>>8)
#define IDCT_ROW(dest,src) IDCT_TRANSFORM(dest,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,MUNGE_ROW,src)

static inline void IDCTCol(int16 *dest, const int16 *src)
{
	if ((src[8] | src[16] | src[24] | src[32] | src[40] | src[48] | src[56]) == 0) {
		dest[ 0] =
		dest[ 8] =
		dest[16] =
		dest[24] =
		dest[32] =
		dest[40] =
		dest[48] =
		dest[56] = src[0];
	} else {
		IDCT_COL(dest, src);
	}
}

void Bink::IDCT(int16 *block) {
	int i;
	int16 temp[64];

	for (i = 0; i < 8; i++)
		IDCTCol(&temp[i], &block[i]);
	for (i = 0; i < 8; i++) {
		IDCT_ROW( (&block[8*i]), (&temp[8*i]) );
	}
}

void Bink::IDCTAdd(DecodeContext &ctx, int16 *block) {
	int i, j;

	IDCT(block);
	byte *dest = ctx.dest;
	for (i = 0; i < 8; i++, dest += ctx.pitch, block += 8)
		for (j = 0; j < 8; j++)
			 dest[j] += block[j];
}

void Bink::IDCTPut(DecodeContext &ctx, int16 *block) {
	int i;
	int16 temp[64];
	for (i = 0; i < 8; i++)
		IDCTCol(&temp[i], &block[i]);
	for (i = 0; i < 8; i++) {
		IDCT_ROW( (&ctx.dest[i*ctx.pitch]), (&temp[8*i]) );
	}
}

} // End of namespace Video
