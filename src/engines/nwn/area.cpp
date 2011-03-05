/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/area.cpp
 *  NWN area.
 */

#include "common/endianness.h"
#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "engines/aurora/util.h"

#include "engines/nwn/area.h"
#include "engines/nwn/module.h"

namespace Engines {

namespace NWN {

Area::Area(Module &module, const Common::UString &name) : _module(&module), _name(name),
	_visible(false) {

	Aurora::GFFFile are;
	loadGFF(are, _name, Aurora::kFileTypeARE, MKID_BE('ARE '));
	loadARE(are.getTopLevel());

	Aurora::GFFFile git;
	loadGFF(git, _name, Aurora::kFileTypeGIT, MKID_BE('GIT '));
	loadGIT(git.getTopLevel());
}

Area::~Area() {
	hide();
}

const Common::UString &Area::getName() {
	return _name;
}

void Area::show() {
	if (_visible)
		return;

	SoundMan.stopChannel(_ambientSound);
	SoundMan.stopChannel(_ambientMusic);

	// TODO Day/Night

	/* IMA ADPCM currently broken for stereo files!
	if (!_ambientDay.empty())
		_ambientSound = playSound(_ambientDay, Sound::kSoundTypeSFX  , true, _ambientDayVol);
	*/
	if (!_musicDay.empty())
		_ambientMusic = playSound(_musicDay  , Sound::kSoundTypeMusic, true);

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	// Stop sound
	SoundMan.stopChannel(_ambientSound);
	SoundMan.stopChannel(_ambientMusic);

	_visible = false;
}

void Area::loadARE(const Aurora::GFFStruct &are) {
}

void Area::loadGIT(const Aurora::GFFStruct &git) {
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));
}

void Area::loadProperties(const Aurora::GFFStruct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get("ambientsound");

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , 0xFFFFFFFF);
	uint32 ambientNight = props.getUint("AmbientSndNight", 0xFFFFFFFF);

	if (ambientDay   != 0xFFFFFFFF)
		_ambientDay   = ambientSound.getCellString(ambientDay  , "Resource");
	if (ambientNight != 0xFFFFFFFF)
		_ambientNight = ambientSound.getCellString(ambientNight, "Resource");

	_ambientDayVol   =  props.getUint("AmbientSndDayVol"  , 127) / 127.0;
	_ambientNightVol =  props.getUint("AmbientSndNightVol", 127) / 127.0;

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get("ambientmusic");

	uint32 musicDay   = props.getUint("MusicDay"   , 0xFFFFFFFF);
	uint32 musicNight = props.getUint("MusicNight" , 0xFFFFFFFF);

	if (musicDay   != 0xFFFFFFFF)
		_musicDay   = ambientMusic.getCellString(musicDay  , "Resource");
	if (musicNight != 0xFFFFFFFF)
		_musicNight = ambientMusic.getCellString(musicNight, "Resource");


	// Battle music

	uint32 musicBattle = props.getUint("MusicBattle", 0xFFFFFFFF);

	if (musicBattle != 0xFFFFFFFF) {
		_musicBattle = ambientMusic.getCellString(musicBattle, "Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getCellString(musicBattle, "Stinger1");
		stinger[1] = ambientMusic.getCellString(musicBattle, "Stinger2");
		stinger[2] = ambientMusic.getCellString(musicBattle, "Stinger3");

		for (int i = 0; i < 3; i++)
			if (!stinger[i].empty())
				_musicBattleStinger.push_back(stinger[i]);
	}
}

} // End of namespace NWN

} // End of namespace Engines
