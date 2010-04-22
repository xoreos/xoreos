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

#include "graphics/cube.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace TheWitcher {

const TheWitcherEngineProbe kTheWitcherEngineProbe;

const Common::UString TheWitcherEngineProbe::kGameName = "The Witcher";

Aurora::GameID TheWitcherEngineProbe::getGameID() const {
	return Aurora::kGameIDTheWitcher;
}

const Common::UString &TheWitcherEngineProbe::getGameName() const {
	return kGameName;
}

bool TheWitcherEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// There should be a system directory
	Common::UString systemDir = Common::FilePath::findSubDirectory(directory, "system", true);
	if (systemDir.empty())
		return false;

	// The system directory has to be readable
	Common::FileList systemFiles;
	if (!systemFiles.addDirectory(systemDir))
		return false;

	// If either witcher.ini or witcher.exe exists, this should be a valid path
	return systemFiles.contains(".*/witcher.(exe|ini)", true);
}

bool TheWitcherEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *TheWitcherEngineProbe::createEngine() const {
	return new TheWitcherEngine;
}


TheWitcherEngine::TheWitcherEngine() {
}

TheWitcherEngine::~TheWitcherEngine() {
}

void TheWitcherEngine::run(const Common::UString &target) {
	_baseDirectory = Common::FilePath::findSubDirectory(target, "data", true);
	if (_baseDirectory.empty())
		throw Common::Exception("No data directory");

	init();

	status("Successfully initialized the engine");

	playVideo("publisher");
	playVideo("developer");
	playVideo("engine");
	playVideo("intro");
	playVideo("title");

/*
	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("m1_axem00020005");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}
*/

	Graphics::Cube *cube = 0;

	try {

		cube = new Graphics::Cube("wilk");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
}

void TheWitcherEngine::init() {
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Loading main KEY");
	indexMandatoryKEY(".*/main.key", 0);

	status("Loading the localized base KEY");
	indexMandatoryKEY(".*/localized.key", 10);

	status("Finding further resource archives directories");
	ResMan.findSourceDirs();
	ResMan.addBIFSourceDir("voices");

	status("Loading the English language KEYs");
	indexMandatoryKEY(".*/lang_3.key", 20);
	indexMandatoryKEY(".*/M1_3.key"  , 21);
	indexMandatoryKEY(".*/M2_3.key"  , 22);

	status("Loading secondary resources");
	ResMan.loadSecondaryResources(30);

	status("Loading override files");
	ResMan.loadOverrideFiles(40);
}

} // End of namespace TheWitcher
