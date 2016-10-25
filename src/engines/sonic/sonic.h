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
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood.
 */

#ifndef ENGINES_SONIC_SONIC_H
#define ENGINES_SONIC_SONIC_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

namespace Engines {

class LoadProgress;

namespace Sonic {

class Game;

class SonicEngine : public Engines::Engine {
public:
	SonicEngine();
	~SonicEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	/** Return the context running the actual game. */
	Game &getGame();


protected:
	void run();


private:
	Aurora::Language _language;

	Common::ChangeID _languageHERF;
	Common::ChangeID _languageTLK;

	Common::ScopedPtr<Game> _game;


	void init();
	void declareLanguages();
	void declareResources();
	void initResources(LoadProgress &progress);
	void initConfig();
	void initGameConfig();

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language language);
	void loadLanguageFiles(Aurora::Language language);

	void deinit();


	static Common::UString getLanguageHERF(Aurora::Language language);
	static Common::UString getLanguageTLK (Aurora::Language language);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_SONIC_H
