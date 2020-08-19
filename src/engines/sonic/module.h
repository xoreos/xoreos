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
 *  The context needed to run a Sonic Chronicles: The Dark Brotherhood module.
 */

#ifndef ENGINES_SONIC_MODULE_H
#define ENGINES_SONIC_MODULE_H

#include <memory>

#include "src/common/types.h"

#include "src/events/types.h"

#include "src/engines/sonic/objectcontainer.h"
#include "src/engines/sonic/object.h"

namespace Engines {

class Console;

namespace Sonic {

class Area;

/** A Sonic module. */
class Module : public Sonic::Object, public Sonic::ObjectContainer {
public:
	Module(::Engines::Console &console);
	~Module();

	/** Clear the whole context. */
	void clear();

	/** Run the module. */
	void run();
	/** Exit the module. */
	void exit();

	/** Is the module currently running? */
	bool isRunning() const;

	/** Return the area the PC is currently in. */
	Area *getCurrentArea();

	/** Move the player character to this area. */
	void movePC(int32_t area);


protected:
	::Engines::Console *_console;

	bool _running; ///< Are we currently running a module?
	bool _exit;    ///< Should we exit the module?

	int32_t _newArea; ///< The new area to enter.

	std::unique_ptr<Area> _area; ///< The current area.


	void unload();

	void loadArea();
	void unloadArea();

	void handleEvents();
	bool handleCameraEvents(const Events::Event &event);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_MODULE_H
