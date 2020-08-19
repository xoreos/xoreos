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
 *  The context handling the gameplay in Dragon Age: Origins.
 */

#ifndef ENGINES_DRAGONAGE_GAME_H
#define ENGINES_DRAGONAGE_GAME_H

#include <vector>
#include <memory>

#include "src/aurora/language.h"

#include "src/sound/types.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage/types.h"

namespace Engines {

class Console;

namespace DragonAge {

class DragonAgeEngine;

class Functions;

class Campaigns;

class Game {
public:
	Game(DragonAgeEngine &engine, ::Engines::Console &console);
	~Game();

	/** Return the campaigns context. */
	Campaigns &getCampaigns();

	void run();

	/** Load all talk tables in the current language found in this directory. */
	void loadTalkTables(const Common::UString &dir, uint32_t priority, ChangeList &res);

	/** Load all game resource archives found in this directory. */
	static void loadResources  (const Common::UString &dir, uint32_t priority, ChangeList &res);
	/** Load all texture packs found in this directory. */
	/** Load all talk tables in this language found in this directory. */
	static void loadTalkTables (const Common::UString &dir, uint32_t priority, ChangeList &res,
	                            Aurora::Language language);
	static void loadTexturePack(const Common::UString &dir, uint32_t priority, ChangeList &res,
	                            TextureQuality quality);

	/** Unload this set of talk tables. */
	static void unloadTalkTables(ChangeList &changes);


private:
	DragonAgeEngine *_engine;

	std::unique_ptr<Campaigns> _campaigns;
	std::unique_ptr<Functions> _functions;

	::Engines::Console *_console;


	void runCampaigns();

	static void loadResourceDir(const Common::UString &dir, uint32_t priority, ChangeList &changes);
	static void loadTalkTable  (const Common::UString &tlk, Aurora::Language language,
	                            uint32_t priority, ChangeList &changes);
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_GAME_H
