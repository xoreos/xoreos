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
 *  The context handling the gameplay in Neverwinter Nights.
 */

#ifndef ENGINES_NWN_GAME_H
#define ENGINES_NWN_GAME_H

#include <vector>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Engines {

class Console;

namespace NWN {

class NWNEngine;
class Version;

class Functions;

class Module;

class Game {
public:
	Game(NWNEngine &engine, ::Engines::Console &console, const Version &version);
	~Game();

	const Version &getVersion() const;

	/** Return the module context. */
	Module &getModule();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	void run();

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);
	/** Return a list of all premium modules. */
	static void getPremiumModules(std::vector<Common::UString> &modules);
	/** Do we actually have any premium modules installed at all? */
	static bool hasPremiumModules();
	/** Is this module file (including extension) a premium module? */
	static bool isPremiumModule(const Common::UString &module);
	/** Does this module exist? */
	static bool hasModule(Common::UString &module);
	/** Return a list of local player characters. */
	static void getCharacters(std::vector<Common::UString> &characters, bool local);


private:
	NWNEngine *_engine;

	Common::ScopedPtr<Module>    _module;
	Common::ScopedPtr<Functions> _functions;

	::Engines::Console *_console;

	const Version *_version;

	Sound::ChannelHandle _menuMusic;


	void stopMenuMusic();
	void playMenuMusic(Common::UString music = "");

	void mainMenu(bool playStartSound, bool showLegal);
	void runModule();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GAME_H
