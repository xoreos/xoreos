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
 *  Dragon Age: Origins (debug) console.
 */

#ifndef ENGINES_DRAGONAGE_CONSOLE_H
#define ENGINES_DRAGONAGE_CONSOLE_H

#include "src/engines/aurora/console.h"

namespace Engines {

namespace DragonAge {

class DragonAgeEngine;

class Console : public ::Engines::Console {
public:
	Console(DragonAgeEngine &engine);
	~Console();

private:
	DragonAgeEngine *_engine;


	// Updating the caches
	void updateCaches();
	void updateAreas();
	void updateCampaigns();

	void cmdListAreas    (const CommandLine &cl);
	void cmdLoadArea     (const CommandLine &cl);
	void cmdListCampaigns(const CommandLine &cl);
	void cmdLoadCampaign (const CommandLine &cl);

};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_CONSOLE_H
