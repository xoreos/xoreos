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
 *  The context handling the gameplay in Neverwinter Nights 2.
 */

#ifndef ENGINES_NWN2_GAME_H
#define ENGINES_NWN2_GAME_H

#include <vector>
#include <memory>

#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Engines {

class Console;

namespace NWN2 {

class NWN2Engine;

class Functions;

class Campaign;
class Module;

class Game {
public:
	Game(NWN2Engine &engine, ::Engines::Console &console);
	~Game();

	/** Return the campaign context. */
	Campaign &getCampaign();
	/** Return the module context. */
	Module &getModule();
	/** Return the game difficulty setting. */
	int32_t getGameDifficulty();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	void run();

	/** Return a list of all campaigns. */
	static void getCampaigns (std::vector<Common::UString> &campaigns);
	/** Return a list of all modules. */
	static void getModules   (std::vector<Common::UString> &modules);
	/** Return a list of local player characters. */
	static void getCharacters(std::vector<Common::UString> &characters, bool local);


private:
	NWN2Engine *_engine;

	std::unique_ptr<Campaign>  _campaign;
	std::unique_ptr<Functions> _functions;

	::Engines::Console *_console;


	void runCampaign();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_GAME_H
