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

#include <SDL.h>

#include "common/util.h"
#include "common/filepath.h"

#include "engines/enginemanager.h"

#include "sound/sound.h"

bool initSDL();
void deinitSDL();

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	std::string baseDir = Common::FilePath::makeAbsolute((const std::string &) argv[1]);

	if (!Common::FilePath::isDirectory(baseDir))
		error("No such directory \"%s\"", baseDir.c_str());

	if (!initSDL())
		error("Fatal");

	atexit(deinitSDL);

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

bool initSDL() {
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
		warning("initSDL(): Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (!SoundMan.initMixer()) {
		warning("initSDL(): Unable to initialize audio: %s", SoundMan.getMixerError());
		return false;
	}

	return true;
}

void deinitSDL() {
	SoundMan.deinitMixer();
	SDL_Quit();
}
