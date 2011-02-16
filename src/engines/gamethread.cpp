/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
#include "common/error.h"
#include "common/configman.h"

#include "../aurora/util.h"

#include "engines/gamethread.h"
#include "engines/enginemanager.h"

#include "graphics/graphics.h"

namespace Engines {

GameThread::GameThread() {
	_gameID = Aurora::kGameIDUnknown;
	_platform = Aurora::kPlatformUnknown;
}

GameThread::~GameThread() {
}

void GameThread::init(const Common::UString &baseDir) {
	// Detecting the game

	_baseDir = baseDir;

	_gameID = EngineMan.probeGameID(_baseDir, _platform);

	if (_gameID == Aurora::kGameIDUnknown)
		throw Common::Exception("Unable to detect the game ID");

	// Get the game description from the config, or alternatively
	// construct one from the game name and platform.
	Common::UString description;
	if (!ConfigMan.getKey("description", description))
		description = EngineMan.getGameName(_gameID) + " ( " +
		              Aurora::getPlatformDescription(_platform) + ")";

	GfxMan.setWindowTitle(Common::UString(PACKAGE_STRING " -- ") + description);

	status("Detected game ID %d -- %s", _gameID, EngineMan.getGameName(_gameID).c_str());
}

void GameThread::run() {
	if (!createThread())
		throw Common::Exception("Failed creating game logic thread");
}

void GameThread::threadMethod() {
	try {
		EngineMan.run(_gameID, _baseDir, _platform);
	} catch (Common::Exception &e) {
		Common::printException(e);
	}
}

} // End of namespace Engines
