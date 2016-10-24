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
 *  Generic engine interface.
 */

#ifndef ENGINES_ENGINE_H
#define ENGINES_ENGINE_H

#include <vector>

#include <boost/noncopyable.hpp>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/language.h"

namespace Graphics {
	namespace Aurora {
		class FPS;
	}
}

namespace Engines {

class Console;

/** The base class for an engine within BioWare's Aurora family. */
class Engine : boost::noncopyable {
public:
	Engine();
	virtual ~Engine();

	/** Detect which languages this game instance supports. */
	virtual bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                             Aurora::Platform platform,
	                             std::vector<Aurora::Language> &languages) const;

	/** Detect which languages this game instance supports. */
	virtual bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                             Aurora::Platform platform,
	                             std::vector<Aurora::Language> &languagesText,
	                             std::vector<Aurora::Language> &languagesVoice) const;

	bool detectLanguages(std::vector<Aurora::Language> &languages) const;
	bool detectLanguages(std::vector<Aurora::Language> &languagesText,
	                     std::vector<Aurora::Language> &languagesVoice) const;

	/** Return the game's current language. */
	virtual bool getLanguage(Aurora::Language &language) const;
	/** Return the game's current language. */
	virtual bool getLanguage(Aurora::Language &languageText, Aurora::Language &languageVoice) const;

	/** Change the game's current language. */
	virtual bool changeLanguage();

	void start(Aurora::GameID game, const Common::UString &target, Aurora::Platform platform);

	/** Evaluate the FPS display setting and show/hide the FPS display. */
	void showFPS();

protected:
	Aurora::GameID   _game;
	Aurora::Platform _platform;
	Common::UString  _target;

	Common::ScopedPtr<Console> _console;

	Common::ScopedPtr<Graphics::Aurora::FPS> _fps;


	/** Run the game. */
	virtual void run() = 0;

	bool evaluateLanguage(bool find, Aurora::Language &language) const;
	bool evaluateLanguage(bool find, Aurora::Language &languageVoice, Aurora::Language &languageText) const;
};

} // End of namespace Engines

#endif // ENGINES_ENGINE_H
