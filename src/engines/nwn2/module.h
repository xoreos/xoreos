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

/** @file engines/nwn2/module.h
 *  The context needed to run a NWN2 module.
 */

#ifndef ENGINES_NWN2_MODULE_H
#define ENGINES_NWN2_MODULE_H

#include <vector>

#include "common/ustring.h"

#include "aurora/resman.h"

#include "aurora/nwscript/object.h"
#include "aurora/nwscript/objectcontainer.h"

#include "events/types.h"

#include "engines/aurora/ifofile.h"

namespace Engines {

namespace NWN2 {

class Console;
class Campaign;
class Area;

class Module : public Aurora::NWScript::Object, public Aurora::NWScript::ObjectContainer {
public:
	Module(Console &console, Campaign &campaign);
	~Module();

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	void load(const Common::UString &module);
	/** Run the currently loaded module. */
	void run();

	/** Is a module currently running? */
	bool isRunning() const;
	/** Return the name of the currently loaded module. */
	const Common::UString &getName() const;

	/** Return the IFO of the currently loaded module. */
	const IFOFile &getIFO() const;
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();

	/** Move the player character to this area. */
	void movePC(const Common::UString &area);
	/** Move the player character to this position within the current area. */
	void movePC(float x, float y, float z);
	/** Move the player character to this position within this area. */
	void movePC(const Common::UString &area, float x, float y, float z);


private:
	typedef std::map<Common::UString, Area *> AreaMap;

	Console  *_console;
	Campaign *_campaign;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	/** Resources added by the module. */
	Aurora::ResourceManager::ChangeID _resModule;

	/** Resources added by the HAKs of the module. */
	std::vector<Aurora::ResourceManager::ChangeID> _resHAKs;

	IFOFile _ifo; ///< The module's IFO.

	bool _exit; //< Should we exit the module?

	AreaMap         _areas;       ///< The areas in the current module.
	Common::UString _newArea;     ///< The new area to enter.
	Area           *_currentArea; ///< The current area.

	Common::UString _newModule; ///< The module we should change to.


	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadAreas();       ///< Unload the areas.

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadHAKs();        ///< Load the HAKs required by the module.
	void loadAreas();       ///< Load the areas.

	void enter();     ///< Enter the currently loaded module.
	void enterArea(); ///< Enter a new area.

	/** Load the actual module. */
	void loadModule(const Common::UString &module);
	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module);
	/** Actually replace the currently running module. */
	void replaceModule();

	void handleEvents();

	void handleActions();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_MODULE_H
