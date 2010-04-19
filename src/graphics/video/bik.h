/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/bik.h
 *  Decoding RAD Game Tools' Bink videos.
 */

#ifndef GRAPHICS_VIDEO_BIK_H
#define GRAPHICS_VIDEO_BIK_H

#include <vector>

#include "graphics/video/decoder.h"

#define ALT_BITSTREAM_READER_LE
#include "graphics/video/get_bits.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** A generic interface for video decoders. */
class BIK : public VideoDecoder {
public:
	BIK(Common::SeekableReadStream *bik);
	~BIK();

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
		kSourceRun              , ///< Run lengths for special fill block:

		kSourceMAX
	};

	/** Bink video block types. */
	enum BlockTypes {
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

	/** Data needed to decode 4-bit Huffman-coded value. */
	struct Tree {
		int   vlcNum;   ///< Tree number (in bink_trees[]).
		uint8 syms[16]; ///< Leaf value to symbol mapping.
	};

	/** Data structure used for decoding single Bink data type. */
	struct Bundle {
		uint32 length;  ///< Length of number of entries to decode (in bits).
		Tree   tree;    ///< Huffman tree-related data
		byte  *data;    ///< Buffer for decoded symbols.
		byte  *dataEnd; ///< Buffer end.
		byte  *curDec;  ///< Pointer to the not yet decoded part of the buffer.
		byte  *curPtr;  ///< Pointer to the data that is not read from buffer yet.
	};

	/** An audio track. */
	struct AudioTrack {
		uint16 sampleRate;
		uint16 flags;

		byte  *data;
		uint32 dataSize;
	};

	/** A video frame. */
	struct VideoFrame {
		bool keyFrame;

		uint32 offset;
		uint32 size;

		byte  *data;
		uint32 dataSize;
	};

	Common::SeekableReadStream *_bik;

	uint32 _id; ///< The BIK FourCC.

	uint32 _fpsNum; ///< Dividend of the FPS value.
	uint32 _fpsDen; ///< Dividor of the FPS value.

	uint32 _videoFlags; ///< Video frame features.

	bool _hasAlpha;   ///< Do video frames have alpha?
	bool _swapPlanes;

	uint32 _curFrame; ///< Current Frame.

	uint32 _startTime;     ///< Timestamp of when the video was started.
	uint32 _lastFrameTime; ///< Timestamp of when the last frame was decoded.

	std::vector<AudioTrack> _audioTracks; ///< All audio tracks.
	std::vector<VideoFrame> _frames;      ///< All video frames.

	Bundle _bundles[kSourceMAX]; ///< Bundles for decoding all data types.

	VLC _trees[16];
	VLC_TYPE _table[16 * 128][2];

	void load();

	/** Decode an audio packet. */
	void audioPacket(AudioTrack &audio);
	/** Decode a video packet. */
	void videoPacket(VideoFrame &video);

	void initTrees();

	/** Allocates memory for bundles. */
	void initBundles();
	/** Frees memory used by bundles. */
	void freeBundles();

	/** Initializes length in all bundles.
	 *
	 *  @param c Decoder context.
	 *  @param width Plane width.
	 *  @param bw Plane width in 8x8 blocks.
	 */
	void initLengths(uint32 width, uint32 bw);
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_BIK_H
