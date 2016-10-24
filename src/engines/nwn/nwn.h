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
 *  Engine class handling Neverwinter Nights.
 */

#ifndef ENGINES_NWN_NWN_H
#define ENGINES_NWN_NWN_H

#include <vector>

#include "src/common/scopedptr.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

namespace Engines {

class LoadProgress;

namespace NWN {

class Version;
class Game;

class NWNEngine : public Engines::Engine {
public:
	NWNEngine();
	~NWNEngine();

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
	Common::ScopedPtr<Version> _version;

	Aurora::Language _language;

	bool _hasXP1; // Shadows of Undrentide (SoU)
	bool _hasXP2; // Hordes of the Underdark (HotU)
	bool _hasXP3; // Kingmaker (resources also included in the final 1.69 patch)

	Common::ScopedPtr<Game> _game;


	void init();

	void detectVersion();

	void initConfig();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void declareBogusTextures();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_NWN_H
