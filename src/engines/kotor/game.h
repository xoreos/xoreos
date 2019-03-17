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
 *  The context handling the gameplay in Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_GAME_H
#define ENGINES_KOTOR_GAME_H

#include "src/engines/kotorbase/game.h"

namespace Engines {

class Console;

namespace KotOR {

class KotOREngine;
class Functions;
class Version;

class Game : public KotORBase::Game {
public:
	Game(KotOREngine &engine, Engines::Console &console, const Version &gameVersion);
	~Game();

	/** Does this module exist? */
	bool hasModule(const Common::UString &module) const override;

	void run() override;

private:
	KotOREngine *_engine;

	std::unique_ptr<Functions> _functions;

	const Version *_gameVersion;

	const Common::UString &getDefaultMenuMusic() const override;

	void collectModules();

	void mainMenu();
	void runModule();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GAME_H
