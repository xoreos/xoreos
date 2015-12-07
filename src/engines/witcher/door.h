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
 *  A door in a The Witcher area.
 */

#ifndef ENGINES_WITCHER_DOOR_H
#define ENGINES_WITCHER_DOOR_H

#include "src/aurora/types.h"

#include "src/engines/witcher/situated.h"

namespace Engines {

namespace Witcher {

class Module;

class Waypoint;

class Door : public Situated {
public:
	/** The state of a door. */
	enum State {
		kStateClosed    = 0, ///< Closed.
		kStateOpened1   = 1, ///< Opened in one direction (normally clock-wise).
		kStateOpened2   = 2, ///< Opened in the other direction (normally counter-clock-wise).
		kStateDestroyed = 3  ///< Destroyed.
	};

	/** Load from a door instance. */
	Door(Module &module, const Aurora::GFF3Struct &door);
	~Door();

	// Basic visuals

	void show(); ///< Show the door's model.
	void hide(); ///< Hide the door's model.

	// Basic properties

	/** Is the door open? */
	bool isOpen() const;

	/** The opener object opens this door. */
	bool open(Object *opener);
	/** The closer object closes this door. */
	bool close(Object *closer);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the door.
	void leave(); ///< The cursor left the door.

	/** (Un)Highlight the door. */
	void highlight(bool enabled);

	/** The door was clicked. */
	bool click(Object *triggerer = 0);

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);

private:
	Module *_module; ///< The module the door is in.

	State _state; ///< The current state of the door.

	Common::UString _linkedTo;       ///< The waypoint tag this door links to.
	Common::UString _linkedToModule; ///< The module the waypoint this door links to is in.

	/** Load from a door instance. */
	void load(const Aurora::GFF3Struct &door);

	/** Sync the model's state with the door's state. */
	void setModelState();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_DOOR_H
