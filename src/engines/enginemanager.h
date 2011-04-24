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

/** @file engines/enginemanager.h
 *  The global engine manager, omniscient about all engines
 */

#ifndef ENGINES_ENGINEMANAGER_H
#define ENGINES_ENGINEMANAGER_H

#include "common/singleton.h"

#include "aurora/types.h"

namespace Common {
	class UString;
	class SeekableReadStream;
	class FileList;
}

namespace Engines {

/** The global engine manager. */
class EngineManager : public Common::Singleton<EngineManager> {
public:
	/** Find an engine capable of running the game found in the directory or file.
	 *
	 *  @param  target The directory or file containing game data.
	 *  @param  platform The platform found
	 *  @return A GameID of the game found in that directory or file, or kGameIDUnknown.
	 */
	Aurora::GameID probeGameID(const Common::UString &target, Aurora::Platform &platform) const;

	/** Return the full game name to that game ID. */
	const Common::UString &getGameName(Aurora::GameID gameID) const;

	/** Run the specified game found in that directory or file. */
	void run(Aurora::GameID gameID, const Common::UString &target, Aurora::Platform platform) const;

private:
	Aurora::GameID probeGameID(const Common::UString &directory, const Common::FileList &rootFiles, Aurora::Platform &platform) const;
	Aurora::GameID probeGameID(Common::SeekableReadStream &stream, Aurora::Platform &platform) const;

	void cleanup() const;
};

} // End of namespace Engines

/** Shortcut for accessing the sound manager. */
#define EngineMan Engines::EngineManager::instance()

#endif // ENGINES_ENGINEMANAGER_H
