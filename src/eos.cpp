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

#include "sound/sound.h"

#include "events/events.h"

#include "engines/enginemanager.h"

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

	warning("Initialing the graphics subsystem");
	if (!GfxMan.init())
		error("Fatal");
	warning("Initialing the sound subsystem");
	if (!SoundMan.init())
		error("Fatal");
	warning("Initialing the event subsystem");
	if (!EventMan.init())
		error("Fatal");

	warning("Setting up graphics");
	// Initialize graphics
	if (!GfxMan.initSize(800, 600, false))
		error("Fatal");

	// Detecting an running the game

	Aurora::GameID gameID = EngineMan.probeGameID(baseDir);
	if (gameID == Aurora::kGameIDUnknown)
		error("Unable to detect the game ID");

	warning("Detected game ID %d", gameID);

	warning("Trying to run the game");

	if (!EngineMan.run(gameID, baseDir))
		error("Engine failed");

	warning("Engine returned success");
	return 0;
}

void deinit() {
	EventMan.deinit();
	SoundMan.deinit();
	GfxMan.deinit();
}
