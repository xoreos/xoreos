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
 *  The context handling the gameplay in The Witcher.
 */

#ifndef ENGINES_WITCHER_GAME_H
#define ENGINES_WITCHER_GAME_H

#include <vector>

#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Engines {

class Console;

namespace Witcher {

class WitcherEngine;

class Campaign;
class Module;

class Game {
public:
	Game(WitcherEngine &engine, ::Engines::Console &console);
	~Game();

	/** Return the campaign context. */
	Campaign &getCampaign();
	/** Return the module context. */
	Module &getModule();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	/** Refresh all localized strings. */
	void refreshLocalized();

	void run();


private:
	WitcherEngine *_engine;
	Campaign      *_campaign;

	::Engines::Console *_console;


	void runCampaign();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_GAME_H
