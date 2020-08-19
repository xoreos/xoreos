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
 *  Decoding RAD Game Tools' Bink videos.
 */

/* Based on the Bink implementation in FFmpeg (<https://ffmpeg.org/)>,
 * which is released under the terms of version 2 or later of the GNU
 * Lesser General Public License.
 *
 * The original copyright notes in the files
 * - libavformat/bink.c
 * - libavcodec/bink.c
 * - libavcodec/binkdata.h
 * - libavcodec/binkdsp.c
 * - libavcodec/binkdsp.h
 * - libavcodec/binkaudio.c
 * read as follows:
 *
 * Bink demuxer
 * Copyright (c) 2008-2010 Peter Ross (pross@xvid.org)
 * Copyright (c) 2009 Daniel Verkamp (daniel@drv.nu)
 *
 * Bink video decoder
 * Copyright (c) 2009 Konstantin Shishkov
 * Copyright (C) 2011 Peter Ross <pross@xvid.org>
 *
 * Bink video decoder
 * Copyright (C) 2009 Konstantin Shishkov
 *
 * Bink DSP routines
 * Copyright (c) 2009 Konstantin Shishkov
 *
 * Bink Audio decoder
 * Copyright (c) 2007-2011 Peter Ross (pross@xvid.org)
 * Copyright (c) 2009 Daniel Verkamp (daniel@drv.nu)
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
#include <cmath>
#include <cstring>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/memreadstream.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/bitstream.h"
#include "src/common/huffman.h"
#include "src/common/rdft.h"
#include "src/common/dct.h"

#include "src/graphics/yuv_to_rgb.h"

#include "src/graphics/images/surface.h"

#include "src/sound/audiostream.h"
#include "src/sound/decoders/pcm.h"
#include "src/sound/decoders/util.h"

#include "src/video/bink.h"
#include "src/video/binkdata.h"

static const uint32_t kBIKfID = MKTAG('B', 'I', 'K', 'f');
static const uint32_t kBIKgID = MKTAG('B', 'I', 'K', 'g');
static const uint32_t kBIKhID = MKTAG('B', 'I', 'K', 'h');
static const uint32_t kBIKiID = MKTAG('B', 'I', 'K', 'i');

static const uint32_t kBIKbID = MKTAG('B', 'I', 'K', 'b');
static const uint32_t kBIKkID = MKTAG('B', 'I', 'K', 'k');

static const uint32_t kKB2aID = MKTAG('K', 'B', '2', 'a');
static const uint32_t kKB2dID = MKTAG('K', 'B', '2', 'd');
static const uint32_t kKB2fID = MKTAG('K', 'B', '2', 'f');
static const uint32_t kKB2gID = MKTAG('K', 'B', '2', 'g');
static const uint32_t kKB2hID = MKTAG('K', 'B', '2', 'h');
static const uint32_t kKB2iID = MKTAG('K', 'B', '2', 'i');
static const uint32_t kKB2jID = MKTAG('K', 'B', '2', 'j');
static const uint32_t kKB2kID = MKTAG('K', 'B', '2', 'k');

static const uint32_t kVideoFlagAlpha = 0x00100000;

static const uint16_t kAudioFlagDCT    = 0x1000;
static const uint16_t kAudioFlagStereo = 0x2000;

// Number of bits used to store first DC value in bundle
static const uint32_t kDCStartBits = 11;

namespace Video {

Bink::BinkVideoTrack::Huffman::Huffman() : index(0) {
	for (size_t i = 0; i < 16; i++)
		symbols[i] = i;
}


Bink::BinkVideoTrack::Bundle::Bundle() : countLength(0), dataEnd(0), curDec(0), curPtr(0) {
	countLengths[0] = countLengths[1] = 0;
}


Bink::VideoFrame::VideoFrame() : bits(0) {
}

Bink::VideoFrame::~VideoFrame() {
	delete bits;
}


Bink::AudioInfo::AudioInfo() : bands(0), rdft(0), dct(0) {
}

Bink::AudioInfo::~AudioInfo() {
	delete[] bands;

	delete rdft;
	delete dct;
}


Bink::Bink(Common::SeekableReadStream *bink) : _bink(bink), _audioTrack(0) {
	assert(_bink);

	load();
}

Bink::~Bink() {
}

void Bink::decodeNextTrackFrame(VideoTrack &track) {
	BinkVideoTrack &videoTrack = static_cast<BinkVideoTrack &>(track);

	VideoFrame &frame = _frames[videoTrack.getCurFrame() + 1];

	_bink->seek(frame.offset);
	uint32_t frameSize = frame.size;

	// Skip over the audio tracks
	for (size_t i = 0; i < _audioTracks.size(); i++) {
		uint32_t audioPacketLength = _bink->readUint32LE();
		frameSize -= 4;

		if (audioPacketLength > frameSize)
			throw Common::Exception("Audio packet too big for the frame");

		_bink->skip(audioPacketLength);
		frameSize -= audioPacketLength;
	}

	size_t videoPacketStart = _bink->pos();
	size_t videoPacketEnd   = _bink->pos() + frameSize;

	frame.bits =
		new Common::BitStream32LELSB(new Common::SeekableSubReadStream(_bink.get(),
		    videoPacketStart, videoPacketEnd), true);

	assert(_surface);
	videoTrack.decodePacket(*_surface, frame);

	delete frame.bits;
	frame.bits = 0;

	_needCopy = true;
}

void Bink::checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime) {
	static_cast<BinkAudioTrack &>(track).decodeAudio(*_bink, _frames, _audioTracks, endTime);
}

void Bink::BinkVideoTrack::decodePacket(Graphics::Surface &surface, VideoFrame &video) {
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
	assert(_curPlanes[0] && _curPlanes[1] && _curPlanes[2] && _curPlanes[3]);
	YUVToRGBMan.convert420(Graphics::YUVToRGBManager::kScaleITU,
			surface.getData(), surface.getWidth() * 4,
			_curPlanes[0].get(), _curPlanes[1].get(), _curPlanes[2].get(), _curPlanes[3].get(),
			_width, _height, _width, _width >> 1);

	// And swap the planes with the reference planes
	for (int i = 0; i < 4; i++)
		_oldPlanes[i].swap(_curPlanes[i]);

	_curFrame++;
}

void Bink::BinkVideoTrack::decodePlane(VideoFrame &video, int planeIdx, bool isChroma) {
	uint32_t blockWidth  = isChroma ? ((_width  + 15) >> 4) : ((_width  + 7) >> 3);
	uint32_t blockHeight = isChroma ? ((_height + 15) >> 4) : ((_height + 7) >> 3);
	uint32_t width       = isChroma ?  (_width        >> 1) :   _width;
	uint32_t height      = isChroma ?  (_height       >> 1) :   _height;

	DecodeContext ctx;

	ctx.video     = &video;
	ctx.planeIdx  = planeIdx;
	ctx.destStart = _curPlanes[planeIdx].get();
	ctx.destEnd   = _curPlanes[planeIdx].get() + width * height;
	ctx.prevStart = _oldPlanes[planeIdx].get();
	ctx.prevEnd   = _oldPlanes[planeIdx].get() + width * height;
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

void Bink::BinkVideoTrack::readBundle(VideoFrame &video, Source source) {
	if (source == kSourceColors) {
		for (int i = 0; i < 16; i++)
			readHuffman(video, _colHighHuffman[i]);

		_colLastVal = 0;
	}

	if ((source != kSourceIntraDC) && (source != kSourceInterDC))
		readHuffman(video, _bundles[source].huffman);

	_bundles[source].curDec = _bundles[source].data.get();
	_bundles[source].curPtr = _bundles[source].data.get();
}

void Bink::BinkVideoTrack::readHuffman(VideoFrame &video, Huffman &huffman) {
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

		std::memset(hasSymbol, 0, 16);

		uint8_t length = video.bits->getBits(3);
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

	uint8_t depth = video.bits->getBits(2);

	for (int i = 0; i < 16; i++)
		in[i] = i;

	for (int i = 0; i <= depth; i++) {
		int size = 1 << i;

		for (int j = 0; j < 16; j += (size << 1))
			mergeHuffmanSymbols(video, out + j, in + j, size);

		std::swap(in, out);
	}

	std::memcpy(huffman.symbols, in, 16);
}

void Bink::BinkVideoTrack::mergeHuffmanSymbols(VideoFrame &video, byte *dst, const byte *src, int size) {
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
	uint32_t id = _bink->readUint32BE();
	if ((id == kKB2aID) || (id == kKB2dID) || (id == kKB2fID) || (id == kKB2gID) ||
	    (id == kKB2hID) || (id == kKB2iID) || (id == kKB2jID) || (id == kKB2kID))
		throw Common::Exception("Bink 2 (%s) is not supported", Common::debugTag(id).c_str());
	if ((id == kBIKbID) || (id == kBIKkID))
		throw Common::Exception("Untested Bink version %s", Common::debugTag(id).c_str());
	if ((id != kBIKfID) && (id != kBIKgID) && (id != kBIKhID) && (id != kBIKiID))
		throw Common::Exception("Unknown Bink FourCC %s", Common::debugTag(id).c_str());

	uint32_t fileSize         = _bink->readUint32LE() + 8;
	uint32_t frameCount       = _bink->readUint32LE();
	uint32_t largestFrameSize = _bink->readUint32LE();

	if (largestFrameSize > fileSize)
		throw Common::Exception("Largest frame size greater than file size");

	_bink->skip(4);

	uint32_t width  = _bink->readUint32LE();
	uint32_t height = _bink->readUint32LE();

	uint32_t fpsNum = _bink->readUint32LE();
	uint32_t fpsDen = _bink->readUint32LE();

	if ((fpsNum == 0) || (fpsDen == 0))
		throw Common::Exception("Invalid FPS (%d/%d)", fpsNum, fpsDen);

	uint32_t videoFlags = _bink->readUint32LE();

	addTrack(new BinkVideoTrack(width, height, frameCount, Common::Rational(fpsNum, fpsDen), (id == kBIKhID || id == kBIKiID), (videoFlags & kVideoFlagAlpha) != 0, id));

	uint32_t audioTrackCount = _bink->readUint32LE();

	if (audioTrackCount > 1) {
		warning("More than one audio track found. Using the first one");

		_audioTrack = 0;
	}

	if (audioTrackCount > 0) {
		_audioTracks.resize(audioTrackCount);

		_bink->skip(4 * audioTrackCount);

		// Reading audio track properties
		for (uint32_t i = 0; i < audioTrackCount; i++) {
			AudioInfo &track = _audioTracks[i];

			track.sampleRate = _bink->readUint16LE();
			track.flags      = _bink->readUint16LE();

			initAudioTrack(track);
		}

		_bink->skip(4 * audioTrackCount);
	}

	// Reading video frame properties
	_frames.resize(frameCount);
	for (uint32_t i = 0; i < frameCount; i++) {
		_frames[i].offset   = _bink->readUint32LE();
		_frames[i].keyFrame = _frames[i].offset & 1;

		_frames[i].offset &= ~1;

		if (i != 0)
			_frames[i - 1].size = _frames[i].offset - _frames[i - 1].offset;

		_frames[i].bits = 0;
	}

	_frames[frameCount - 1].size = _bink->size() - _frames[frameCount - 1].offset;

	initVideo();

	if (_audioTrack < _audioTracks.size())
		addTrack(new BinkAudioTrack(_audioTrack, _audioTracks[_audioTrack]));
}

void Bink::initAudioTrack(AudioInfo &audio) {
	audio.channels = ((audio.flags & kAudioFlagStereo) != 0) ? 2 : 1;
	audio.codec    = ((audio.flags & kAudioFlagDCT   ) != 0) ? kAudioCodecDCT : kAudioCodecRDFT;

	if (audio.channels > kAudioChannelsMax)
		throw Common::Exception("Too many audio channels: %d", audio.channels);

	uint32_t frameLenBits;
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
	audio.root       = 2.0 / sqrt((double) audio.frameLen);

	uint32_t sampleRateHalf = (audio.sampleRate + 1) / 2;

	// Calculate number of bands
	for (audio.bandCount = 1; audio.bandCount < 25; audio.bandCount++)
		if (sampleRateHalf <= binkCriticalFreqs[audio.bandCount - 1])
			break;

	audio.bands = new uint32_t[audio.bandCount + 1];

	// Populate bands
	audio.bands[0] = 1;
	for (uint32_t i = 1; i < audio.bandCount; i++)
		audio.bands[i] = binkCriticalFreqs[i - 1] * (audio.frameLen / 2) / sampleRateHalf;
	audio.bands[audio.bandCount] = audio.frameLen / 2;

	audio.first = true;

	for (uint8_t i = 0; i < audio.channels; i++)
		audio.coeffsPtr[i] = audio.coeffs + i * audio.frameLen;

	audio.codec = ((audio.flags & kAudioFlagDCT) != 0) ? kAudioCodecDCT : kAudioCodecRDFT;

	if      (audio.codec == kAudioCodecRDFT)
		audio.rdft = new Common::RDFT(frameLenBits, Common::RDFT::DFT_C2R);
	else if (audio.codec == kAudioCodecDCT)
		audio.dct  = new Common::DCT(frameLenBits, Common::DCT::DCT_III);
}

void Bink::BinkVideoTrack::initBundles() {
	uint32_t bw     = (_width  + 7) >> 3;
	uint32_t bh     = (_height + 7) >> 3;
	uint32_t blocks = bw * bh;

	for (int i = 0; i < kSourceMAX; i++) {
		_bundles[i].data = std::make_unique<byte[]>(blocks * 64);
		_bundles[i].dataEnd = _bundles[i].data.get() + blocks * 64;
	}

	uint32_t cbw[2] = { (_width + 7) >> 3, (_width  + 15) >> 4 };
	uint32_t cw [2] = {  _width          ,  _width        >> 1 };

	// Calculate the lengths of an element count in bits
	for (int i = 0; i < 2; i++) {
		int width = MAX<uint32_t>(cw[i], 8);

		_bundles[kSourceBlockTypes   ].countLengths[i] = Common::intLog2((width       >> 3) + 511) + 1;
		_bundles[kSourceSubBlockTypes].countLengths[i] = Common::intLog2(((width + 7) >> 4) + 511) + 1;
		_bundles[kSourceColors       ].countLengths[i] = Common::intLog2((cbw[i])     * 64  + 511) + 1;
		_bundles[kSourceIntraDC      ].countLengths[i] = Common::intLog2((width       >> 3) + 511) + 1;
		_bundles[kSourceInterDC      ].countLengths[i] = Common::intLog2((width       >> 3) + 511) + 1;
		_bundles[kSourceXOff         ].countLengths[i] = Common::intLog2((width       >> 3) + 511) + 1;
		_bundles[kSourceYOff         ].countLengths[i] = Common::intLog2((width       >> 3) + 511) + 1;
		_bundles[kSourcePattern      ].countLengths[i] = Common::intLog2((cbw[i]      << 3) + 511) + 1;
		_bundles[kSourceRun          ].countLengths[i] = Common::intLog2((cbw[i])     * 48  + 511) + 1;
	}
}

void Bink::BinkVideoTrack::initHuffman() {
	for (int i = 0; i < 16; i++)
		_huffman[i] = std::make_unique<Common::Huffman>(binkHuffmanLengths[i][15], 16, binkHuffmanCodes[i], binkHuffmanLengths[i]);
}

byte Bink::BinkVideoTrack::getHuffmanSymbol(VideoFrame &video, Huffman &huffman) {
	return huffman.symbols[_huffman[huffman.index]->getSymbol(*video.bits)];
}

int32_t Bink::BinkVideoTrack::getBundleValue(Source source) {
	if ((source < kSourceXOff) || (source == kSourceRun))
		return *_bundles[source].curPtr++;

	if ((source == kSourceXOff) || (source == kSourceYOff))
		return (int8_t) *_bundles[source].curPtr++;

	int16_t ret = *reinterpret_cast<int16_t *>(_bundles[source].curPtr);

	_bundles[source].curPtr += 2;

	return ret;
}

uint32_t Bink::BinkVideoTrack::readBundleCount(VideoFrame &video, Bundle &bundle) {
	if (!bundle.curDec || (bundle.curDec > bundle.curPtr))
		return 0;

	uint32_t n = video.bits->getBits(bundle.countLength);
	if (n == 0)
		bundle.curDec = 0;

	return n;
}

void Bink::BinkVideoTrack::blockSkip(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *prev = ctx.prev;

	for (int j = 0; j < 8; j++, dest += ctx.pitch, prev += ctx.pitch)
		std::memcpy(dest, prev, 8);
}

void Bink::BinkVideoTrack::blockScaledSkip(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *prev = ctx.prev;

	for (int j = 0; j < 16; j++, dest += ctx.pitch, prev += ctx.pitch)
		std::memcpy(dest, prev, 16);
}

void Bink::BinkVideoTrack::blockScaledRun(DecodeContext &ctx) {
	const uint8_t *scan = binkPatterns[ctx.video->bits->getBits(4)];

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

void Bink::BinkVideoTrack::blockScaledIntra(DecodeContext &ctx) {
	int16_t block[64];
	std::memset(block, 0, 64 * sizeof(int16_t));

	block[0] = getBundleValue(kSourceIntraDC);

	readDCTCoeffs(*ctx.video, block, true);

	IDCT(block);

	int16_t *src   = block;
	byte  *dest1 = ctx.dest;
	byte  *dest2 = ctx.dest + ctx.pitch;
	for (int j = 0; j < 8; j++, dest1 += (ctx.pitch << 1) - 16, dest2 += (ctx.pitch << 1) - 16, src += 8) {

		for (int i = 0; i < 8; i++, dest1 += 2, dest2 += 2)
			dest1[0] = dest1[1] = dest2[0] = dest2[1] = src[i];

	}
}

void Bink::BinkVideoTrack::blockScaledFill(DecodeContext &ctx) {
	byte v = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 16; i++, dest += ctx.pitch)
		std::memset(dest, v, 16);
}

void Bink::BinkVideoTrack::blockScaledPattern(DecodeContext &ctx) {
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

void Bink::BinkVideoTrack::blockScaledRaw(DecodeContext &ctx) {
	byte row[8];

	byte *dest1 = ctx.dest;
	byte *dest2 = ctx.dest + ctx.pitch;
	for (int j = 0; j < 8; j++, dest1 += (ctx.pitch << 1) - 16, dest2 += (ctx.pitch << 1) - 16) {
		std::memcpy(row, _bundles[kSourceColors].curPtr, 8);

		for (int i = 0; i < 8; i++, dest1 += 2, dest2 += 2)
			dest1[0] = dest1[1] = dest2[0] = dest2[1] = row[i];

		_bundles[kSourceColors].curPtr += 8;
	}
}

void Bink::BinkVideoTrack::blockScaled(DecodeContext &ctx) {
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

void Bink::BinkVideoTrack::blockMotion(DecodeContext &ctx) {
	int8_t xOff = getBundleValue(kSourceXOff);
	int8_t yOff = getBundleValue(kSourceYOff);

	byte *dest = ctx.dest;
	byte *prev = ctx.prev + yOff * ((int32_t) ctx.pitch) + xOff;
	if ((prev < ctx.prevStart) || (prev > ctx.prevEnd))
		throw Common::Exception("Copy out of bounds (%d | %d)", ctx.blockX * 8 + xOff, ctx.blockY * 8 + yOff);

	for (int j = 0; j < 8; j++, dest += ctx.pitch, prev += ctx.pitch)
		std::memcpy(dest, prev, 8);
}

void Bink::BinkVideoTrack::blockRun(DecodeContext &ctx) {
	const uint8_t *scan = binkPatterns[ctx.video->bits->getBits(4)];

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

void Bink::BinkVideoTrack::blockResidue(DecodeContext &ctx) {
	blockMotion(ctx);

	byte v = ctx.video->bits->getBits(7);

	int16_t block[64];
	std::memset(block, 0, 64 * sizeof(int16_t));

	readResidue(*ctx.video, block, v);

	byte  *dst = ctx.dest;
	int16_t *src = block;
	for (int i = 0; i < 8; i++, dst += ctx.pitch, src += 8)
		for (int j = 0; j < 8; j++)
			dst[j] += src[j];
}

void Bink::BinkVideoTrack::blockIntra(DecodeContext &ctx) {
	int16_t block[64];
	std::memset(block, 0, 64 * sizeof(int16_t));

	block[0] = getBundleValue(kSourceIntraDC);

	readDCTCoeffs(*ctx.video, block, true);

	IDCTPut(ctx, block);
}

void Bink::BinkVideoTrack::blockFill(DecodeContext &ctx) {
	byte v = getBundleValue(kSourceColors);

	byte *dest = ctx.dest;
	for (int i = 0; i < 8; i++, dest += ctx.pitch)
		std::memset(dest, v, 8);
}

void Bink::BinkVideoTrack::blockInter(DecodeContext &ctx) {
	blockMotion(ctx);

	int16_t block[64];
	std::memset(block, 0, 64 * sizeof(int16_t));

	block[0] = getBundleValue(kSourceInterDC);

	readDCTCoeffs(*ctx.video, block, false);

	IDCTAdd(ctx, block);
}

void Bink::BinkVideoTrack::blockPattern(DecodeContext &ctx) {
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

void Bink::BinkVideoTrack::blockRaw(DecodeContext &ctx) {
	byte *dest = ctx.dest;
	byte *data = _bundles[kSourceColors].curPtr;
	for (int i = 0; i < 8; i++, dest += ctx.pitch, data += 8)
		std::memcpy(dest, data, 8);

	_bundles[kSourceColors].curPtr += 64;
}

void Bink::BinkVideoTrack::readRuns(VideoFrame &video, Bundle &bundle) {
	uint32_t n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Run value went out of bounds");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		std::memset(bundle.curDec, v, n);
		bundle.curDec += n;

	} else
		while (bundle.curDec < decEnd)
			*bundle.curDec++ = getHuffmanSymbol(video, bundle.huffman);
}

void Bink::BinkVideoTrack::readMotionValues(VideoFrame &video, Bundle &bundle) {
	uint32_t n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many motion values");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		if (v) {
			int sign = -((int)video.bits->getBit());
			v = (v ^ sign) - sign;
		}

		std::memset(bundle.curDec, v, n);

		bundle.curDec += n;
		return;
	}

	do {
		byte v = getHuffmanSymbol(video, bundle.huffman);

		if (v) {
			int sign = -((int)video.bits->getBit());
			v = (v ^ sign) - sign;
		}

		*bundle.curDec++ = v;

	} while (bundle.curDec < decEnd);
}

const uint8_t rleLens[4] = { 4, 8, 12, 32 };
void Bink::BinkVideoTrack::readBlockTypes(VideoFrame &video, Bundle &bundle) {
	uint32_t n = readBundleCount(video, bundle);
	if (n == 0)
		return;

	byte *decEnd = bundle.curDec + n;
	if (decEnd > bundle.dataEnd)
		throw Common::Exception("Too many block type values");

	if (video.bits->getBit()) {
		byte v = video.bits->getBits(4);

		std::memset(bundle.curDec, v, n);

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

			std::memset(bundle.curDec, last, run);

			bundle.curDec += run;
		}

	} while (bundle.curDec < decEnd);
}

void Bink::BinkVideoTrack::readPatterns(VideoFrame &video, Bundle &bundle) {
	uint32_t n = readBundleCount(video, bundle);
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


void Bink::BinkVideoTrack::readColors(VideoFrame &video, Bundle &bundle) {
	uint32_t n = readBundleCount(video, bundle);
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
			int sign = ((int8_t) v) >> 7;
			v = ((v & 0x7F) ^ sign) - sign;
			v += 0x80;
		}

		std::memset(bundle.curDec, v, n);
		bundle.curDec += n;

		return;
	}

	while (bundle.curDec < decEnd) {
		_colLastVal = getHuffmanSymbol(video, _colHighHuffman[_colLastVal]);

		byte v;
		v = getHuffmanSymbol(video, bundle.huffman);
		v = (_colLastVal << 4) | v;

		if (_id != kBIKiID) {
			int sign = ((int8_t) v) >> 7;
			v = ((v & 0x7F) ^ sign) - sign;
			v += 0x80;
		}
		*bundle.curDec++ = v;
	}
}

void Bink::BinkVideoTrack::readDCS(VideoFrame &video, Bundle &bundle, int startBits, bool hasSign) {
	uint32_t length = readBundleCount(video, bundle);
	if (length == 0)
		return;

	int16_t *dest = reinterpret_cast<int16_t *>(bundle.curDec);

	int32_t v = video.bits->getBits(startBits - (hasSign ? 1 : 0));
	if (v && hasSign) {
		int sign = -((int)video.bits->getBit());
		v = (v ^ sign) - sign;
	}

	*dest++ = v;
	length--;

	for (uint32_t i = 0; i < length; i += 8) {
		uint32_t length2 = MIN<uint32_t>(length - i, 8);

		byte bSize = video.bits->getBits(4);

		if (bSize) {

			for (uint32_t j = 0; j < length2; j++) {
				int16_t v2 = video.bits->getBits(bSize);
				if (v2) {
					int sign = -((int)video.bits->getBit());
					v2 = (v2 ^ sign) - sign;
				}

				v += v2;
				*dest++ = v;

				if ((v < -32768) || (v > 32767))
					throw Common::Exception("DC value went out of bounds: %d", v);
			}

		} else
			for (uint32_t j = 0; j < length2; j++)
				*dest++ = v;
	}

	bundle.curDec = reinterpret_cast<byte *>(dest);
}

/* WORKAROUND: This fixes the NWN2 WotC logo.
 * [cf. ffmpeg 47b71eea099b3fe2c7e16644878ad9b7067974e3] */
static inline int16_t dequant(int16_t in, uint32_t quant, bool dc) {
	/* Note: multiplication is unsigned but we want signed shift
	 * otherwise clipping breaks.
	 *
	 * TODO: The official decoder does not use clipping at all
	 * but instead uses the full 32-bit result.
	 * However clipping at least gets rid of the case that a
	 * half-black half-white intra block gets black and white swapped
	 * and should cause at most minor differences (except for DC).
	 */

	int32_t res = ((int32_t) (in * quant)) >> 11;
	if (!dc)
		res = CLIP(res, -32768, 32767);

	return res;
}

/** Reads 8x8 block of DCT coefficients. */
void Bink::BinkVideoTrack::readDCTCoeffs(VideoFrame &video, int16_t *block, bool isIntra) {
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
	for (int mask = 1 << (MAX<int>(bits, 0)); bits >= 0; mask >>= 1, bits--) {
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
				XOREOS_FALLTHROUGH;
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

							int sign = -((int)video.bits->getBit());
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

					int sign = -((int)video.bits->getBit());
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

	uint8_t quantIdx = video.bits->getBits(4);
	const uint32_t *quant = isIntra ? binkIntraQuant[quantIdx] : binkInterQuant[quantIdx];
	block[0] = dequant(block[0], quant[0], true);

	for (int i = 0; i < coefCount; i++) {
		int idx = coefIdx[i];
		block[binkScan[idx]] = dequant(block[binkScan[idx]], quant[idx], false);
	}

}

/** Reads 8x8 block with residue after motion compensation. */
void Bink::BinkVideoTrack::readResidue(VideoFrame &video, int16_t *block, int masksCount) {
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
				XOREOS_FALLTHROUGH;
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

						int sign = -((int)video.bits->getBit());
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

				int sign = -((int)video.bits->getBit());
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

Bink::BinkAudioTrack::BinkAudioTrack(size_t index, Bink::AudioInfo &audio) :
	_index(index),
	_info(audio),
	_audioStream(Sound::makePacketizedPCMStream(_info.outSampleRate, Sound::FLAG_16BITS | Sound::FLAG_NATIVE_ENDIAN, _info.outChannels)),
	_curFrame(0),
	_audioBuffered(0, _info.sampleRate) {
}

Bink::BinkAudioTrack::~BinkAudioTrack() {
	delete _audioStream;
}

Sound::AudioStream *Bink::BinkAudioTrack::getAudioStream() const {
	return _audioStream;
}

void Bink::BinkAudioTrack::decodeAudio(Common::SeekableReadStream& bink, const std::vector<VideoFrame>& frames, const std::vector<AudioInfo>& audioTracks, const Common::Timestamp& endTime) {
	while (_audioBuffered < endTime && _curFrame < frames.size()) {
		const VideoFrame &frame = frames[_curFrame++];
		bink.seek(frame.offset);
		uint32_t frameSize = frame.size;

		uint32_t audioPacketLength = 0;

		for (size_t i = 0; i < audioTracks.size(); i++) {
			audioPacketLength = bink.readUint32LE();
			frameSize -= 4;

			if (audioPacketLength > frameSize)
				throw Common::Exception("Audio packet too big for the frame");

			frameSize -= audioPacketLength;

			if (i != _index) {
				bink.skip(audioPacketLength);
				continue;
			}

			// We found what we were looking for
			break;
		}

		if (audioPacketLength < 4)
			continue;

		// Number of samples in bytes
		uint32_t sampleCount = bink.readUint32LE() / (2 * _info.channels);

		// Create a substream for these bits
		Common::BitStream32LELSB bits(new Common::SeekableSubReadStream(&bink, bink.pos(), bink.pos() + audioPacketLength - 4), true);

		int outSize = _info.frameLen * _info.channels;

		while (bits.pos() < bits.size()) {
			std::unique_ptr<int16_t[]> out = std::make_unique<int16_t[]>(outSize);
			memset(out.get(), 0, outSize * 2);

			audioBlock(bits, out.get());

			_audioStream->queuePacket(new Common::MemoryReadStream(reinterpret_cast<byte *>(out.release()), _info.blockSize * 2, true));

			if (bits.pos() & 0x1F) // next data block starts at a 32-byte boundary
				bits.skip(32 - (bits.pos() & 0x1F));
		}

		_audioBuffered = _audioBuffered.addFrames(sampleCount);
	}

	// If we have reached the end, mark us as finished
	if (_curFrame >= frames.size())
		_audioStream->finish();
}

float Bink::BinkAudioTrack::getFloat(Common::BitStream &bits) {
	int power = bits.getBits(5);

	float f = ldexpf(bits.getBits(23), power - 23);

	if (bits.getBit())
		f = -f;

	return f;
}

bool Bink::BinkAudioTrack::canBufferData() const {
	return !_audioStream->isFinished();
}

void Bink::BinkAudioTrack::audioBlock(Common::BitStream &bits, int16_t *out) {
	if      (_info.codec == kAudioCodecDCT)
		audioBlockDCT (bits);
	else if (_info.codec == kAudioCodecRDFT)
		audioBlockRDFT(bits);

	Sound::floatToInt16Interleave(out, const_cast<const float **>(_info.coeffsPtr),
	                              _info.frameLen, _info.channels);

	if (!_info.first) {
		int count = _info.overlapLen * _info.channels;
		int shift = Common::intLog2(count);
		for (int i = 0; i < count; i++) {
			out[i] = (_info.prevCoeffs[i] * (count - i) + out[i] * i) >> shift;
		}
	}

	std::memcpy(_info.prevCoeffs, out + _info.blockSize, _info.overlapLen * _info.channels * sizeof(*out));

	_info.first = false;
}

void Bink::BinkAudioTrack::audioBlockDCT(Common::BitStream &bits) {
	bits.skip(2);

	for (uint8_t i = 0; i < _info.channels; i++) {
		float *coeffs = _info.coeffsPtr[i];

		readAudioCoeffs(bits, coeffs);

		coeffs[0] /= 0.5f;

		_info.dct->calc(coeffs);

		for (uint32_t j = 0; j < _info.frameLen; j++)
			coeffs[j] *= (_info.frameLen / 2.0f);
	}

}

void Bink::BinkAudioTrack::audioBlockRDFT(Common::BitStream &bits) {
	for (uint8_t i = 0; i < _info.channels; i++) {
		float *coeffs = _info.coeffsPtr[i];

		readAudioCoeffs(bits, coeffs);

		_info.rdft->calc(coeffs);
	}
}

static const uint8_t rleLengthTab[16] = {
	2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 32, 64
};

void Bink::BinkAudioTrack::readAudioCoeffs(Common::BitStream &bits, float *coeffs) {
	coeffs[0] = getFloat(bits) * _info.root;
	coeffs[1] = getFloat(bits) * _info.root;

	float quant[25];

	for (uint32_t i = 0; i < _info.bandCount; i++) {
		int value = bits.getBits(8);

		//                               0.066399999 / log10(M_E)
		quant[i] = expf(MIN(value, 95) * 0.15289164787221953823f) * _info.root;
	}

	float q = 0.0f;

	// Find band (k)
	int k;
	for (k = 0; _info.bands[k] < 1; k++)
		q = quant[k];

	// Parse coefficients
	uint32_t i = 2;
	while (i < _info.frameLen) {

		uint32_t j = 0;
		if (bits.getBit())
			j = i + rleLengthTab[bits.getBits(4)] * 8;
		else
			j = i + 8;

		j = MIN(j, _info.frameLen);

		int width = bits.getBits(4);
		if (width == 0) {

			std::memset(coeffs + i, 0, (j - i) * sizeof(*coeffs));
			i = j;
			while (_info.bands[k] * 2 < i)
				q = quant[k++];

		} else {

			while (i < j) {
				if (_info.bands[k] * 2 == i)
					q = quant[k++];

				int coeff = bits.getBits(width);
				if (coeff) {

					if (bits.getBit())
						coeffs[i] = -q * coeff;
					else
						coeffs[i] =  q * coeff;

				} else {
					coeffs[i] = 0.0f;
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

static inline void IDCTCol(int16_t *dest, const int16_t *src)
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

void Bink::BinkVideoTrack::IDCT(int16_t *block) {
	int i;
	int16_t temp[64];

	for (i = 0; i < 8; i++)
		IDCTCol(&temp[i], &block[i]);
	for (i = 0; i < 8; i++) {
		IDCT_ROW( (&block[8*i]), (&temp[8*i]) );
	}
}

void Bink::BinkVideoTrack::IDCTAdd(DecodeContext &ctx, int16_t *block) {
	int i, j;

	IDCT(block);
	byte *dest = ctx.dest;
	for (i = 0; i < 8; i++, dest += ctx.pitch, block += 8)
		for (j = 0; j < 8; j++)
			 dest[j] += block[j];
}

void Bink::BinkVideoTrack::IDCTPut(DecodeContext &ctx, int16_t *block) {
	int i;
	int16_t temp[64];
	for (i = 0; i < 8; i++)
		IDCTCol(&temp[i], &block[i]);
	for (i = 0; i < 8; i++) {
		IDCT_ROW( (&ctx.dest[i*ctx.pitch]), (&temp[8*i]) );
	}
}

Bink::BinkVideoTrack::BinkVideoTrack(uint32_t width, uint32_t height, uint32_t frameCount, const Common::Rational &frameRate, bool swapPlanes, bool hasAlpha, uint32_t id) :
		_width(width), _height(height), _curFrame(-1), _frameCount(frameCount), _frameRate(frameRate), _swapPlanes(swapPlanes), _hasAlpha(hasAlpha), _id(id) {
	// Give the planes a bit extra space
	width  = _width  + 32;
	height = _height + 32;

	_curPlanes[0] = std::make_unique<byte[]>( width       *  height      ); // Y
	_curPlanes[1] = std::make_unique<byte[]>((width >> 1) * (height >> 1)); // U, 1/4 resolution
	_curPlanes[2] = std::make_unique<byte[]>((width >> 1) * (height >> 1)); // V, 1/4 resolution
	_curPlanes[3] = std::make_unique<byte[]>( width       *  height      ); // A
	_oldPlanes[0] = std::make_unique<byte[]>( width       *  height      ); // Y
	_oldPlanes[1] = std::make_unique<byte[]>((width >> 1) * (height >> 1)); // U, 1/4 resolution
	_oldPlanes[2] = std::make_unique<byte[]>((width >> 1) * (height >> 1)); // V, 1/4 resolution
	_oldPlanes[3] = std::make_unique<byte[]>( width       *  height      ); // A

	// Initialize the video with solid green
	std::memset(_curPlanes[0].get(),   0,  width       *  height      );
	std::memset(_curPlanes[1].get(),   0, (width >> 1) * (height >> 1));
	std::memset(_curPlanes[2].get(),   0, (width >> 1) * (height >> 1));
	std::memset(_curPlanes[3].get(), 255,  width       *  height      );
	std::memset(_oldPlanes[0].get(),   0,  width       *  height      );
	std::memset(_oldPlanes[1].get(),   0, (width >> 1) * (height >> 1));
	std::memset(_oldPlanes[2].get(),   0, (width >> 1) * (height >> 1));
	std::memset(_oldPlanes[3].get(), 255,  width       *  height      );

	initBundles();
	initHuffman();
}

} // End of namespace Video
