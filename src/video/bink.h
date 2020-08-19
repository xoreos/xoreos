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
#include <memory>

#include "src/common/types.h"
#include "src/common/rational.h"

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

protected:
	void decodeNextTrackFrame(VideoTrack &track);
	void checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime);

private:
	static const int kAudioChannelsMax  = 2;
	static const int kAudioBlockSizeMax = (kAudioChannelsMax << 11);

	enum AudioCodec {
		kAudioCodecDCT,
		kAudioCodecRDFT
	};

	/** An audio track. */
	struct AudioInfo {
		uint16_t flags;

		uint32_t sampleRate;
		uint8_t  channels;

		uint32_t outSampleRate;
		uint8_t  outChannels;

		AudioCodec codec;

		bool first;

		uint32_t frameLen;
		uint32_t overlapLen;

		uint32_t blockSize;

		uint32_t  bandCount;
		uint32_t *bands;

		float root;

		float coeffs[16 * kAudioBlockSizeMax];
		int16_t prevCoeffs[kAudioBlockSizeMax];

		float *coeffsPtr[kAudioChannelsMax];

		Common::RDFT *rdft;
		Common::DCT  *dct;

		AudioInfo();
		~AudioInfo();
	};

	/** A video frame. */
	struct VideoFrame {
		bool keyFrame;

		uint32_t offset;
		uint32_t size;

		Common::BitStream *bits;

		VideoFrame();
		~VideoFrame();
	};

	std::unique_ptr<Common::SeekableReadStream> _bink;

	std::vector<AudioInfo> _audioTracks; ///< All audio tracks.
	std::vector<VideoFrame> _frames;      ///< All video frames.

	uint32_t _audioTrack; ///< Audio track to use.

	/** Load a Bink file. */
	void load();

	class BinkVideoTrack : public FixedRateVideoTrack {
	public:
		BinkVideoTrack(uint32_t width, uint32_t height, uint32_t frameCount, const Common::Rational &frameRate, bool swapPlanes, bool hasAlpha, uint32_t id);

		uint32_t getWidth() const { return _width; }
		uint32_t getHeight() const { return _height; }
		int getCurFrame() const { return _curFrame; }
		int getFrameCount() const { return _frameCount; }

		/** Decode a video packet. */
		void decodePacket(Graphics::Surface &surface, VideoFrame &frame);

	protected:
		Common::Rational getFrameRate() const { return _frameRate; }

	private:
		/** A decoder state. */
		struct DecodeContext {
			VideoFrame *video;

			uint32_t planeIdx;

			uint32_t blockX;
			uint32_t blockY;

			byte *dest;
			byte *prev;

			byte *destStart, *destEnd;
			byte *prevStart, *prevEnd;

			uint32_t pitch;

			int coordMap[64];
			int coordScaledMap1[64];
			int coordScaledMap2[64];
			int coordScaledMap3[64];
			int coordScaledMap4[64];
		};

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

			std::unique_ptr<byte[]> data; ///< Buffer for decoded symbols.

			byte *dataEnd; ///< Pointer to the data end end.
			byte *curDec;  ///< Pointer to the data that wasn't yet decoded.
			byte *curPtr;  ///< Pointer to the data that wasn't yet read.

			Bundle();
		};

		uint32_t _width;
		uint32_t _height;

		int _curFrame; ///< Current Frame.
		int _frameCount;

		Common::Rational _frameRate; ///< The frame rate of the video.

		bool _swapPlanes; ///< Are the planes ordered (A)YVU instead of (A)YUV?
		bool _hasAlpha;   ///< Do video frames have alpha?

		uint32_t _id; ///< The BIK FourCC.

		Bundle _bundles[kSourceMAX]; ///< Bundles for decoding all data types.

		std::unique_ptr<Common::Huffman> _huffman[16]; ///< The 16 Huffman codebooks used in Bink decoding.

		/** Huffman codebooks to use for decoding high nibbles in color data types. */
		Huffman _colHighHuffman[16];
		/** Value of the last decoded high nibble in color data types. */
		int _colLastVal;

		std::unique_ptr<byte[]> _curPlanes[4]; ///< The 4 color planes, YUVA, current frame.
		std::unique_ptr<byte[]> _oldPlanes[4]; ///< The 4 color planes, YUVA, last frame.

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
		int32_t getBundleValue(Source source);
		/** Read a count value out of a bundle. */
		uint32_t readBundleCount(VideoFrame &video, Bundle &bundle);

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
		void readDCTCoeffs   (VideoFrame &video, int16_t *block, bool isIntra);
		void readResidue     (VideoFrame &video, int16_t *block, int masksCount);

		// Bink video IDCT
		void IDCT(int16_t *block);
		void IDCTPut(DecodeContext &ctx, int16_t *block);
		void IDCTAdd(DecodeContext &ctx, int16_t *block);
	};

	class BinkAudioTrack : public AudioTrack {
	public:
		BinkAudioTrack(size_t index, AudioInfo &audio);
		~BinkAudioTrack();

		bool canBufferData() const;

		/** Decode audio data up to endTime. */
		void decodeAudio(Common::SeekableReadStream& bink, const std::vector<VideoFrame>& frames, const std::vector<AudioInfo>& audioTracks, const Common::Timestamp& endTime);

	protected:
		Sound::AudioStream *getAudioStream() const;

	private:
		size_t _index;
		AudioInfo &_info;
		Sound::PacketizedAudioStream *_audioStream;
		uint32_t _curFrame;
		Common::Timestamp _audioBuffered;

		float getFloat(Common::BitStream &bits);

		/** Decode an audio block. */
		void audioBlock(Common::BitStream &bits, int16_t *out);
		/** Decode a DCT'd audio block. */
		void audioBlockDCT(Common::BitStream &bits);
		/** Decode a RDFT'd audio block. */
		void audioBlockRDFT(Common::BitStream &bits);

		void readAudioCoeffs(Common::BitStream &bits, float *coeffs);

		static void floatToInt16Interleave(int16_t *dst, const float **src, uint32_t length, uint8_t channels);
	};

	void initAudioTrack(AudioInfo &audio);
};

} // End of namespace Video

#endif // VIDEO_BINK_H
