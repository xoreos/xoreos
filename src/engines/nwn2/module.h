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
 *  The context needed to run a Neverwinter Nights 2 module.
 */

#ifndef ENGINES_NWN2_MODULE_H
#define ENGINES_NWN2_MODULE_H

#include <vector>

#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/ifofile.h"

#include "src/events/types.h"

#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/object.h"

namespace Engines {

class Console;

namespace NWN2 {

class Campaign;
class Area;

class Module : public NWN2::Object, public NWN2::ObjectContainer {
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

	/** Return the IFO of the currently loaded module. */
	const Aurora::IFOFile &getIFO() const;
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


	::Engines::Console  *_console;

	Campaign *_campaign;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	/** Resources added by the module. */
	Common::ChangeID _resModule;
	/** Resources added for the custom TLK. */
	Common::ChangeID _resTLK;

	/** Resources added by the HAKs of the module. */
	std::vector<Common::ChangeID> _resHAKs;

	Aurora::IFOFile _ifo; ///< The module's IFO.

	bool _exit; //< Should we exit the module?

	AreaMap         _areas;       ///< The areas in the current module.
	Common::UString _newArea;     ///< The new area to enter.
	Area           *_currentArea; ///< The current area.

	Common::UString _newModule; ///< The module we should change to.


	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadTLK();         ///< Unload the TLK used by the module.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadAreas();       ///< Unload the areas.

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadTLK();         ///< Load the TLK used by the module.
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
