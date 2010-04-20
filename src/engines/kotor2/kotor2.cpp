/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/kotor2.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#include "engines/kotor2/kotor2.h"

#include "common/util.h"
#include "common/strutil.h"
#include "common/filelist.h"
#include "common/stream.h"

#include "graphics/graphics.h"
#include "graphics/cube.h"
#include "graphics/font.h"
#include "graphics/text.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace KotOR2 {

const KotOR2EngineProbe kKotOR2EngineProbe;

const std::string KotOR2EngineProbe::kGameName = "Star Wars: Knights of the Old Republic II - The Sith Lords";

Aurora::GameID KotOR2EngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR2;
}

const std::string &KotOR2EngineProbe::getGameName() const {
	return kGameName;
}

bool KotOR2EngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// If either swkotor2.ini or swkotor2.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor2.(exe|ini)", true);
}

Engines::Engine *KotOR2EngineProbe::createEngine() const {
	return new KotOR2Engine;
}


KotOR2Engine::KotOR2Engine() {
}

KotOR2Engine::~KotOR2Engine() {
}

void KotOR2Engine::run(const std::string &directory) {
	_baseDirectory = directory;

	init();

	status("Successfully initialized the engine");

	playVideo("leclogo");
	playVideo("obsidianent");
	playVideo("legal");
	// Intro movie: "permov01"

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("298hk50mun003");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}

	Graphics::Cube *cube = 0;

	try {

		cube = new Graphics::Cube("tel_hk51_front");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	Graphics::Font *font = new Graphics::Font("dialogfont32x32b");
	Graphics::Text *text = 0;

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);

		GfxMan.lockFrame();
		delete text;
		text = new Graphics::Text(*font, -1.0, 1.0, Common::sprintf("%d fps", GfxMan.getFPS()));
		GfxMan.unlockFrame();
	}

	delete text;
	delete font;

	delete cube;
}

void KotOR2Engine::init() {
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Loading main KEY");
	indexMandatoryKEY(".*/chitin.key", 0);

	status("Finding further resource archives directories");
	ResMan.findSourceDirs();

	status("Loading high-res texture packs");
	indexMandatoryERF("swpc_tex_gui.erf", 10);
	indexMandatoryERF("swpc_tex_tpa.erf", 11);

	status("Loading secondary resources");
	ResMan.loadSecondaryResources(20);

	status("Loading override files");
	ResMan.loadOverrideFiles(30);
}

} // End of namespace KotOR2
