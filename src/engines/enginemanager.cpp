/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/enginemanager.cpp
 *  The global engine manager, omniscient about all engines
 */

#include "common/util.h"
#include "common/file.h"
#include "common/filelist.h"
#include "common/filepath.h"

#include "aurora/resman.h"
#include "aurora/error.h"

#include "engines/enginemanager.h"
#include "engines/engineprobe.h"

// The engines
#include "engines/nwn/nwn.h"
#include "engines/nwn2/nwn2.h"
#include "engines/kotor/kotor.h"
#include "engines/kotor2/kotor2.h"
#include "engines/thewitcher/thewitcher.h"

DECLARE_SINGLETON(Engines::EngineManager)

namespace Engines {

static const EngineProbe *kProbes[] = {
	&NWN::kNWNEngineProbe,
	&NWN2::kNWN2EngineProbe,
	&KotOR::kKotOREngineProbe,
	&KotOR2::kKotOR2EngineProbe,
	&TheWitcher::kTheWitcherEngineProbe
};

Aurora::GameID EngineManager::probeGameID(const std::string &target) const {
	if (Common::FilePath::isDirectory(target)) {
		// Try to probe from that directory

		Common::FileList rootFiles;

		if (!rootFiles.addDirectory(target))
			// Fatal: can't read the directory
			return Aurora::kGameIDUnknown;

		return probeGameID(target, rootFiles);
	}

	if (Common::FilePath::isRegularFile(target)) {
		// Try to probe from that file

		Common::File *file = new Common::File;
		if (file->open(target))
			return probeGameID(file);
		else
			delete file;
	}

	return Aurora::kGameIDUnknown;
}

Aurora::GameID EngineManager::probeGameID(const std::string &directory, const Common::FileList &rootFiles) const {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->probe(directory, rootFiles))
			// Found one, return the game ID
			return kProbes[i]->getGameID();

	// None found
	return Aurora::kGameIDUnknown;
}

Aurora::GameID EngineManager::probeGameID(Common::SeekableReadStream *stream) const {
	// Try to find the first engine able to handle the directory's data
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->probe(stream))
			// Found one, return the game ID
			return kProbes[i]->getGameID();

	// None found
	delete stream;
	return Aurora::kGameIDUnknown;
}

static const std::string kEmptyString;
const std::string &EngineManager::getGameName(Aurora::GameID gameID) const {
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->getGameID() == gameID)
			return kProbes[i]->getGameName();

	return kEmptyString;
}

void EngineManager::run(Aurora::GameID gameID, const std::string &target) const {
	// Try to find the first engine able to handle that game ID
	Engine *engine = 0;
	for (int i = 0; i < ARRAYSIZE(kProbes); i++)
		if (kProbes[i]->getGameID() == gameID)
			engine = kProbes[i]->createEngine();

	if (!engine)
		// None found
		throw Common::Exception("No engine handling GameID %d found", gameID);

	try {
		engine->run(target);
	} catch(...) {

		delete engine;
		// Clean up after the engine
		ResMan.clear();

		throw;
	}

	delete engine;

	// Clean up after the engine
	ResMan.clear();
}

} // End of namespace Engines
