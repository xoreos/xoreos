/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/jade/jade.cpp
 *  Engine class handling Jade Empire
 */

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/error.h"

#include "engines/jade/jade.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/cube.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/jade/gui/main/main.h"

namespace Engines {

namespace Jade {

const JadeEngineProbe kJadeEngineProbe;

const Common::UString JadeEngineProbe::kGameName = "Jade Empire";

Aurora::GameID JadeEngineProbe::getGameID() const {
	return Aurora::kGameIDJade;
}

const Common::UString &JadeEngineProbe::getGameName() const {
	return kGameName;
}

bool JadeEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains(".*/JadeEmpire.exe", true))
		return true;

	return false;
}

bool JadeEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *JadeEngineProbe::createEngine() const {
	return new JadeEngine;
}


JadeEngine::JadeEngine() {
}

JadeEngine::~JadeEngine() {
}

void JadeEngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	CursorMan.hideCursor();
	CursorMan.set();

	playVideo("black");
	playVideo("publisher");
	playVideo("bwlogo");
	playVideo("graymatr");
	playVideo("attract");
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();


	MainMenu *mainMenu = new MainMenu();

	mainMenu->show();
	mainMenu->run();

	delete mainMenu;

	if (EventMan.quitRequested())
		return;


	playSound("musicbank00046", Sound::kSoundTypeMusic, true);

	bool showFPS = ConfigMan.getBool("showfps", false);

	Graphics::Aurora::FPS *fps = 0;
	if (showFPS) {
		fps = new Graphics::Aurora::FPS(FontMan.get("asian"));
		fps->show();
	}

	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("ui_ph_silk");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
	delete fps;
}

void JadeEngine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data/bips");

	// TODO: This should probably be done recoursively through data...
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j00_demonfight");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_beach");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_spiritcave");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_swamp");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_swampcave");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_town");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j01_townc");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_caves");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_fightclub");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_pirates");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_ruins");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_ruinslower");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_teahouse");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_tienlanding");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j02_workshop");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j03_caves");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j03_forest");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j03_furnace");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j03_heaven");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j03_inn");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_brnz");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_gold");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_pens");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_qual");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_silv");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_arena_tavern");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_city_core");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_city_kfs");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_city_rich");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_lotus");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_mhostel");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_necropolis");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j04_scholar");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j05_palace");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j05_throne");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_cave1");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_innercourt");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_outercourt");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_partycall");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_spirit");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_templecourt");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j06_templetop");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j07_drm_town");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j07_outercourt");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j07_templecourt");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j08_mind");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j08_palace");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j08_temple");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j08_throne");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data/j300_hench");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 0);

	status("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveERF, "loadscreens.mod"   , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "players.mod"       , 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global-a.rim"      , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "ingamemenu-a.rim"  , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globalunload-a.rim", 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "minigame-a.rim"    , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "miniglobal-a.rim"  , 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mmenu-a.rim"       , 17);

	status("Indexing extra font resources");
	indexMandatoryDirectory("fonts"   , 0, -1, 20);
	status("Indexing extra sound resources");
	indexMandatoryDirectory("sound"   , 0, -1, 21);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"  , 0, -1, 22);
	status("Indexing extra shader resources");
	indexMandatoryDirectory("shaderpc", 0, -1, 23);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 30);

	FontMan.setFormat(Graphics::Aurora::kFontFormatABC);
}

void JadeEngine::initCursors() {
	CursorMan.add("ui_cursor32", "default");

	CursorMan.setDefault("default");
}

} // End of namespace Jade

} // End of namespace Engines
