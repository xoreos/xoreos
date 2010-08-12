/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/util.cpp
 *  Generic Aurora engines utility functions.
 */

#include "engines/util.h"

#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"

#include "events/events.h"

#include "sound/sound.h"

#include "graphics/aurora/videoplayer.h"

#include "aurora/resman.h"

namespace Engines {

void playVideo(const Common::UString &video) {
	try {
		Graphics::Aurora::VideoPlayer videoPlayer(video);

		videoPlayer.play();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType, bool loop) {
	Aurora::ResourceType resType =
		(soundType == Sound::kSoundTypeMusic) ? Aurora::kResourceMusic : Aurora::kResourceSound;

	Sound::ChannelHandle channel;

	try {
		Common::SeekableReadStream *soundStream = ResMan.getResource(resType, sound);
		if (!soundStream)
			return channel;

		channel = SoundMan.playSoundFile(soundStream, soundType, loop);

		SoundMan.startChannel(channel);

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}

	return channel;
}

bool longDelay(uint32 ms) {
	while ((ms > 0) && !EventMan.quitRequested()) {
		uint32 delay = MIN<uint32>(ms, 10);

		EventMan.delay(delay);

		ms -= delay;
	}

	return EventMan.quitRequested();
}

bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName) {
	Common::DumpFile file;
	if (!file.open(fileName))
		return false;

	uint32 pos = stream.pos();

	stream.seek(0);

	file.writeStream(stream);
	file.flush();

	bool error = file.err();

	stream.seek(pos);
	file.close();

	return !error;
}

} // End of namespace Engines
