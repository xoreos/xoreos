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
 *  Engine class handling Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_KOTOR_H
#define ENGINES_KOTOR_KOTOR_H

#include "src/common/scopedptr.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

namespace Engines {

class LoadProgress;

namespace KotOR {

class Game;

class KotOREngine : public Engines::Engine {
public:
	KotOREngine();
	~KotOREngine();

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

	bool _hasLiveKey;

	Common::ScopedPtr<Game> _game;


	bool hasYavin4Module() const;

	void init();

	void initConfig();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursorsRemap();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_KOTOR_H
