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
 *  The context handling the gameplay in Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_GAME_H
#define ENGINES_KOTOR_GAME_H

#include <vector>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Engines {

class Console;

namespace KotOR {

class KotOREngine;

class Functions;

class Module;

class Game {
public:
	Game(KotOREngine &engine, ::Engines::Console &console, Aurora::Platform platform);
	~Game();

	/** Return the module context. */
	Module &getModule();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	void run();

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);


private:
	KotOREngine *_engine;

	Common::ScopedPtr<Module>    _module;
	Common::ScopedPtr<Functions> _functions;

	Aurora::Platform _platform;

	::Engines::Console *_console;

	Sound::ChannelHandle _menuMusic;


	void stopMenuMusic();
	void playMenuMusic(Common::UString music = "");

	void mainMenu();
	void runModule();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GAME_H
