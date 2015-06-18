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
 *  The context needed to run a Witcher module.
 */

#ifndef ENGINES_WITCHER_MODULE_H
#define ENGINES_WITCHER_MODULE_H

#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/ifofile.h"

#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/objectcontainer.h"

#include "src/events/types.h"

namespace Engines {

class Console;

namespace Witcher {

class Campaign;
class Area;

class Module : public Aurora::NWScript::Object, public Aurora::NWScript::ObjectContainer {
public:
	Module(::Engines::Console &console, Campaign *campaign = 0);
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
	const Aurora::IFOFile &getIFO() const;
	/** Return the area the PC is currently in. */
	Area *getCurrentArea();

	/** Refresh all localized strings. */
	void refreshLocalized();

	/** Move the player character to this area. */
	void movePC(const Common::UString &area);
	/** Move the player character to this position within the current area. */
	void movePC(float x, float y, float z);
	/** Move the player character to this position within this area. */
	void movePC(const Common::UString &area, float x, float y, float z);


private:
	typedef std::map<Common::UString, Area *> AreaMap;


	::Engines::Console  *_console;

	Campaign *_campaign;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	/** Resources added by the module. */
	Common::ChangeID _resModule;

	Aurora::IFOFile _ifo; ///< The module's IFO.

	bool _exit; //< Should we exit the module?

	AreaMap         _areas;           ///< The areas in the current module.
	Common::UString _newArea;         ///< The new area to enter.
	Area           *_currentArea;     ///< The current area.

	Common::UString _newModule; ///< The module we should change to.


	void unload(); ///< Unload the whole shebang.

	void unloadModule(); ///< Unload the module.
	void unloadAreas();  ///< Unload the areas.

	void loadAreas(); ///< Load the areas.

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

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_MODULE_H
