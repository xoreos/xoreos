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

namespace Common {
	class SeekableReadStream;
	class BitStream;
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

	void load();

	/** Decode an audio packet. */
	void audioPacket(AudioTrack &audio);
	/** Decode a video packet. */
	void videoPacket(VideoFrame &video);
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_BIK_H
