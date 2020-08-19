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
 *  Sonic Chronicles: The Dark Brotherhood (debug) console.
 */

#ifndef ENGINES_SONIC_CONSOLE_H
#define ENGINES_SONIC_CONSOLE_H

#include <set>

#include "src/common/types.h"

#include "src/engines/aurora/console.h"

namespace Engines {

namespace Sonic {

class SonicEngine;

class Console : public ::Engines::Console {
public:
	Console(SonicEngine &engine);
	~Console();


private:
	SonicEngine *_engine;

	// Caches
	std::set<int32_t> _areas; ///< All known areas.


	// Updating the caches
	void updateCaches();
	void updateAreas();

	// The commands
	void cmdListAreas(const CommandLine &cl);
	void cmdGotoArea (const CommandLine &cl);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_CONSOLE_H
