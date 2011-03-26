/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/console.cpp
 *  NWN (debug) console.
 */

#include "boost/bind.hpp"

#include "common/ustring.h"
#include "common/util.h"

#include "engines/aurora/util.h"

#include "engines/nwn/console.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

namespace Engines {

namespace NWN {

Console::Console() : ::Engines::Console("fnt_console"), _module(0) {
	registerCommand("quitmodule" , boost::bind(&Console::cmdQuitModule, this, _1),
			"Usage: quitmodule\nLeave the module, returning to the main menu");
	registerCommand("listareas"  , boost::bind(&Console::cmdListAreas , this, _1),
			"Usage: listareas\nList all areas in the current module");
	registerCommand("gotoarea"   , boost::bind(&Console::cmdGotoArea  , this, _1),
			"Usage: gotoarea <area>\nMove to a specific area");
	registerCommand("listmusic"  , boost::bind(&Console::cmdListMusic , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"  , boost::bind(&Console::cmdStopMusic , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"  , boost::bind(&Console::cmdPlayMusic , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("listvideos" , boost::bind(&Console::cmdListVideos, this, _1),
			"Usage: listvideos\nList all available videos");
	registerCommand("playvideo"  , boost::bind(&Console::cmdPlayVideo , this, _1),
			"Usage: playvideo <video>\nPlays the specified video");
	registerCommand("listsounds" , boost::bind(&Console::cmdListSounds, this, _1),
			"Usage: listsounds\nList all available sounds");
	registerCommand("playsound"  , boost::bind(&Console::cmdPlaySound , this, _1),
			"Usage: playsound <sound>\nPlays the specified sound");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::cmdQuitModule(const CommandLine &cl) {
	_module->_exit = true;
}

void Console::cmdListAreas(const CommandLine &cl) {
	if (!_module)
		return;

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		printf("%s (\"%s\")", a->c_str(), Area::getName(*a).c_str());
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	const std::vector<Common::UString> &areas = _module->_ifo.getAreas();
	for (std::vector<Common::UString>::const_iterator a = areas.begin(); a != areas.end(); ++a)
		if (a->equalsIgnoreCase(cl.args)) {
			_module->_newArea = *a;
			return;
		}

	printf("Area \"%s\" does not exist", cl.args.c_str());
}

void Console::cmdListMusic(const CommandLine &cl) {
	std::list<Aurora::ResourceManager::ResourceID> music;
	ResMan.getAvailabeResources(Aurora::kFileTypeBMU, music);

	uint32 maxSize = 0;
	std::list<Common::UString> musics;
	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator m = music.begin();
	     m != music.end(); ++m) {

		musics.push_back(m->name);

		maxSize = MAX(maxSize, musics.back().size());
	}

	printList(musics, maxSize);
}

void Console::cmdStopMusic(const CommandLine &cl) {
	if (!_module || !_module->_area)
		return;

	_module->_area->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	if (!_module || !_module->_area)
		return;

	_module->_area->playAmbientMusic(cl.args);
}

void Console::cmdListVideos(const CommandLine &cl) {
	std::list<Aurora::ResourceManager::ResourceID> videos;
	ResMan.getAvailabeResources(Aurora::kFileTypeBIK, videos);

	uint32 maxSize = 0;
	std::list<Common::UString> vids;
	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator v = videos.begin();
	     v != videos.end(); ++v) {

		vids.push_back(v->name);

		maxSize = MAX(maxSize, vids.back().size());
	}

	printList(vids, maxSize);
}

void Console::cmdPlayVideo(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	playVideo(cl.args);
}

void Console::cmdListSounds(const CommandLine &cl) {
	std::list<Aurora::ResourceManager::ResourceID> sounds;
	ResMan.getAvailabeResources(Aurora::kFileTypeWAV, sounds);

	uint32 maxSize = 0;
	std::list<Common::UString> snds;
	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator s = sounds.begin();
	     s != sounds.end(); ++s) {

		snds.push_back(s->name);

		maxSize = MAX(maxSize, snds.back().size());
	}

	printList(snds, maxSize);
}

void Console::cmdPlaySound(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	playSound(cl.args, Sound::kSoundTypeSFX);
}

} // End of namespace NWN

} // End of namespace Engines
