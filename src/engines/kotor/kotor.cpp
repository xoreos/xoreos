/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/kotor.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#include "engines/kotor/kotor.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/stream.h"

#include "graphics/cube.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace KotOR {

static const std::string kGameName = "Star Wars: Knights of the Old Republic";

const KotOREngineProbe kKotOREngineProbe;

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const std::string &KotOREngineProbe::getGameName() const {
	return kGameName;
}

bool KotOREngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// If either swkotor.ini or swkotor.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor.(exe|ini)", true);
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine;
}


KotOREngine::KotOREngine() {
}

KotOREngine::~KotOREngine() {
}

void KotOREngine::run(const std::string &directory) {
	_baseDirectory = directory;

	init();

	status("Successfully initialized the engine");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("nm35aahhkd07134_");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}

	Graphics::Cube *cube = 0;

	try {

		cube = new Graphics::Cube("chrome1");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
}

void KotOREngine::init() {
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Loading main KEY");
	indexMandatoryKEY(".*/chitin.key", 0);

	status("Finding further resource archives directories");
	ResMan.findSourceDirs();

	status("Loading global auxiliary resources");
	indexMandatoryRIM("mainmenu.rim"  , 10);
	indexMandatoryRIM("mainmenudx.rim", 11);
	indexMandatoryRIM("legal.rim"     , 12);
	indexMandatoryRIM("legaldx.rim"   , 13);
	indexMandatoryRIM("global.rim"    , 14);
	indexMandatoryRIM("globaldx.rim"  , 15);
	indexMandatoryRIM("chargen.rim"   , 16);
	indexMandatoryRIM("chargendx.rim" , 17);

	status("Loading high-res texture packs");
	indexMandatoryERF("swpc_tex_gui.erf", 20);
	indexMandatoryERF("swpc_tex_tpa.erf", 21);

	status("Loading secondary resources");
	ResMan.loadSecondaryResources(30);

	status("Loading override files");
	ResMan.loadOverrideFiles(40);
}

} // End of namespace KotOR
