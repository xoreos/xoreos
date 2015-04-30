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
 *  KotOR (debug) console.
 */

#include <boost/bind.hpp>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/graphics/aurora/fontman.h"

#include "src/engines/kotor/console.h"
#include "src/engines/kotor/kotor.h"
#include "src/engines/kotor/module.h"

namespace Engines {

namespace KotOR {

Console::Console(KotOREngine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine) {

	registerCommand("exitmodule" , boost::bind(&Console::cmdExitModule , this, _1),
			"Usage: exitmodule\nExit the module, returning to the main menu");
	registerCommand("listmodules", boost::bind(&Console::cmdListModules, this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule" , boost::bind(&Console::cmdLoadModule , this, _1),
			"Usage: loadmodule <module>\nLoad and enter the specified module");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateModules();
}

void Console::updateModules() {
	_modules.clear();
	setArguments("loadmodule");

	Common::UString moduleDir = ConfigMan.getString("KOTOR_moduleDir");
	if (moduleDir.empty())
		return;

	Common::FileList mods;
	mods.addDirectory(moduleDir);

	for (Common::FileList::const_iterator m = mods.begin(); m != mods.end(); ++m) {
		Common::UString file = m->toLower();
		if (!file.endsWith("_s.rim"))
			continue;

		file = Common::FilePath::getStem(file);
		file.truncate(file.size() - Common::UString("_s").size());

		_modules.push_back(file);
	}

	_modules.sort(Common::UString::iless());
	setArguments("loadmodule", _modules);
}

void Console::cmdExitModule(const CommandLine &UNUSED(cl)) {
	Module *module = _engine->getModule();
	if (!module)
		return;

	hide();
	module->exit();
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
	updateModules();
	printList(_modules);
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	Module *module = _engine->getModule();
	if (!module)
		return;

	for (std::list<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			hide();
			module->load(cl.args);
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

} // End of namespace KotOR

} // End of namespace Engines
