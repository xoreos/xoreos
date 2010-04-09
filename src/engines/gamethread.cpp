/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/gamethread.cpp
 *  The thread the game logic runs in.
 */

#include "common/util.h"

#include "engines/gamethread.h"
#include "engines/enginemanager.h"

#include "graphics/graphics.h"

namespace Engines {

GameThread::GameThread() {
	_gameID = Aurora::kGameIDUnknown;
}

GameThread::~GameThread() {
}

bool GameThread::init(const std::string &baseDir) {
	// Detecting the game

	_baseDir = baseDir;

	_gameID = EngineMan.probeGameID(_baseDir);

	if (_gameID == Aurora::kGameIDUnknown) {
		warning("Unable to detect the game ID");
		return false;
	}

	// Set the window title to our and the detected game's name
	GfxMan.setWindowTitle(PACKAGE_STRING " -- " + EngineMan.getGameName(_gameID));

	status("Detected game ID %d -- %s", _gameID, EngineMan.getGameName(_gameID).c_str());

	return true;
}

bool GameThread::run() {
	if (!createThread()) {
		warning("Failed creating game logic thread");
		return false;
	}

	return true;
}

void GameThread::threadMethod() {
	EngineMan.run(_gameID, _baseDir);
}

} // End of namespace Engines
