/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/videoplayer.cpp
 *  A video player.
 */

#include "common/error.h"
#include "common/util.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "graphics/video/decoder.h"
#include "graphics/video/bink.h"
#include "graphics/video/quicktime.h"

#include "graphics/aurora/videoplayer.h"

#include "events/events.h"
#include "events/requests.h"

#include "aurora/resman.h"

namespace Graphics {

namespace Aurora {

VideoPlayer::VideoPlayer(const Common::UString &video) : _video(0) {
	load(video);
}

VideoPlayer::~VideoPlayer() {
	delete _video;
}

void VideoPlayer::load(const Common::UString &name) {
	delete _video;
	_video = 0;

	::Aurora::FileType type;
	Common::SeekableReadStream *video = ResMan.getResource(::Aurora::kResourceVideo, name, &type);
	if (!video)
		throw Common::Exception("No such video resource \"%s\"", name.c_str());

	// Loading the different image formats
	if      (type == ::Aurora::kFileTypeBIK)
		_video = new Bink(video);
	else if (type == ::Aurora::kFileTypeMOV)
		_video = new QuickTimeDecoder(video);
	else if (type == ::Aurora::kFileTypeXMV) {
		delete video;
		throw Common::Exception("XMV video not supported");
	} else {
		delete video;
		throw Common::Exception("Unsupported video resource type %d", (int) type);
	}

	_video->setScale(VideoDecoder::kScaleUpDown);
}

void VideoPlayer::play() {
	RequestMan.sync();

	bool brk = false;

	try {
		Events::Event event;
		while (!EventMan.quitRequested()) {
			_video->update();

			while (EventMan.pollEvent(event)) {
				if ((event.type == Events::kEventKeyDown && event.key.keysym.sym == SDLK_ESCAPE) || event.type == Events::kEventMouseUp)
					brk = true;
			}

			if (brk || !_video->isPlaying())
				break;

			if (_video->hasTime())
				EventMan.delay(10);
		}
	} catch (...) {
		_video->abort();
		throw;
	}

	_video->abort();
}

} // End of namespace Aurora

} // End of namespace Graphics
