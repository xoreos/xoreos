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
 *  Engine class handling Dragon Age: Origins.
 */

#ifndef ENGINES_DRAGONAGE_DRAGONAGE_H
#define ENGINES_DRAGONAGE_DRAGONAGE_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

#include "src/engines/aurora/resources.h"

namespace Engines {

class LoadProgress;

namespace DragonAge {

class Game;

class DragonAgeEngine : public Engines::Engine {
public:
	DragonAgeEngine();
	~DragonAgeEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	/** Return the context running the actual game. */
	Game &getGame();

	/** Return the string Dragon Age: Origins uses to representing this language. */
	static Common::UString getLanguageString(Aurora::Language language);

protected:
	void run();


private:
	Aurora::Language _language;

	ChangeList _resources;
	ChangeList _languageTLK;

	Common::ScopedPtr<Game> _game;


	void init();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void deinit();

	void playIntroVideos();
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_DRAGONAGE_H
