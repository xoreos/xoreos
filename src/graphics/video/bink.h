/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Based quite heavily on the Bink decoder found in FFmpeg.
// Many thanks to Kostya Shishkov for doing the hard work.

/** @file graphics/video/bink.h
 *  Decoding RAD Game Tools' Bink videos.
 */

#ifndef GRAPHICS_VIDEO_BINK_H
#define GRAPHICS_VIDEO_BINK_H

#include <vector>

#include "common/types.h"
#include "graphics/video/decoder.h"

namespace Common {
	class SeekableReadStream;
	class BitStream;
	class Huffman;
}

namespace Graphics {

/** A generic interface for video decoders. */
class Bink : public VideoDecoder {
public:
	Bink(Common::SeekableReadStream *bink);
	~Bink();

	bool gotTime() const;

protected:
	void processData();

private:
	/** IDs for different data types used in Bink video codec. */
	enum Source {
		kSourceBlockTypes    = 0, ///< 8x8 block types.
		kSourceSubBlockTypes    , ///< 16x16 block types (a subset of 8x8 block types).
		kSourceColors           , ///< Pixel values used for different block types.
		kSourcePattern          , ///< 8-bit values for 2-colour pattern fill.
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
		kBlockRun        ,  ///< Block is composed from runs of colours with custom scan order.
		kBlockResidue    ,  ///< Motion block with some difference added.
		kBlockIntra      ,  ///< Intra DCT block.
		kBlockFill       ,  ///< Block is filled with single colour.
		kBlockInter      ,  ///< Motion block with DCT applied to the difference.
		kBlockPattern    ,  ///< Block is filled with two colours following custom pattern.
		kBlockRaw        ,  ///< Uncoded 8x8 block.
	};

	/** Data structure for decoding and tranlating Huffman'd data. */
	struct Huffman {
		int  index;       ///< Index of the Huffman codebook to use.
		byte symbols[16]; ///< Huffman symbol => Bink symbol tranlation list.
	};

	/** Data structure used for decoding a single Bink data type. */
	struct Bundle {
		int countLengths[2]; ///< Lengths of number of entries to decode (in bits).
		int countLength;     ///< Length of number of entries to decode (in bits) for the current plane.

		Huffman huffman; ///< Huffman codebook.

		byte *data;    ///< Buffer for decoded symbols.
		byte *dataEnd; ///< Buffer end.

		byte *curDec; ///< Pointer to the data that wasn't yet decoded.
		byte *curPtr; ///< Pointer to the data that wasn't yet read.
	};

	/** An audio track. */
	struct AudioTrack {
		uint16 sampleRate;
		uint16 flags;

		uint32 sampleCount;

		Common::BitStream *bits;
	};

	/** A video frame. */
	struct VideoFrame {
		bool keyFrame;

		uint32 offset;
		uint32 size;

		Common::BitStream *bits;
	};

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

	Common::SeekableReadStream *_bink;

	uint32 _id; ///< The BIK FourCC.

	uint32 _fpsNum; ///< Dividend of the FPS value.
	uint32 _fpsDen; ///< Dividor of the FPS value.

	uint32 _videoFlags; ///< Video frame features.

	bool _hasAlpha;   ///< Do video frames have alpha?
	bool _swapPlanes; ///< Are the planes ordered (A)YVU instead of (A)YUV?

	uint32 _curFrame; ///< Current Frame.

	uint32 _startTime;     ///< Timestamp of when the video was started.
	uint32 _lastFrameTime; ///< Timestamp of when the last frame was decoded.

	std::vector<AudioTrack> _audioTracks; ///< All audio tracks.
	std::vector<VideoFrame> _frames;      ///< All video frames.

	Common::Huffman *_huffman[16]; ///< The 16 Huffman codebooks used in Bink decoding.

	Bundle _bundles[kSourceMAX]; ///< Bundles for decoding all data types.

	/** Huffman codebooks to use for decoding high nibbles in color data types. */
	Huffman _colHighHuffman[16];
	/** Value of the last decoded high nibble in color data types. */
	int _colLastVal;

	byte *_curPlanes[4]; ///< The 4 color planes, YUVA, current frame.
	byte *_oldPlanes[4]; ///< The 4 color planes, YUVA, last frame.

	/** Load a Bink file. */
	void load();

	/** Initialize the bundles. */
	void initBundles();
	/** Deinitialize the bundles. */
	void deinitBundles();

	/** Initialize the Huffman decoders. */
	void initHuffman();

	/** Convert the YUVA420p data we get to BGRA. */
	void yuva2bgra();

	/** Decode an audio packet. */
	void audioPacket(AudioTrack &audio);
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
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_BINK_H
