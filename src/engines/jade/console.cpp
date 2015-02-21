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
 *  Jade (debug) console.
 */

#include <boost/bind.hpp>

#include "common/ustring.h"
#include "common/util.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "graphics/aurora/fontman.h"

#include "engines/jade/console.h"
#include "engines/jade/module.h"

namespace Engines {

namespace Jade {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_module(0) {

	registerCommand("exitmodule" , boost::bind(&Console::cmdExitModule , this, _1),
			"Usage: exitmodule\nExit the module, returning to the main menu");
	registerCommand("listmodules", boost::bind(&Console::cmdListModules, this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule" , boost::bind(&Console::cmdLoadModule , this, _1),
			"Usage: loadmodule <module>\nLoad and enter the specified module");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateModules();
}

void Console::updateModules() {
	_modules.clear();
	setArguments("loadmodule");

	Common::UString modulesDir = ConfigMan.getString("JADE_moduleDir");
	if (modulesDir.empty())
		return;

	Common::FileList files;
	files.addDirectory(modulesDir, -1);

	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f) {
		Common::UString file = Common::FilePath::relativize(modulesDir, *f).toLower();
		if (!file.endsWith(".rim"))
			continue;

		Common::UString dir = Common::FilePath::getDirectory(file).c_str();
		if (dir.empty())
			continue;

		if (std::find(_modules.begin(), _modules.end(), dir) != _modules.end())
			continue;

		_modules.push_back(dir);
	}

	_modules.sort(Common::UString::iless());
	setArguments("loadmodule", _modules);
}

void Console::cmdExitModule(const CommandLine &UNUSED(cl)) {
	_module->exit();
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
	updateModules();
	printList(_modules);
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (!_module)
		return;

	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	for (std::list<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			_module->load(cl.args);
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

} // End of namespace Jadd

} // End of namespace Engines
