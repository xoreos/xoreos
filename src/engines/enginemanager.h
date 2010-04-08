/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/enginemanager.h
 *  The global engine manager, omniscient about all engines
 */

#ifndef ENGINES_ENGINEMANAGER_H
#define ENGINES_ENGINEMANAGER_H

#include <string>

#include "common/singleton.h"

#include "aurora/types.h"

namespace Engines {

/** The global engine manager. */
class EngineManager : public Common::Singleton<EngineManager> {
public:
	/** Find an engine capable of running the game found in the directory.
	 *
	 *  @param  directory The directory containing game data.
	 *  @return A GameID of the game found in that directory, or kGameIDUnknown.
	 */
	Aurora::GameID probeGameID(const std::string &directory) const;

	/** Return the full game name to that game ID. */
	const std::string &getGameName(Aurora::GameID gameID) const;

	/** Run the specified game found in that directory. */
	bool run(Aurora::GameID gameID, const std::string &directory) const;
};

} // End of namespace Engines

/** Shortcut for accessing the sound manager. */
#define EngineMan Engines::EngineManager::instance()

#endif // ENGINES_ENGINEMANAGER_H
