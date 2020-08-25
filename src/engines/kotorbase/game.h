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
 *  The context handling the gameplay in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GAME_H
#define ENGINES_KOTORBASE_GAME_H

#include <vector>
#include <memory>

#include "src/common/ustring.h"

#include "src/sound/types.h"

#include "src/engines/kotorbase/module.h"

namespace Engines {

class Console;

namespace KotORBase {

class Game {
public:
	Game(Engines::Console &console);
	virtual ~Game() = default;

	// Modules

	/** Return a list of all modules. */
	const std::vector<Common::UString> &getModules() const { return _modules; }
	/** Does this module exist? */
	virtual bool hasModule(const Common::UString &module) const = 0;

	// Music

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();


	/** Return the module context. */
	KotORBase::Module &getModule();

	virtual void run() = 0;

protected:
	Engines::Console *_console;
	std::unique_ptr<KotORBase::Module> _module;
	std::vector<Common::UString> _modules;
	Sound::ChannelHandle _menuMusic;

	virtual const Common::UString &getDefaultMenuMusic() const = 0;

private:
	void stopMenuMusic();
	void playMenuMusic(Common::UString music = "");
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GAME_H
