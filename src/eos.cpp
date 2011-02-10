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

#include "common/ustring.h"
#include "common/util.h"
#include "common/error.h"
#include "common/filepath.h"
#include "common/threads.h"

#include "common/configman.h"

#include "graphics/graphics.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/enginemanager.h"
#include "engines/gamethread.h"

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

	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	Common::UString dirArg  = argv[1];
	Common::UString baseDir = Common::FilePath::makeAbsolute(dirArg);

	if (!Common::FilePath::isDirectory(baseDir) && !Common::FilePath::isRegularFile(baseDir))
		error("No such file or directory \"%s\"", baseDir.c_str());

	atexit(deinit);

	try {
		init();

		EventMan.initMainLoop();

		Engines::GameThread gameThread;

		gameThread.init(baseDir);
		gameThread.run();

		EventMan.runMainLoop();
	} catch (Common::Exception &e) {
		Common::printException(e);
		std::exit(1);
	}

	status("Shutting down");

	// Don't clobber a broken save
	if (!configFileIsBroken)
		ConfigMan.save();

	return 0;
}

void initConfig() {
	if (!ConfigMan.load()) {
		// Loading failed, create an empty config file

		ConfigMan.create();

		// Does the config file exist per se?
		if (ConfigMan.fileExists())
			// Yes, mark it as broken
			configFileIsBroken = true;
		else
			// Nope, just write the empty file
			ConfigMan.save();
	}
}

void init() {
	Common::initThreads();

	GfxMan.init();
	status("Graphics subsystem initialized");
	SoundMan.init();
	status("Sound subsystem initialized");
	EventMan.init();
	status("Event subsystem initialized");
}

void deinit() {
	try {
		EventMan.deinit();
		SoundMan.deinit();
		GfxMan.deinit();
	} catch (...) {
	}
}
