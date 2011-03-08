/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "engines/kotor2/modelloader.h"
#include "engines/kotor2/module.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/model.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/stream.h"
#include "common/configman.h"

#include "graphics/camera.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

namespace KotOR2 {

const KotOR2EngineProbe kKotOR2EngineProbe;

const Common::UString KotOR2EngineProbe::kGameName = "Star Wars: Knights of the Old Republic II - The Sith Lords";

Aurora::GameID KotOR2EngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR2;
}

const Common::UString &KotOR2EngineProbe::getGameName() const {
	return kGameName;
}

bool KotOR2EngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If either swkotor2.ini or swkotor2.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor2.(exe|ini)", true);
}

bool KotOR2EngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *KotOR2EngineProbe::createEngine() const {
	return new KotOR2Engine;
}


KotOR2Engine::KotOR2Engine() {
}

KotOR2Engine::~KotOR2Engine() {
}

void KotOR2Engine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	playVideo("leclogo");
	playVideo("obsidianent");
	playVideo("legal");

	playVideo("permov01");

	playSound("298hk50mun003", Sound::kSoundTypeVoice);

	// Test load up Nar Shaddaa

	Module *narShaddaa = new Module;

	narShaddaa->load("301NAR");
	narShaddaa->enter();

	bool showFPS = ConfigMan.getBool("showfps", false);

	Graphics::Aurora::FPS *fps = 0;
	if (showFPS) {
		fps = new Graphics::Aurora::FPS(FontMan.get("dialogfont32x32b"));
		fps->show();
	}

	EventMan.enableKeyRepeat();

	status("Entering event loop");

	// Show a cursor
	CursorMan.set("default", false);

	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (event.type == Events::kEventKeyDown) {
				if      (event.key.keysym.sym == SDLK_UP)
					CameraMan.move( 0.5);
				else if (event.key.keysym.sym == SDLK_DOWN)
					CameraMan.move(-0.5);
				else if (event.key.keysym.sym == SDLK_RIGHT)
					CameraMan.turn( 0.0,  5.0, 0.0);
				else if (event.key.keysym.sym == SDLK_LEFT)
					CameraMan.turn( 0.0, -5.0, 0.0);
				else if (event.key.keysym.sym == SDLK_w)
					CameraMan.move( 0.5);
				else if (event.key.keysym.sym == SDLK_s)
					CameraMan.move(-0.5);
				else if (event.key.keysym.sym == SDLK_d)
					CameraMan.turn( 0.0,  5.0, 0.0);
				else if (event.key.keysym.sym == SDLK_a)
					CameraMan.turn( 0.0, -5.0, 0.0);
				else if (event.key.keysym.sym == SDLK_e)
					CameraMan.strafe( 0.5);
				else if (event.key.keysym.sym == SDLK_q)
					CameraMan.strafe(-0.5);
				else if (event.key.keysym.sym == SDLK_INSERT)
					CameraMan.move(0.0,  0.5, 0.0);
				else if (event.key.keysym.sym == SDLK_DELETE)
					CameraMan.move(0.0, -0.5, 0.0);
				else if (event.key.keysym.sym == SDLK_PAGEUP)
					CameraMan.turn( 5.0,  0.0, 0.0);
				else if (event.key.keysym.sym == SDLK_PAGEDOWN)
					CameraMan.turn(-5.0,  0.0, 0.0);
				else if (event.key.keysym.sym == SDLK_END) {
					const float *orient = CameraMan.getOrientation();

					CameraMan.setOrientation(0.0, orient[1], orient[2]);
				} else if (event.key.keysym.sym == SDLK_t) {
					const float *pos = CameraMan.getPosition();
					const float *ort = CameraMan.getOrientation();

					warning("%f, %f, %f -- %f, %f, %f", pos[0], pos[1], pos[2], ort[0], ort[1], ort[2]);
				}
			}
		}

		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(0);

	narShaddaa->leave();

	delete narShaddaa;

	delete fps;
}

void KotOR2Engine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "lips");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "texturepacks");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "rims");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "modules");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 0);

	status("Loading high-res texture packs");
	indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_gui.erf", 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_tpa.erf", 11);

	status("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 20);
	status("Indexing extra voice resources");
	indexMandatoryDirectory("streamvoice" , 0, -1, 21);
	status("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 22);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 23);

	status("Indexing Windows-specific resources");
	indexMandatoryArchive(Aurora::kArchiveEXE, "swkotor2.exe", 24);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 30);

	registerModelLoader(new KotOR2ModelLoader);

	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);
}

void KotOR2Engine::initCursors() {
	CursorMan.add("gui_mp_defaultd" , "default"  , true);
	CursorMan.add("gui_mp_defaultu" , "default"  , false);

	CursorMan.add("gui_mp_bashd"    , "bash"     , true);
	CursorMan.add("gui_mp_bashu"    , "bash"     , false);
	CursorMan.add("gui_mp_bashdp"   , "bash+"    , true);
	CursorMan.add("gui_mp_bashup"   , "bash+"    , false);
	CursorMan.add("gui_mp_dismined" , "dismine"  , true);
	CursorMan.add("gui_mp_dismineu" , "dismine"  , false);
	CursorMan.add("gui_mp_disminedp", "dismine+" , true);
	CursorMan.add("gui_mp_dismineup", "dismine+" , false);
	CursorMan.add("gui_mp_doord"    , "door"     , true);
	CursorMan.add("gui_mp_dooru"    , "door"     , false);
	CursorMan.add("gui_mp_doordp"   , "door+"    , true);
	CursorMan.add("gui_mp_doorup"   , "door+"    , false);
	CursorMan.add("gui_mp_invalidd" , "invalid"  , true);
	CursorMan.add("gui_mp_invalidu" , "invalid"  , false);
	CursorMan.add("gui_mp_killd"    , "kill"     , true);
	CursorMan.add("gui_mp_killu"    , "kill"     , false);
	CursorMan.add("gui_mp_recmined" , "recmine"  , true);
	CursorMan.add("gui_mp_recmineu" , "recmine"  , false);
	CursorMan.add("gui_mp_recminedp", "recmine+" , true);
	CursorMan.add("gui_mp_recmineup", "recmine+" , false);
	CursorMan.add("gui_mp_selectd"  , "select"   , true);
	CursorMan.add("gui_mp_selectu"  , "select"   , false);
	CursorMan.add("gui_mp_talkd"    , "talk"     , true);
	CursorMan.add("gui_mp_talku"    , "talk"     , false);
	CursorMan.add("gui_mp_used"     , "use"      , true);
	CursorMan.add("gui_mp_useu"     , "use"      , false);
	CursorMan.add("gui_mp_usedp"    , "use+"     , true);
	CursorMan.add("gui_mp_useup"    , "use+"     , false);
}

} // End of namespace KotOR2

} // End of namespace Engines
