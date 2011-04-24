/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
		description = EngineMan.getGameName(_gameID) + " (" +
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
