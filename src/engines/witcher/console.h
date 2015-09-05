/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The Witcher (debug) console.
 */

#ifndef ENGINES_WITCHER_CONSOLE_H
#define ENGINES_WITCHER_CONSOLE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/engines/aurora/console.h"

namespace Engines {

namespace Witcher {

class WitcherEngine;

class Console : public ::Engines::Console {
public:
	Console(WitcherEngine &engine);
	~Console();


private:
	WitcherEngine *_engine;

	// Caches
	std::vector<Common::UString> _music;     ///< All known music resources.
	std::vector<Common::UString> _campaigns; ///< All known campaigns.
	std::vector<Common::UString> _modules;   ///< All known modules.
	std::vector<Common::UString> _areas;     ///< All known areas in the current module.

	size_t _maxSizeMusic;


	// Updating the caches
	void updateCaches();
	void updateMusic();
	void updateCampaigns();
	void updateModules();
	void updateAreas();

	// The commands
	void cmdListMusic    (const CommandLine &cl);
	void cmdStopMusic    (const CommandLine &cl);
	void cmdPlayMusic    (const CommandLine &cl);
	void cmdMove         (const CommandLine &cl);
	void cmdListAreas    (const CommandLine &cl);
	void cmdGotoArea     (const CommandLine &cl);
	void cmdListCampaigns(const CommandLine &cl);
	void cmdLoadCampaign (const CommandLine &cl);
	void cmdListModules  (const CommandLine &cl);
	void cmdLoadModule   (const CommandLine &cl);
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_CONSOLE_H
