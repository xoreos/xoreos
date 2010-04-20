/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/nwn.cpp
 *  Engine class handling Neverwinter Nights
 */

#include "engines/nwn/nwn.h"

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

namespace NWN {

const NWNEngineProbe kNWNEngineProbe;

const std::string NWNEngineProbe::kGameName = "Neverwinter Nights";

Aurora::GameID NWNEngineProbe::getGameID() const {
	return Aurora::kGameIDNWN;
}

const std::string &NWNEngineProbe::getGameName() const {
	return kGameName;
}

bool NWNEngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// Don't accidentally trigger on NWN2
	if (rootFiles.contains(".*/nwn2.ini", true))
		return false;
	if (rootFiles.contains(".*/nwn2main.exe", true))
		return false;

	// If either the ini file or a binary is found, this should be a valid path

	if (rootFiles.contains(".*/nwn.ini", true))
		return true;

	if (rootFiles.contains(".*/(nw|nwn)main.exe", true))
		return true;

	if (rootFiles.contains(".*/(nw|nwn)main", true))
		return true;

	return false;
}

Engines::Engine *NWNEngineProbe::createEngine() const {
	return new NWNEngine;
}


NWNEngine::NWNEngine() {
}

NWNEngine::~NWNEngine() {
}

void NWNEngine::run(const std::string &directory) {
	_baseDirectory = directory;

	init();

	status("Successfully initialized the engine");

	playVideo("atarilogo");
	playVideo("biowarelogo");
	playVideo("wotclogo");
	playVideo("fge_logo_black");
	playVideo("nwnintro");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("as_pl_evanglstm1");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}

	Graphics::Cube *cube = 0;

	try {

		cube = new Graphics::Cube("ashlw_066");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	Graphics::Font *font = new Graphics::Font("fnt_galahad14");
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

void NWNEngine::init() {
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Loading main KEY");
	indexMandatoryKEY(".*/chitin.key", 0);

	status("Loading expansions and patch KEYs");

	// Base game patch
	indexOptionalKEY(".*/patch.key", 1);

	// Expansion 1: Shadows of Undrentide (SoU)
	indexOptionalKEY(".*/xp1.key"     , 2);
	indexOptionalKEY(".*/xp1patch.key", 3);

	// Expansion 2: Hordes of the Underdark (HotU)
	indexOptionalKEY(".*/xp2.key"     , 4);
	indexOptionalKEY(".*/xp2patch.key", 5);

	// Expansion 3: Kingmaker (resources also included in the final 1.69 patch)
	indexOptionalKEY(".*/xp3.key"     , 6);
	indexOptionalKEY(".*/xp3patch.key", 7);

	status("Finding further resource archives directories");
	ResMan.findSourceDirs();

	status("Loading high-res texture packs");
	indexMandatoryERF("gui_32bit.erf"   , 10);
	indexOptionalERF ("xp1_gui.erf"     , 11);
	indexOptionalERF ("xp2_gui.erf"     , 12);
	indexMandatoryERF("textures_tpa.erf", 13);
	indexMandatoryERF("tiles_tpa.erf"   , 14);
	indexOptionalERF ("xp1_tex_tpa.erf" , 15);
	indexOptionalERF ("xp2_tex_tpa.erf" , 16);

	status("Loading secondary resources");
	ResMan.loadSecondaryResources(20);

	status("Loading override files");
	ResMan.loadOverrideFiles(30);
}

} // End of namespace NWN
