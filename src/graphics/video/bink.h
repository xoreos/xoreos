/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

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

	/** Data structure used for decoding a single Bink data type. */
	struct Bundle {
		int huffman; ///< Index of the Huffman codebook to use.

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

	Common::SeekableReadStream *_bink;

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

	Common::Huffman *_huffman[16]; ///< The 16 Huffman codebooks used in Bink decoding.

	Bundle _bundles[kSourceMAX]; ///< Bundles for decoding all data types.

	/** Indices of the Huffman codebooks to use for decoding high nibbles in color data types. */
	int _colHighHuffman[16];
	/** Value of the last decoded high nibble in color data types. */
	int _colLastVal;

	void load();

	void initBundles();
	void deinitBundles();

	void initHuffman();

	/** Decode an audio packet. */
	void audioPacket(AudioTrack &audio);
	/** Decode a video packet. */
	void videoPacket(VideoFrame &video);

	void decodePlane(VideoFrame &video, int planeIdx, bool isChroma);

	void readBundle(VideoFrame &video, int bundle);

	void readHuffman(VideoFrame &video, int &huffman);
	void mergeHuffmanSymbols(VideoFrame &video, uint32 *dst, uint32 *src, int size);
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_BINK_H
