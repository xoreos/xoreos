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

/** @file engines/enginemanager.cpp
 *  The global engine manager, omniscient about all engines
 */

#include "common/util.h"
#include "common/ustring.h"
#include "common/file.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "aurora/error.h"
#include "aurora/resman.h"
#include "aurora/talkman.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/textureman.h"

#include "events/events.h"
#include "events/requests.h"

#include "engines/enginemanager.h"
#include "engines/engineprobe.h"

#include "engines/aurora/tokenman.h"
#include "engines/aurora/model.h"

// The engines
#include "engines/nwn/nwn.h"
#include "engines/nwn2/nwn2.h"
#include "engines/kotor/kotor.h"
#include "engines/kotor2/kotor2.h"
#include "engines/thewitcher/thewitcher.h"
#include "engines/sonic/sonic.h"
#include "engines/dragonage/dragonage.h"
#include "engines/jade/jade.h"

DECLARE_SINGLETON(Engines::EngineManager)

namespace Engines {

static const EngineProbe *kProbes[] = {
	&NWN::kNWNEngineProbe,
	&NWN2::kNWN2EngineProbe,
	&KotOR::kKotOREngineProbeWin,
	&KotOR::kKotOREngineProbeMac,
	&KotOR::kKotOREngineProbeXbox,
	&KotOR2::kKotOR2EngineProbeWin,
	&KotOR2::kKotOR2EngineProbeXbox,
	&TheWitcher::kTheWitcherEngineProbe,
	&Sonic::kSonicEngineProbe,
	&DragonAge::kDragonAgeEngineProbe,
	&Jade::kJadeEngineProbe
};

Aurora::GameID EngineManager::probeGameID(const Common::UString &target, Aurora::Platform &platform) const {
	if (Common::FilePath::isDirectory(target)) {
		// Try to probe from that directory

		Common::FileList rootFiles;

		if (!rootFiles.addDirectory(target))
			// Fatal: can't read the directory
			return Aurora::kGameIDUnknown;

		return probeGameID(target, rootFiles, platform);
	}

	if (Common::FilePath::isRegularFile(target)) {
		// Try to probe from that file

		Common::File file;
		if (file.open(target))
			return probeGameID(file, platform);
	}

	return Aurora::kGameIDUnknown;
}

Aurora::GameID EngineManager::probeGameID(const Common::UString &directory, const Common::FileList &rootFiles, Aurora::Platform &platform) const {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->probe(directory, rootFiles)) {
			// Found one, return the game ID
			platform = kProbes[i]->getPlatform();
			return kProbes[i]->getGameID();
		}

	// None found
	return Aurora::kGameIDUnknown;
}

Aurora::GameID EngineManager::probeGameID(Common::SeekableReadStream &stream, Aurora::Platform &platform) const {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->probe(stream)) {
			// Found one, return the game ID
			platform = kProbes[i]->getPlatform();
			return kProbes[i]->getGameID();
		}

	// None found
	return Aurora::kGameIDUnknown;
}

static const Common::UString kEmptyString;
const Common::UString &EngineManager::getGameName(Aurora::GameID gameID) const {
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->getGameID() == gameID)
			return kProbes[i]->getGameName();

	return kEmptyString;
}

void EngineManager::run(Aurora::GameID gameID, const Common::UString &target, Aurora::Platform platform) const {
	// Try to find the first engine able to handle that game ID
	Engine *engine = 0;
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->getGameID() == gameID && kProbes[i]->getPlatform() == platform)
			engine = kProbes[i]->createEngine();

	if (!engine)
		// None found
		throw Common::Exception("No engine handling GameID %d found", gameID);

	try {
		engine->run(target);
		EventMan.requestQuit();

		delete engine;
		// Clean up after the engine
		cleanup();

		EventMan.doQuit();

	} catch(...) {
		EventMan.requestQuit();

		delete engine;
		// Clean up after the engine
		cleanup();

		EventMan.doQuit();

		throw;
	}

}

void EngineManager::cleanup() const {
	try {
		unregisterModelLoader();

		RequestMan.sync();

		FontMan.clear();
		CursorMan.clear();
		TextureMan.clear();

		TokenMan.clear();

		TalkMan.clear();
		TwoDAReg.clear();
		ResMan.clear();

		ConfigMan.setGame();

	} catch (...) {
	}
}

} // End of namespace Engines
