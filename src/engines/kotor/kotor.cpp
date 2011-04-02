/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "engines/kotor/modelloader.h"
#include "engines/kotor/module.h"
#include "engines/kotor/console.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/model.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/stream.h"
#include "common/configman.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"
#include "graphics/aurora/model.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

namespace KotOR {

const KotOREngineProbeWin  kKotOREngineProbeWin;
const KotOREngineProbeMac  kKotOREngineProbeMac;
const KotOREngineProbeXbox kKotOREngineProbeXbox;

const Common::UString KotOREngineProbe::kGameName = "Star Wars: Knights of the Old Republic";

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const Common::UString &KotOREngineProbe::getGameName() const {
	return kGameName;
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine(getPlatform());
}

bool KotOREngineProbeWin::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If swkotor.exe exists, this should be a valid path for the Windows port
	return rootFiles.contains(".*/swkotor.exe", true);
}

bool KotOREngineProbeMac::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "Knights of the Old Republic.app" directory exists, this should be a valid path for the Mac OS X port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app");
	return !appDirectory.empty();
}

bool KotOREngineProbeXbox::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "dataxbox" directory exists and "players.erf" exists, this should be a valid path for the Xbox port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");
	return !appDirectory.empty() && rootFiles.contains(".*/players.erf", true);
}


KotOREngine::KotOREngine(Aurora::Platform platform) : _platform(platform) {
	_hasLiveKey = false;
}

KotOREngine::~KotOREngine() {
}

void KotOREngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	CursorMan.hideCursor();
	CursorMan.set();

	if (_platform == Aurora::kPlatformXbox) {
		playVideo("logo");
		// TODO: What the hell is (sizzle|sizzle2).xmv?
	} else {
		playVideo("leclogo");
		playVideo("biologo");

		// On Mac OS X, play the Aspyr logo
		if (_platform == Aurora::kPlatformMacOSX)
			playVideo("Aspyr_BlueDust_intro");

		playVideo("legal");
	}

	playVideo("01a");

	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	playSound("nm35aahhkd07134_", Sound::kSoundTypeVoice);

	Module *module = new Module;

	Console *console = new Console(*module);

	// Test load up a Leviathan module
	module->load("lev_m40aa");
	module->enter();

	bool showFPS = ConfigMan.getBool("showfps", false);

	Graphics::Aurora::FPS *fps = 0;
	if (showFPS) {
		fps = new Graphics::Aurora::FPS(FontMan.get("dialogfont32x32"));
		fps->show();
	}

	EventMan.enableUnicode(true);
	EventMan.enableKeyRepeat();

	status("Entering event loop");

	uint32 lastCameraChange = 0;
	Graphics::Aurora::Model *activeModel = 0;

	while (!EventMan.quitRequested()) {
		bool hasMove = false;

		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (console->processEvent(event))
				continue;

			if (event.type == Events::kEventKeyDown) {
				if      ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL))
					console->show();
				else if (event.key.keysym.sym == SDLK_UP)
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
			} else if (event.type == Events::kEventMouseMove)
				hasMove = true;
		}

		uint32 curLastCameraChange = CameraMan.lastChanged();
		if (lastCameraChange < curLastCameraChange) {
			hasMove = true;
			lastCameraChange = curLastCameraChange;
		}

		if (hasMove) {
			int mouseX, mouseY;
			CursorMan.getPosition(mouseX, mouseY);

			Graphics::Aurora::Model *model =
				dynamic_cast<Graphics::Aurora::Model *>(GfxMan.getObjectAt(mouseX, mouseY));
			if (model != activeModel) {
				if (activeModel)
					activeModel->drawBound(false);

				activeModel = model;

				if (activeModel) {
					warning("Now in \"%s\" (%d)", activeModel->getTag().c_str(),
							activeModel->getID());
					activeModel->drawBound(true);
				}
			}
		}

		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(0);
	EventMan.enableUnicode(false);

	delete console;

	module->leave();

	delete module;

	delete fps;
}

void KotOREngine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, (_platform == Aurora::kPlatformXbox) ? "dataxbox" : "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "lips");

	if (_platform != Aurora::kPlatformXbox)
		ResMan.addArchiveDir(Aurora::kArchiveERF, "texturepacks");

	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, (_platform == Aurora::kPlatformXbox) ? "rimsxbox" : "rims");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "modules");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 0);

	if (indexOptionalArchive(Aurora::kArchiveKEY, "live1.key", 1)) {
		status("Loading Xbox DLC KEY");
		_hasLiveKey = true;
	}

	status("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenu.rim"  , 10);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenudx.rim", 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legal.rim"     , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legaldx.rim"   , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global.rim"    , 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globaldx.rim"  , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargen.rim"   , 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargendx.rim" , 17);

	if (_platform == Aurora::kPlatformXbox) {
		// The Xbox version has most of its textures in "textures.bif"
		// Some, however, reside in "players.erf"
		status("Loading Xbox textures");
		indexMandatoryArchive(Aurora::kArchiveERF, "players.erf", 20);
	} else {
		// The Windows/Mac versions have swappable texture packs
		status("Loading high-res texture packs");
		indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_gui.erf", 20);
		indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_tpa.erf", 21);
	}

	status("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 30);
	status("Indexing extra voice resources");
	indexMandatoryDirectory("streamwaves" , 0, -1, 31);
	status("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 32);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 33);

	if (_platform == Aurora::kPlatformWindows) {
		status("Indexing Windows-specific resources");
		initCursorsRemap();
		indexMandatoryArchive(Aurora::kArchiveEXE, "swkotor.exe", 34);
	} else if (_platform == Aurora::kPlatformMacOSX) {
		status("Indexing Mac-specific resources");
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources",         0, -1, 34);
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources/Cursors", 0, -1, 35);
	} else if (_platform == Aurora::kPlatformXbox) {
		status("Indexing Xbox-specific resources");
		indexMandatoryDirectory("errortex"  , 0, -1, 34);
		indexMandatoryDirectory("localvault", 0, -1, 35);
		indexMandatoryDirectory("media"     , 0, -1, 36);

		// For the DLC, we need to index the "sound" directory as well
		if (_hasLiveKey)
			indexMandatoryDirectory("sound", 0, -1, 37);
	}

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 40);

	registerModelLoader(new KotORModelLoader);

	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);
}

void KotOREngine::initCursorsRemap() {
	std::vector<Common::UString> cursorRemap;

	cursorRemap.push_back("gui_mp_defaultu");
	cursorRemap.push_back("gui_mp_defaultd");
	cursorRemap.push_back("gui_mp_walku");
	cursorRemap.push_back("gui_mp_walkd");
	cursorRemap.push_back("gui_mp_invalidu");
	cursorRemap.push_back("gui_mp_invalidd");
	cursorRemap.push_back("gui_mp_bashu");
	cursorRemap.push_back("gui_mp_bashd");
	cursorRemap.push_back("gui_mp_bashup");
	cursorRemap.push_back("gui_mp_bashdp");
	cursorRemap.push_back("gui_mp_talku");
	cursorRemap.push_back("gui_mp_talkd");
	cursorRemap.push_back("gui_mp_notalku");
	cursorRemap.push_back("gui_mp_notalkd");
	cursorRemap.push_back("gui_mp_followu");
	cursorRemap.push_back("gui_mp_followd");
	cursorRemap.push_back("gui_mp_examineu");
	cursorRemap.push_back("gui_mp_examined");
	cursorRemap.push_back("gui_mp_noexamu");
	cursorRemap.push_back("gui_mp_noexamd");
	cursorRemap.push_back("gui_mp_transu");
	cursorRemap.push_back("gui_mp_transd");
	cursorRemap.push_back("gui_mp_dooru");
	cursorRemap.push_back("gui_mp_doord");
	cursorRemap.push_back("gui_mp_useu");
	cursorRemap.push_back("gui_mp_used");
	cursorRemap.push_back("gui_mp_useup");
	cursorRemap.push_back("gui_mp_usedp");
	cursorRemap.push_back("gui_mp_magicu");
	cursorRemap.push_back("gui_mp_magicd");
	cursorRemap.push_back("gui_mp_nomagicu");
	cursorRemap.push_back("gui_mp_nomagicd");
	cursorRemap.push_back("gui_mp_dismineu");
	cursorRemap.push_back("gui_mp_dismined");
	cursorRemap.push_back("gui_mp_dismineup");
	cursorRemap.push_back("gui_mp_disminedp");
	cursorRemap.push_back("gui_mp_recmineu");
	cursorRemap.push_back("gui_mp_recmined");
	cursorRemap.push_back("gui_mp_recmineup");
	cursorRemap.push_back("gui_mp_recminedp");
	cursorRemap.push_back("gui_mp_locku");
	cursorRemap.push_back("gui_mp_lockd");
	cursorRemap.push_back("gui_mp_doorup");
	cursorRemap.push_back("gui_mp_doordp");
	cursorRemap.push_back("gui_mp_selectu");
	cursorRemap.push_back("gui_mp_selectd");
	cursorRemap.push_back("gui_mp_createu");
	cursorRemap.push_back("gui_mp_created");
	cursorRemap.push_back("gui_mp_nocreatu");
	cursorRemap.push_back("gui_mp_nocreatd");
	cursorRemap.push_back("gui_mp_killu");
	cursorRemap.push_back("gui_mp_killd");
	cursorRemap.push_back("gui_mp_nokillu");
	cursorRemap.push_back("gui_mp_nokilld");
	cursorRemap.push_back("gui_mp_healu");
	cursorRemap.push_back("gui_mp_heald");
	cursorRemap.push_back("gui_mp_nohealu");
	cursorRemap.push_back("gui_mp_noheald");
	cursorRemap.push_back("gui_mp_arrun00");
	cursorRemap.push_back("gui_mp_arrun01");
	cursorRemap.push_back("gui_mp_arrun02");
	cursorRemap.push_back("gui_mp_arrun03");
	cursorRemap.push_back("gui_mp_arrun04");
	cursorRemap.push_back("gui_mp_arrun05");
	cursorRemap.push_back("gui_mp_arrun06");
	cursorRemap.push_back("gui_mp_arrun07");
	cursorRemap.push_back("gui_mp_arrun08");
	cursorRemap.push_back("gui_mp_arrun09");
	cursorRemap.push_back("gui_mp_arrun10");
	cursorRemap.push_back("gui_mp_arrun11");
	cursorRemap.push_back("gui_mp_arrun12");
	cursorRemap.push_back("gui_mp_arrun13");
	cursorRemap.push_back("gui_mp_arrun14");
	cursorRemap.push_back("gui_mp_arrun15");
	cursorRemap.push_back("gui_mp_arwalk00");
	cursorRemap.push_back("gui_mp_arwalk01");
	cursorRemap.push_back("gui_mp_arwalk02");
	cursorRemap.push_back("gui_mp_arwalk03");
	cursorRemap.push_back("gui_mp_arwalk04");
	cursorRemap.push_back("gui_mp_arwalk05");
	cursorRemap.push_back("gui_mp_arwalk06");
	cursorRemap.push_back("gui_mp_arwalk07");
	cursorRemap.push_back("gui_mp_arwalk08");
	cursorRemap.push_back("gui_mp_arwalk09");
	cursorRemap.push_back("gui_mp_arwalk10");
	cursorRemap.push_back("gui_mp_arwalk11");
	cursorRemap.push_back("gui_mp_arwalk12");
	cursorRemap.push_back("gui_mp_arwalk13");
	cursorRemap.push_back("gui_mp_arwalk14");
	cursorRemap.push_back("gui_mp_arwalk15");
	cursorRemap.push_back("gui_mp_pickupu");
	cursorRemap.push_back("gui_mp_pickupd");

	ResMan.setCursorRemap(cursorRemap);
}

void KotOREngine::initCursors() {
	if (_platform == Aurora::kPlatformXbox)
		return;

	CursorMan.add("gui_mp_defaultd" , "default"  , "down");
	CursorMan.add("gui_mp_defaultu" , "default"  , "up"  );

	CursorMan.add("gui_mp_bashd"    , "bash"     , "down");
	CursorMan.add("gui_mp_bashu"    , "bash"     , "up"  );
	CursorMan.add("gui_mp_bashdp"   , "bash+"    , "down");
	CursorMan.add("gui_mp_bashup"   , "bash+"    , "up"  );
	CursorMan.add("gui_mp_dismined" , "dismine"  , "down");
	CursorMan.add("gui_mp_dismineu" , "dismine"  , "up"  );
	CursorMan.add("gui_mp_disminedp", "dismine+" , "down");
	CursorMan.add("gui_mp_dismineup", "dismine+" , "up"  );
	CursorMan.add("gui_mp_doord"    , "door"     , "down");
	CursorMan.add("gui_mp_dooru"    , "door"     , "up"  );
	CursorMan.add("gui_mp_doordp"   , "door+"    , "down");
	CursorMan.add("gui_mp_doorup"   , "door+"    , "up"  );
	CursorMan.add("gui_mp_invalidd" , "invalid"  , "down");
	CursorMan.add("gui_mp_invalidu" , "invalid"  , "up"  );
	CursorMan.add("gui_mp_killd"    , "kill"     , "down");
	CursorMan.add("gui_mp_killu"    , "kill"     , "up"  );
	CursorMan.add("gui_mp_recmined" , "recmine"  , "down");
	CursorMan.add("gui_mp_recmineu" , "recmine"  , "up"  );
	CursorMan.add("gui_mp_recminedp", "recmine+" , "down");
	CursorMan.add("gui_mp_recmineup", "recmine+" , "up"  );
	CursorMan.add("gui_mp_selectd"  , "select"   , "down");
	CursorMan.add("gui_mp_selectu"  , "select"   , "up"  );
	CursorMan.add("gui_mp_talkd"    , "talk"     , "down");
	CursorMan.add("gui_mp_talku"    , "talk"     , "up"  );
	CursorMan.add("gui_mp_used"     , "use"      , "down");
	CursorMan.add("gui_mp_useu"     , "use"      , "up"  );
	CursorMan.add("gui_mp_usedp"    , "use+"     , "down");
	CursorMan.add("gui_mp_useup"    , "use+"     , "up"  );

	CursorMan.setDefault("default", "up");
}

bool KotOREngine::hasYavin4Module() const {
	return _platform != Aurora::kPlatformXbox || _hasLiveKey;
}

} // End of namespace KotOR

} // End of namespace Engines
