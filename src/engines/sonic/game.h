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
 *  The context handling the gameplay in Sonic Chronicles: The Dark Brotherhood.
 */

#ifndef ENGINES_SONIC_GAME_H
#define ENGINES_SONIC_GAME_H

#include "src/common/scopedptr.h"

#include "src/graphics/aurora/fonthandle.h"

namespace Engines {

class Console;

namespace Sonic {

class SonicEngine;
class Module;

class Game {
public:
	Game(SonicEngine &engine, ::Engines::Console &console);
	~Game();

	/** Return the module context. */
	Module &getModule();

	void run();

private:
	SonicEngine *_engine;

	Common::ScopedPtr<Module> _module;

	::Engines::Console *_console;

	Graphics::Aurora::FontHandle _guiFont;
	Graphics::Aurora::FontHandle _quoteFont;


	bool waitClick();

	bool showLicenseSplash();
	bool showTitle();
	bool showMainMenu();
	bool showQuote();
	bool showChapter1();

	void runModule();
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_GAME_H
