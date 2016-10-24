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
 *  Engine class handling Neverwinter Nights 2.
 */

#ifndef ENGINES_NWN2_NWN2_H
#define ENGINES_NWN2_NWN2_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"

namespace Engines {

class LoadProgress;

namespace NWN2 {

class Game;

class NWN2Engine : public Engines::Engine {
public:
	NWN2Engine();
	~NWN2Engine();

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

	bool _hasXP1; // Mask of the Betrayer (MotB)
	bool _hasXP2; // Storm of Zehir (SoZ)
	bool _hasXP3; // Mysteries of Westgate (MoW)

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

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_NWN2_H
