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
 *  In-game sound within an area in KotOR games.
 */

#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/sound.h"

namespace Engines {

namespace KotORBase {

SoundObject::SoundObject(const Aurora::GFF3Struct &sound) {
	Common::UString temp = sound.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> uts;
	if (!temp.empty())
		uts.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTS, MKTAG('U', 'T', 'S', ' ')));

	if (!uts)
		throw Common::Exception("Sound \"%s\" has no blueprint", _tag.c_str());

	const Aurora::GFF3Struct &gff = uts->getTopLevel();

	Aurora::GFF3List soundFileList = gff.getList("Sounds");
	for (Aurora::GFF3List::const_iterator c = soundFileList.begin(); c != soundFileList.end(); ++c) {
		_soundFiles.push_back((*c)->getString("Sound"));
	}

	_looping = gff.getBool("Looping");
	_positional = gff.getBool("Positional");
	_random = gff.getBool("Random");

	_interval = gff.getUint("Interval");

	// TODO: Investigate how kotor handles randomness of sounds.
	Common::UString soundFile = _soundFiles[0];
	if (_random)
		soundFile = _soundFiles[std::rand() % _soundFiles.size()];

	Common::SeekableReadStream *soundStream = ResMan.getResource(Aurora::kResourceSound, soundFile);

	_sound = SoundMan.playSoundFile(soundStream, Sound::kSoundTypeSFX, _looping);

	_name = gff.getString("Tag");

	if (gff.getBool("Active")) {
		play();
	}

	setPosition(
			static_cast<float>(sound.getDouble("XPosition")),
			static_cast<float>(sound.getDouble("YPosition")),
			static_cast<float>(sound.getDouble("ZPosition"))
	);

	SoundMan.setChannelGain(_sound, static_cast<float>(gff.getUint("Volume"))/100.0f);
	SoundMan.setChannelRelative(_sound, false);

	SoundMan.setChannelDistance(
		_sound,
		static_cast<float>(gff.getDouble("MinDistance")),
		static_cast<float>(gff.getDouble("MaxDistance"))
	);
}

void SoundObject::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);

	if (_positional)
		SoundMan.setChannelPosition(_sound, x, y, z);
}

void SoundObject::play() {
	SoundMan.startChannel(_sound);
}

void SoundObject::stop() {
	SoundMan.stopChannel(_sound);
}

} // End of namespace KotORBase

} // End of namespace Engines
