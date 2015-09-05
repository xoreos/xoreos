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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords (debug) console.
 */

#ifndef ENGINES_KOTOR2_CONSOLE_H
#define ENGINES_KOTOR2_CONSOLE_H

#include <vector>

#include "src/engines/aurora/console.h"

namespace Engines {

namespace KotOR2 {

class KotOR2Engine;

class Console : public ::Engines::Console {
public:
	Console(KotOR2Engine &engine);
	~Console();


private:
	KotOR2Engine *_engine;

	// Caches
	std::vector<Common::UString> _modules; ///< All known modules.
	std::vector<Common::UString> _music;   ///< All known music resources.

	size_t _maxSizeMusic;


	// Updating the caches
	void updateCaches();
	void updateModules();
	void updateMusic();

	// The commands
	void cmdExitModule (const CommandLine &cl);
	void cmdListModules(const CommandLine &cl);
	void cmdLoadModule (const CommandLine &cl);
	void cmdListMusic  (const CommandLine &cl);
	void cmdStopMusic  (const CommandLine &cl);
	void cmdPlayMusic  (const CommandLine &cl);
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_CONSOLE_H
