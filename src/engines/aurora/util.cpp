/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/util.cpp
 *  Generic Aurora engines utility functions.
 */

#include "engines/aurora/util.h"

#include "common/error.h"
#include "common/stream.h"
#include "common/file.h"
#include "common/configman.h"

#include "events/events.h"

#include "sound/sound.h"

#include "graphics/aurora/videoplayer.h"
#include "graphics/aurora/texture.h"

#include "../../aurora/util.h"
#include "../../aurora/resman.h"
#include "../../aurora/gfffile.h"

namespace Engines {

void playVideo(const Common::UString &video) {
	if (ConfigMan.getBool("skipvideos", false))
		return;

	// Mute other sound sources
	SoundMan.setTypeGain(Sound::kSoundTypeMusic, 0.0);
	SoundMan.setTypeGain(Sound::kSoundTypeSFX  , 0.0);
	SoundMan.setTypeGain(Sound::kSoundTypeVoice, 0.0);

	try {
		Graphics::Aurora::VideoPlayer videoPlayer(video);

		videoPlayer.play();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}

	// Restore volumes
	SoundMan.setTypeGain(Sound::kSoundTypeMusic, ConfigMan.getDouble("volume_music", 1.0));
	SoundMan.setTypeGain(Sound::kSoundTypeSFX  , ConfigMan.getDouble("volume_sfx"  , 1.0));
	SoundMan.setTypeGain(Sound::kSoundTypeVoice, ConfigMan.getDouble("volume_voice", 1.0));
}

Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType,
		bool loop, float volume) {

	Aurora::ResourceType resType =
		(soundType == Sound::kSoundTypeMusic) ? Aurora::kResourceMusic : Aurora::kResourceSound;

	Sound::ChannelHandle channel;

	try {
		Common::SeekableReadStream *soundStream = ResMan.getResource(resType, sound);
		if (!soundStream)
			return channel;

		channel = SoundMan.playSoundFile(soundStream, soundType, loop);

		SoundMan.setChannelGain(channel, volume);
		SoundMan.startChannel(channel);

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}

	return channel;
}

void checkConfigInt(const Common::UString &key, int min, int max, int def) {
	int value = ConfigMan.getInt(key, def);
	if ((value >= min) && (value <= max))
		return;

	warning("Config \"%s\" has invalid value (%d), resetting to default (%d)",
			key.c_str(), value, def);
	ConfigMan.setInt(key, def);
}

void checkConfigDouble(const Common::UString &key, double min, double max, double def) {
	double value = ConfigMan.getDouble(key, def);
	if ((value >= min) && (value <= max))
		return;

	warning("Config \"%s\" has invalid value (%lf), resetting to default (%lf)",
			key.c_str(), value, def);
	ConfigMan.setDouble(key, def);
}

bool longDelay(uint32 ms) {
	while ((ms > 0) && !EventMan.quitRequested()) {
		uint32 delay = MIN<uint32>(ms, 10);

		EventMan.delay(delay);

		ms -= delay;
	}

	return EventMan.quitRequested();
}

void loadGFF(Aurora::GFFFile &gff, Common::SeekableReadStream &stream, uint32 id) {
	gff.load(stream);

	if (gff.getID() != id)
		throw Common::Exception("GFF has invalid ID (want 0x%08X, got 0x%08X)", id, gff.getID());
}

void loadGFF(Aurora::GFFFile &gff, const Common::UString &name, Aurora::FileType type, uint32 id) {
	Common::SeekableReadStream *gffFile = 0;
	try {
		if (!(gffFile = ResMan.getResource(name, type)))
			throw Common::Exception("No such resource \"%s\"", Aurora::setFileType(name, type).c_str());

		loadGFF(gff, *gffFile, id);

		delete gffFile;
	} catch (...) {
		delete gffFile;
		throw;
	}
}

Aurora::GFFFile *loadGFF(const Common::UString &name, Aurora::FileType type, uint32 id) {
	Aurora::GFFFile *gff = new Aurora::GFFFile;

	try {

		loadGFF(*gff, name, type, id);

	} catch (...) {
		delete gff;
		throw;
	}

	return gff;
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

bool dumpResource(const Common::UString &name, Aurora::FileType type, Common::UString file) {
	Common::SeekableReadStream *res = ResMan.getResource(name, type);
	if (!res)
		return false;

	if (file.empty())
		file = Aurora::setFileType(name, type);

	bool success = dumpStream(*res, file);

	delete res;

	return success;
}

bool dumpResource(const Common::UString &name, const Common::UString &file) {
	Aurora::FileType type = Aurora::getFileType(name);

	return dumpResource(Aurora::setFileType(name, Aurora::kFileTypeNone), type, file);
}

bool dumpTGA(const Common::UString &name) {
	try {

		Graphics::Aurora::Texture texture(name);

		return texture.dumpTGA(name + ".tga");

	} catch (...) {
	}

	return false;
}

} // End of namespace Engines
