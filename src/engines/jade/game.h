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
 *  The context handling the gameplay in Jade Empire.
 */

#ifndef ENGINES_JADE_GAME_H
#define ENGINES_JADE_GAME_H

#include <vector>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/variablecontainer.h"

namespace Engines {

class Console;

namespace Jade {

class JadeEngine;

class Functions;

class Module;

class Game : public Aurora::NWScript::VariableContainer {
public:
	Game(JadeEngine &engine, ::Engines::Console &console, Aurora::Platform platform);
	~Game();

	/** Return the module context. */
	Module &getModule();

	void run();

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);


private:
	JadeEngine *_engine;

	Common::ScopedPtr<Module>    _module;
	Common::ScopedPtr<Functions> _functions;

	Aurora::Platform _platform;

	::Engines::Console *_console;


	void mainMenu();
	void runModule();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GAME_H
