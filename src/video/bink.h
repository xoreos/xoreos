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

#ifndef VIDEO_BINK_H
#define VIDEO_BINK_H

#include <vector>

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
	class BitStream;
	class Huffman;

	class RDFT;
	class DCT;
}

namespace Sound {
	class PacketizedAudioStream;
}

namespace Video {

/** A decoder for RAD Game Tools' Bink videos. */
class Bink : public VideoDecoder {
public:
	Bink(Common::SeekableReadStream *bink);
	~Bink();

	uint32 getNextFrameStartTime() const;

protected:
	void startVideo();
	void processData();

private:
	static const int kAudioChannelsMax  = 2;
	static const int kAudioBlockSizeMax = (kAudioChannelsMax << 11);

	/** IDs for different data types used in Bink video codec. */
	enum Source {
		kSourceBlockTypes    = 0, ///< 8x8 block types.
		kSourceSubBlockTypes    , ///< 16x16 block types (a subset of 8x8 block types).
		kSourceColors           , ///< Pixel values used for different block types.
		kSourcePattern          , ///< 8-bit values for 2-color pattern fill.
		kSourceXOff             , ///< X components of motion value.
		kSourceYOff             , ///< Y components of motion value.
		kSourceIntraDC          , ///< DC values for intrablocks with DCT.
		kSourceInterDC          , ///< DC values for interblocks with DCT.
		kSourceRun              , ///< Run lengths for special fill block.

		kSourceMAX
	};

	/** Bink video block types. */
	enum BlockType {
		kBlockSkip    = 0,  ///< Skipped block.
		kBlockScaled     ,  ///< Block has size 16x16.
		kBlockMotion     ,  ///< Block is copied from previous frame with some offset.
		kBlockRun        ,  ///< Block is composed from runs of colors with custom scan order.
		kBlockResidue    ,  ///< Motion block with some difference added.
		kBlockIntra      ,  ///< Intra DCT block.
		kBlockFill       ,  ///< Block is filled with single color.
		kBlockInter      ,  ///< Motion block with DCT applied to the difference.
		kBlockPattern    ,  ///< Block is filled with two colors following custom pattern.
		kBlockRaw           ///< Uncoded 8x8 block.
	};

	/** Data structure for decoding and translating Huffman'd data. */
	struct Huffman {
		int  index;       ///< Index of the Huffman codebook to use.
		byte symbols[16]; ///< Huffman symbol => Bink symbol translation list.

		Huffman();
	};

	/** Data structure used for decoding a single Bink data type. */
	struct Bundle {
		int countLengths[2]; ///< Lengths of number of entries to decode (in bits).
		int countLength;     ///< Length of number of entries to decode (in bits) for the current plane.

		Huffman huffman; ///< Huffman codebook.

		Common::ScopedArray<byte> data; ///< Buffer for decoded symbols.

		byte *dataEnd; ///< Pointer to the data end end.
		byte *curDec;  ///< Pointer to the data that wasn't yet decoded.
		byte *curPtr;  ///< Pointer to the data that wasn't yet read.

		Bundle();
	};

	enum AudioCodec {
		kAudioCodecDCT,
		kAudioCodecRDFT
	};

	/** An audio track. */
	struct AudioInfo {
		uint16 flags;

		uint32 sampleRate;
		uint8  channels;

		uint32 outSampleRate;
		uint8  outChannels;

		AudioCodec codec;

		uint32 sampleCount;

		Common::BitStream *bits;

		bool first;

		uint32 frameLen;
		uint32 overlapLen;

		uint32 blockSize;

		uint32  bandCount;
		uint32 *bands;

		float root;

		float coeffs[16 * kAudioBlockSizeMax];
		int16 prevCoeffs[kAudioBlockSizeMax];

		float *coeffsPtr[kAudioChannelsMax];

		Common::RDFT *rdft;
		Common::DCT  *dct;

		AudioInfo();
		~AudioInfo();
	};

	/** A video frame. */
	struct VideoFrame {
		bool keyFrame;

		uint32 offset;
		uint32 size;

		Common::BitStream *bits;

		VideoFrame();
		~VideoFrame();
	};

	/** A decoder state. */
	struct DecodeContext {
		VideoFrame *video;

		uint32 planeIdx;

		uint32 blockX;
		uint32 blockY;

		byte *dest;
		byte *prev;

		byte *destStart, *destEnd;
		byte *prevStart, *prevEnd;

		uint32 pitch;

		int coordMap[64];
		int coordScaledMap1[64];
		int coordScaledMap2[64];
		int coordScaledMap3[64];
		int coordScaledMap4[64];
	};

	Common::ScopedPtr<Common::SeekableReadStream> _bink;

	uint32 _id; ///< The BIK FourCC.

	uint32 _fpsNum; ///< Dividend of the FPS value.
	uint32 _fpsDen; ///< Divisor of the FPS value.

	uint32 _videoFlags; ///< Video frame features.

	bool _hasAlpha;   ///< Do video frames have alpha?
	bool _swapPlanes; ///< Are the planes ordered (A)YVU instead of (A)YUV?

	uint32 _curFrame; ///< Current Frame.

	std::vector<AudioInfo> _audioTracks; ///< All audio tracks.
	std::vector<VideoFrame> _frames;      ///< All video frames.

	uint32 _audioTrack; ///< Audio track to use.

	Common::ScopedPtr<Common::Huffman> _huffman[16]; ///< The 16 Huffman codebooks used in Bink decoding.

	Bundle _bundles[kSourceMAX]; ///< Bundles for decoding all data types.

	/** Huffman codebooks to use for decoding high nibbles in color data types. */
	Huffman _colHighHuffman[16];
	/** Value of the last decoded high nibble in color data types. */
	int _colLastVal;

	Common::ScopedArray<byte> _curPlanes[4]; ///< The 4 color planes, YUVA, current frame.
	Common::ScopedArray<byte> _oldPlanes[4]; ///< The 4 color planes, YUVA, last frame.

	/** Load a Bink file. */
	void load();

	/** Initialize the bundles. */
	void initBundles();

	/** Initialize the Huffman decoders. */
	void initHuffman();

	/** Decode a video packet. */
	void videoPacket(VideoFrame &video);

	/** Decode a plane. */
	void decodePlane(VideoFrame &video, int planeIdx, bool isChroma);

	/** Read/Initialize a bundle for decoding a plane. */
	void readBundle(VideoFrame &video, Source source);

	/** Read the symbols for a Huffman code. */
	void readHuffman(VideoFrame &video, Huffman &huffman);
	/** Merge two Huffman symbol lists. */
	void mergeHuffmanSymbols(VideoFrame &video, byte *dst, const byte *src, int size);

	/** Read and translate a symbol out of a Huffman code. */
	byte getHuffmanSymbol(VideoFrame &video, Huffman &huffman);

	/** Get a direct value out of a bundle. */
	int32 getBundleValue(Source source);
	/** Read a count value out of a bundle. */
	uint32 readBundleCount(VideoFrame &video, Bundle &bundle);

	// Handle the block types
	void blockSkip         (DecodeContext &ctx);
	void blockScaledSkip   (DecodeContext &ctx);
	void blockScaledRun    (DecodeContext &ctx);
	void blockScaledIntra  (DecodeContext &ctx);
	void blockScaledFill   (DecodeContext &ctx);
	void blockScaledPattern(DecodeContext &ctx);
	void blockScaledRaw    (DecodeContext &ctx);
	void blockScaled       (DecodeContext &ctx);
	void blockMotion       (DecodeContext &ctx);
	void blockRun          (DecodeContext &ctx);
	void blockResidue      (DecodeContext &ctx);
	void blockIntra        (DecodeContext &ctx);
	void blockFill         (DecodeContext &ctx);
	void blockInter        (DecodeContext &ctx);
	void blockPattern      (DecodeContext &ctx);
	void blockRaw          (DecodeContext &ctx);

	// Read the bundles
	void readRuns        (VideoFrame &video, Bundle &bundle);
	void readMotionValues(VideoFrame &video, Bundle &bundle);
	void readBlockTypes  (VideoFrame &video, Bundle &bundle);
	void readPatterns    (VideoFrame &video, Bundle &bundle);
	void readColors      (VideoFrame &video, Bundle &bundle);
	void readDCS         (VideoFrame &video, Bundle &bundle, int startBits, bool hasSign);
	void readDCTCoeffs   (VideoFrame &video, int16 *block, bool isIntra);
	void readResidue     (VideoFrame &video, int16 *block, int masksCount);

	// Bink video IDCT
	void IDCT(int16 *block);
	void IDCTPut(DecodeContext &ctx, int16 *block);
	void IDCTAdd(DecodeContext &ctx, int16 *block);

	class BinkAudioTrack : public AudioTrack {
	public:
		BinkAudioTrack(AudioInfo &audio);
		~BinkAudioTrack();

		bool canBufferData() const;

		/** Decode an audio packet. */
		void decodePacket();

		void setEndOfData();

	protected:
		Sound::AudioStream *getAudioStream() const;

	private:
		AudioInfo *_audioInfo;
		Sound::PacketizedAudioStream *_audioStream;

		float getFloat();

		/** Decode an audio block. */
		void audioBlock(int16 *out);
		/** Decode a DCT'd audio block. */
		void audioBlockDCT();
		/** Decode a RDFT'd audio block. */
		void audioBlockRDFT();

		void readAudioCoeffs(float *coeffs);

		static void floatToInt16Interleave(int16 *dst, const float **src, uint32 length, uint8 channels);
	};

	void initAudioTrack(AudioInfo &audio);
};

} // End of namespace Video

#endif // VIDEO_BINK_H
