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
 *  Generic Aurora engines utility functions.
 */

#include <cstdlib>

#include <memory>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/writefile.h"
#include "src/common/configman.h"
#include "src/common/debug.h"
#include "src/common/random.h"

#include "src/aurora/util.h"
#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/images/decoder.h"

#include "src/graphics/aurora/texture.h"

#include "src/sound/sound.h"

#include "src/video/aurora/videoplayer.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"

namespace Engines {

void playVideo(const Common::UString &video) {
	if (ConfigMan.getBool("skipvideos", false)) {
		debugC(Common::kDebugEngineVideo, 1, "Skipping video \"%s\"", video.c_str());
		return;
	}

	// Mute other sound sources
	SoundMan.setTypeGain(Sound::kSoundTypeMusic, 0.0f);
	SoundMan.setTypeGain(Sound::kSoundTypeSFX  , 0.0f);
	SoundMan.setTypeGain(Sound::kSoundTypeVoice, 0.0f);

	try {
		Video::Aurora::VideoPlayer videoPlayer(video);

		debugC(Common::kDebugEngineVideo, 1, "Playing video \"%s\"", video.c_str());

		videoPlayer.play();
	} catch (...) {
		Common::exceptionDispatcherWarning();
	}

	// Restore volumes
	SoundMan.setTypeGain(Sound::kSoundTypeMusic, ConfigMan.getDouble("volume_music", 1.0));
	SoundMan.setTypeGain(Sound::kSoundTypeSFX  , ConfigMan.getDouble("volume_sfx"  , 1.0));
	SoundMan.setTypeGain(Sound::kSoundTypeVoice, ConfigMan.getDouble("volume_voice", 1.0));
}

Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType,
		bool loop, float volume, bool pitchVariance) {

	Aurora::ResourceType resType =
		(soundType == Sound::kSoundTypeMusic) ? Aurora::kResourceMusic : Aurora::kResourceSound;

	Sound::ChannelHandle channel;

	try {
		Common::SeekableReadStream *soundStream = ResMan.getResource(resType, sound);
		if (!soundStream)
			return channel;

		channel = SoundMan.playSoundFile(soundStream, soundType, loop);

		debugC(Common::kDebugEngineSound, 1, "Playing sound \"%s\" in %s",
		       sound.c_str(), SoundMan.formatChannel(channel).c_str());

		SoundMan.setChannelGain(channel, volume);

		if (pitchVariance) {
			const float pitch = 1.0f + (((RNG.getNext(0, 1001) / 1000.0f) / 5.0f) - 0.1f);

			SoundMan.setChannelPitch(channel, pitch);
		}

		SoundMan.startChannel(channel);

	} catch (...) {
		Common::exceptionDispatcherWarning();
	}

	return channel;
}

void checkConfigInt(const Common::UString &key, int min, int max) {
	const int def = ConfigMan.getDefaultInt(key);

	const int value = ConfigMan.getInt(key, def);
	if ((value >= min) && (value <= max))
		return;

	warning("Config \"%s\" has invalid value (%d), resetting to default (%d)", key.c_str(), value, def);
	ConfigMan.setInt(key, def);
}

void checkConfigDouble(const Common::UString &key, double min, double max) {
	const double def = ConfigMan.getDefaultDouble(key);

	const double value = ConfigMan.getDouble(key, def);
	if ((value >= min) && (value <= max))
		return;

	warning("Config \"%s\" has invalid value (%lf), resetting to default (%lf)", key.c_str(), value, def);
	ConfigMan.setDouble(key, def);
}

bool longDelay(uint32_t ms) {
	while ((ms > 0) && !EventMan.quitRequested()) {
		uint32_t delay = MIN<uint32_t>(ms, 10);

		EventMan.delay(delay);

		ms -= delay;
	}

	return EventMan.quitRequested();
}

Aurora::GFF3File *loadOptionalGFF3(const Common::UString &gff3, Aurora::FileType type,
                                   uint32_t id, bool repairNWNPremium) {

	try {
		return new Aurora::GFF3File(gff3, type, id, repairNWNPremium);
	} catch (...) {
	}

	return 0;
}

Aurora::GFF4File *loadOptionalGFF4(const Common::UString &gff4,
                                   Aurora::FileType fileType, uint32_t type) {

	try {
		return new Aurora::GFF4File(gff4, fileType, type);
	} catch (...) {
	}

	return 0;
}

bool dumpResList(const Common::UString &name) {
	try {

		ResMan.dumpResourcesList(name);
		return true;

	} catch (...) {
	}

	return false;
}

bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName) {
	Common::WriteFile file;
	if (!file.open(fileName))
		return false;

	const size_t pos = stream.pos();
	try {
		stream.seek(0);

		file.writeStream(stream);
		stream.seek(pos);

		file.flush();

	} catch (...) {
		stream.seek(pos);
		return false;
	}

	file.close();
	return true;
}

bool dumpResource(const Common::UString &name, Aurora::FileType type, const Common::UString &file) {
	std::unique_ptr<Common::SeekableReadStream> res(ResMan.getResource(name, type));
	if (!res)
		return false;

	return dumpStream(*res, file.empty() ? TypeMan.setFileType(name, type) : file);
}

bool dumpResource(const Common::UString &name, const Common::UString &file) {
	Aurora::FileType type = TypeMan.getFileType(name);

	return dumpResource(TypeMan.setFileType(name, Aurora::kFileTypeNone), type, file);
}

bool dumpTGA(const Common::UString &name, const Common::UString &file) {
	try {
		std::unique_ptr<Graphics::ImageDecoder> image(Graphics::Aurora::Texture::loadImage(name));

		return image->dumpTGA(file.empty() ? (name + ".tga") : file);
	} catch (...) {
	}

	return false;
}

bool dump2DA(const Common::UString &name, const Common::UString &file) {
	std::unique_ptr<Common::SeekableReadStream> twoDAFile(ResMan.getResource(name, Aurora::kFileType2DA));
	if (!twoDAFile)
		return false;

	try {
		Aurora::TwoDAFile twoda(*twoDAFile);

		return twoda.writeASCII(file.empty() ? (name + ".2da") : file);
	} catch (...) {
	}

	return false;
}

} // End of namespace Engines
