/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "common/util.h"
#include "common/filepath.h"

#include "graphics/graphics.h"
#include "graphics/cube.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/enginemanager.h"
#include "engines/gamethread.h"

void init();
void deinit();

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	std::string baseDir = Common::FilePath::makeAbsolute((const std::string &) argv[1]);

	if (!Common::FilePath::isDirectory(baseDir))
		error("No such directory \"%s\"", baseDir.c_str());

	atexit(deinit);

	init();

	if (!EventMan.initMainLoop())
		error("Failed initializing the main loop");

	Engines::GameThread gameThread;

	if (!gameThread.init(baseDir))
		error("Failed initializing the game thread");

	if (!gameThread.run())
		error("Failed running the game thread");

	EventMan.runMainLoop();

	status("Shutting down");
	return 0;
}

void init() {
	status("Initializing the graphics subsystem");
	if (!GfxMan.init())
		error("Initializing the graphics subsystem failed");
	status("Initializing the sound subsystem");
	if (!SoundMan.init())
		error("Initializing the sound subsystem failed");
	status("Initializing the event subsystem");
	if (!EventMan.init())
		error("Initializing the event subsystem failed");
}

void deinit() {
	EventMan.deinit();
	SoundMan.deinit();
	GfxMan.deinit();
}
