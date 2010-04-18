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
	struct AudioTrack {
		uint16 sampleRate;
		uint16 flags;
	};

	struct VideoFrame {
		bool keyFrame;
		uint32 offset;
	};

	Common::SeekableReadStream *_bik;

	uint32 _id;

	uint32 _fpsNum;
	uint32 _fpsDen;

	uint32 _videoFlags;

	uint32 _curFrame;

	uint32 _startTime;
	uint32 _lastFrameTime;

	std::vector<AudioTrack> _audioTracks;
	std::vector<VideoFrame> _frames;

	void load();
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_BIK_H
