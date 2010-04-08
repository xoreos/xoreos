/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/thewitcher/thewitcher.cpp
 *  Engine class handling The Witcher
 */

#include "engines/thewitcher/thewitcher.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"

namespace TheWitcher {

const TheWitcherEngineProbe kTheWitcherEngineProbe;

Aurora::GameID TheWitcherEngineProbe::getGameID() const {
	return Aurora::kGameIDTheWitcher;
}

bool TheWitcherEngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// There should be a ../system directory
	std::string systemDir = Common::FilePath::findSubDirectory(directory + "/../", "system", true);
	if (systemDir.empty())
		return false;

	// The ../system directory has to be readable
	Common::FileList systemFiles;
	if (!systemFiles.addDirectory(systemDir))
		return false;

	// If either witcher.ini or witcher.exe exists, this should be a valid path
	return systemFiles.contains(".*/witcher.(exe|ini)", true);
}

Engines::Engine *TheWitcherEngineProbe::createEngine() const {
	return new TheWitcherEngine;
}


TheWitcherEngine::TheWitcherEngine() {
}

TheWitcherEngine::~TheWitcherEngine() {
}

bool TheWitcherEngine::run(const std::string &directory) {
	_baseDirectory = directory;

	if (!init())
		return false;

	status("Successfully initialized the engine");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("m1_axem00020005");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}

	while (!EventMan.quitRequested()) {
		if (!SoundMan.isPlaying(channel))
			EventMan.requestQuit();

		Events::Event event;
		while (EventMan.pollEvent(event));

		EventMan.delay(10);
	}

	return true;
}

bool TheWitcherEngine::init() {
	if (!ResMan.registerDataBaseDir(_baseDirectory))
		return false;

	status("Loading main KEY");
	if (!indexMandatoryKEY(".*/main.key"))
		return false;

	status("Loading the localized base KEY");
	if (!indexMandatoryKEY(".*/localized.key"))
		return false;

	ResMan.addBIFSourceDir("voices");

	status("Loading the English language KEYs");
	if (!indexMandatoryKEY(".*/lang_3.key"))
		return false;
	if (!indexMandatoryKEY(".*/M1_3.key"))
		return false;
	if (!indexMandatoryKEY(".*/M2_3.key"))
		return false;

	status("Loading secondary resources");
	if (!ResMan.loadSecondaryResources())
		return false;

	return true;
}

} // End of namespace TheWitcher
