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
 *  Engine class handling The Witcher.
 */

#ifndef ENGINES_WITCHER_WITCHER_H
#define ENGINES_WITCHER_WITCHER_H

#include <list>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

namespace Engines {

class LoadProgress;

namespace Witcher {

class Game;

class WitcherEngine : public Engines::Engine {
public:
	WitcherEngine();
	~WitcherEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languagesText,
	                     std::vector<Aurora::Language> &languagesVoice) const;

	bool getLanguage(Aurora::Language &languageText, Aurora::Language &languageVoice) const;
	bool changeLanguage();

	/** Return the context running the actual game. */
	Game &getGame();


protected:
	void run();


private:
	Aurora::Language _languageText;
	Aurora::Language _languageVoice;

	std::list<Common::ChangeID> _languageResources;
	Common::ChangeID _languageTLK;

	Common::ScopedPtr<Game> _game;


	void init();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();
	void initLua();

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language langText, Aurora::Language langVoice);
	void loadLanguageFiles(Aurora::Language langText, Aurora::Language langVoice);

	void deinit();

	void playIntroVideos();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_WITCHER_H
