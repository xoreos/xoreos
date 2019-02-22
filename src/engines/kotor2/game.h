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
 *  The context handling the gameplay in Star Wars: Knights of the Old Republic II - The Sith Lords.
 */

#ifndef ENGINES_KOTOR2_GAME_H
#define ENGINES_KOTOR2_GAME_H

#include <vector>

#include "src/common/ustring.h"

#include "src/sound/types.h"

#include "src/engines/kotorbase/game.h"

namespace Engines {

class Console;

namespace KotOR2 {

class KotOR2Engine;

class Functions;

class Game : public KotORBase::Game {
public:
	Game(KotOR2Engine &engine, ::Engines::Console &console, Aurora::Platform platform);
	~Game();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	void run();

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);


private:
	KotOR2Engine *_engine;

	Common::ScopedPtr<Functions> _functions;

	Aurora::Platform _platform;

	::Engines::Console *_console;

	Sound::ChannelHandle _menuMusic;


	void stopMenuMusic();
	void playMenuMusic(Common::UString music = "");

	void mainMenu();
	void runModule();
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_GAME_H
