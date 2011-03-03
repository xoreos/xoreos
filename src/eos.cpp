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
#include "init.h"

#include "common/ustring.h"
#include "common/util.h"
#include "common/error.h"
#include "common/filepath.h"

#include "common/configman.h"

#include "events/events.h"

#include "engines/gamethread.h"

void initConfig();

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

	if (!target.empty()) {
		status("Target \"%s\"", target.c_str());
		if (!ConfigMan.setGame(target))
			error("No target \"%s\" in the config file", target.c_str());
	} else
		warning("No target specified, probing");

	Common::UString dirArg;
	if (!ConfigMan.getKey("path", dirArg)) {
		warning("No game path specified, using the current directory");
		dirArg = ".";
	}

	Common::UString baseDir = Common::FilePath::makeAbsolute(dirArg);

	if (!Common::FilePath::isDirectory(baseDir) && !Common::FilePath::isRegularFile(baseDir))
		error("No such file or directory \"%s\"", baseDir.c_str());

	atexit(deinit);

	try {
		init();

		Engines::GameThread gameThread;

		gameThread.init(baseDir);
		gameThread.run();

		EventMan.runMainLoop();
	} catch (Common::Exception &e) {
		Common::printException(e);
		std::exit(1);
	}

	status("Shutting down");

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

	// Populate the new config with the defaults
	if (newConfig) {
		ConfigMan.setDefaults();
		ConfigMan.save();
	}
}
