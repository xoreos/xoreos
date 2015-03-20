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
 *  A module.
 */

#ifndef ENGINES_JADE_MODULE_H
#define ENGINES_JADE_MODULE_H

#include "src/common/ustring.h"
#include "src/common/configman.h"

#include "src/events/types.h"

namespace Engines {

namespace Jade {

class Console;

class Area;

/** A Jade module. */
class Module {
public:
	Module(Console &console);
	~Module();

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	void load(const Common::UString &module);
	/** Run the currently loaded module. */
	void run();
	/** Exit the currently running module. */
	void exit();

	/** Is a module currently running? */
	bool isRunning() const;
	/** Return the name of the currently loaded module. */
	const Common::UString &getName() const;

	/** Return the area the PC is currently in. */
	Area *getCurrentArea();


private:
	Console *_console;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	bool _exit; //< Should we exit the module?

	Common::UString _module;    ///< The current module's name.
	Common::UString _newModule; ///< The module we should change to.

	Common::UString _areaName; ///< The name of this module's area.

	Area *_area; ///< The current module's area.


	void load();
	void loadArea();

	void unload();
	void unloadArea();

	void findAreaName();

	void enter(); ///< Enter the current module.
	void leave(); ///< Leave the current module.

	void handleEvents();

	/** Load the actual module. */
	void loadModule(const Common::UString &module);
	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module);
	/** Actually replace the currently running module. */
	void replaceModule();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_MODULE_H
