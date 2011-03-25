/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file eos.cpp
 *  The project's main entry point.
 */

#include <cstdio>

#include "cline.h"

#include "common/ustring.h"
#include "common/util.h"
#include "common/error.h"
#include "common/filepath.h"
#include "common/threads.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/2dareg.h"
#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "sound/sound.h"

#include "events/requests.h"
#include "events/events.h"

#include "engines/enginemanager.h"
#include "engines/gamethread.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"

void initConfig();

void init();
void deinit();

// *grumbles about Microsoft incompetence*
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int __stdcall WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,  LPSTR /*lpCmdLine*/, int /*iShowCmd*/) {
	SDL_SetModuleHandle(GetModuleHandle(NULL));
	return main(__argc, __argv);
}
#endif

static bool configFileIsBroken = false;

int main(int argc, char **argv) {
	initConfig();

	Common::UString target;
	int code;
	if (!parseCommandline(argc, argv, target, code))
		return code;

	if (target.empty()) {
		Common::UString path = ConfigMan.getString("path");
		if (path.empty())
			error("Neither a target, nor a path specified");

		target = ConfigMan.findGame(path);
		if (target.empty()) {
			target = ConfigMan.createGame(path);
			if (target.empty())
				error("Failed creating a new config target for the game");

			warning("No target specified. Creating a new target");
		} else
			warning("No target specified, but found a target with a matching path");
	}

	status("Target \"%s\"", target.c_str());
	if (!ConfigMan.setGame(target) || !ConfigMan.isInGame())
		error("No target \"%s\" in the config file", target.c_str());

	Common::UString dirArg = ConfigMan.getString("path");
	if (dirArg.empty())
		error("Target \"%s\" is missing a path", target.c_str());

	Common::UString baseDir = Common::FilePath::makeAbsolute(dirArg);
	if (!Common::FilePath::isDirectory(baseDir) && !Common::FilePath::isRegularFile(baseDir))
		error("No such file or directory \"%s\"", baseDir.c_str());

	atexit(deinit);

	Engines::GameThread *gameThread = new Engines::GameThread;
	try {
		init();

		gameThread->init(baseDir);
		gameThread->run();

		EventMan.runMainLoop();
	} catch (Common::Exception &e) {
		Common::printException(e);
		std::exit(1);
	}

	status("Shutting down");

	delete gameThread;

	// Configs changed, we should save them
	if (ConfigMan.changed()) {
		// But don't clobber a broken save
		if (!configFileIsBroken)
			ConfigMan.save();
	}

	return 0;
}

void initConfig() {
	bool newConfig = false;
	if (!ConfigMan.load()) {
		// Loading failed, create an empty config file

		ConfigMan.create();

		// Mark the config as either broken or new
		if (ConfigMan.fileExists())
			configFileIsBroken = true;
		else
			newConfig = true;
	}

	ConfigMan.setInt   (Common::kConfigRealmDefault, "width" ,     800);
	ConfigMan.setInt   (Common::kConfigRealmDefault, "height",     600);
	ConfigMan.setBool  (Common::kConfigRealmDefault, "fullscreen", false);
	ConfigMan.setInt   (Common::kConfigRealmDefault, "fsaa",       0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "gamma",    1.0);

	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume"      , 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_music", 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_sfx"  , 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_voice", 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_video", 1.0);

	ConfigMan.setBool(Common::kConfigRealmDefault, "showfps", false);

	ConfigMan.setBool(Common::kConfigRealmDefault, "skipvideos", false);

	// Populate the new config with the defaults
	if (newConfig) {
		ConfigMan.setDefaults();
		ConfigMan.save();
	}
}

void init() {
	// Init threading system
	Common::initThreads();

	// Init subsystems
	GfxMan.init();
	status("Graphics subsystem initialized");
	SoundMan.init();
	status("Sound subsystem initialized");
	EventMan.init();
	status("Event subsystem initialized");
}

void deinit() {
	// Deinit subsystems
	try {
		EventMan.deinit();
		SoundMan.deinit();
		GfxMan.deinit();
	} catch (...) {
	}

	// Destroy global singletons
	Graphics::Aurora::FontManager::destroy();
	Graphics::Aurora::CursorManager::destroy();
	Graphics::Aurora::TextureManager::destroy();

	Aurora::TalkManager::destroy();
	Aurora::TwoDARegistry::destroy();
	Aurora::ResourceManager::destroy();

	Engines::EngineManager::destroy();

	Events::EventsManager::destroy();
	Events::RequestManager::destroy();

	Sound::SoundManager::destroy();

	Graphics::GraphicsManager::destroy();

	Common::ConfigManager::destroy();
}
