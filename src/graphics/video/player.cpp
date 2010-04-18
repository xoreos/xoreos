/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/player.cpp
 *  A video player.
 */

#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/player.h"
#include "graphics/video/decoder.h"
#include "graphics/video/fader.h"

#include "events/events.h"
#include "events/requests.h"

#include "aurora/resman.h"

namespace Graphics {

VideoPlayer::VideoPlayer(const std::string &video) : _video(0) {
	load(video);
}

VideoPlayer::~VideoPlayer() {
	delete _video;
}

void VideoPlayer::load(const std::string &name) {
	_video = new Fader(320, 240, 2);
	return;

	Aurora::FileType type;
	Common::SeekableReadStream *video = ResMan.getImage(name, &type);
	if (!video)
		throw Common::Exception("No such video resource \"%s\"", name.c_str());

	// Loading the different image formats
	if      (type == Aurora::kFileTypeBIK)
		throw Common::Exception("TODO: BIK");
	else
		throw Common::Exception("Unsupported video resource type %d", (int) type);
}

void VideoPlayer::play() {
	RequestMan.sync();

	bool brk = false;

	Events::Event event;
	while (!EventMan.quitRequested()) {
		_video->update();

		while (EventMan.pollEvent(event)) {
			if ((event.type == Events::kEventKeyDown) && (event.key.keysym.sym == SDLK_ESCAPE))
				brk = true;
		}

		if (brk || !_video->isPlaying())
			break;

		if (_video->gotTime())
			EventMan.delay(10);
	}

	_video->abort();
}

} // End of namespace Graphics
