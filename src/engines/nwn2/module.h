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

#include "events/types.h"

#include "engines/nwn2/ifofile.h"

namespace Engines {

namespace NWN2 {

class Area;

class Module {
public:
	Module();
	~Module();

	/** Clear the whole context. */
	void clear();

	/** Load a module. */
	bool loadModule(const Common::UString &module);

	void run();

	const Common::UString &getName() const;

private:
	typedef std::map<Common::UString, Area *> AreaMap;

	bool _hasModule; ///< Do we have a module?

	/** Resources added by the module. */
	Aurora::ResourceManager::ChangeID _resModule;

	/** Resources added by the HAKs of the module. */
	std::vector<Aurora::ResourceManager::ChangeID> _resHAKs;

	IFOFile _ifo; ///< The module's IFO.

	bool _exit; //< Should we exit the module?

	AreaMap         _areas;       ///< The areas in the current module.
	Common::UString _newArea;     ///< The new area to enter.
	Area           *_currentArea; ///< The current area.


	void unload(); ///< Unload the whole shebang.

	void unloadModule();      ///< Unload the module.
	void unloadHAKs();        ///< Unload the HAKs required by the module.
	void unloadAreas();       ///< Unload the areas.

	void checkXPs();  ///< Do we have all expansions needed for the module?
	void checkHAKs(); ///< Do we have all HAKs needed for the module?

	void loadHAKs();        ///< Load the HAKs required by the module.
	void loadAreas();       ///< Load the areas.

	bool enter();     ///< Enter the currently loaded module.
	void enterArea(); ///< Enter a new area.

	void handleEvents();
	bool handleCamera(const Events::Event &e);

	void handleActions();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_MODULE_H
