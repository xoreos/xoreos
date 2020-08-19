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
 *  Door within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_DOOR_H
#define ENGINES_KOTORBASE_DOOR_H

#include "src/aurora/types.h"

#include "src/engines/kotorbase/situated.h"

namespace Engines {

namespace KotORBase {

class Module;

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

	// Basic visuals

	/** Hide the door's model. */
	void hide();
	/** Notify the object that it is no longer seen. */
	void notifyNotSeen();

	// Basic properties

	/** Is the door open? */
	bool isOpen() const;

	/** The opener object opens this door. */
	bool open(Object *opener);
	/** The closer object closes this door. */
	bool close(Object *closer);
	/** The unlocker object unlocks this door. */
	bool unlock(Object *unlocker);

	// Object/cursor interactions

	const Common::UString &getCursor() const;

	/** (Un)Highlight the door. */
	void highlight(bool enabled);
	/** The door was clicked. */
	bool click(Object *triggerer = 0);

	// Tooltip

	void getTooltipAnchor(float &x, float &y, float &z) const;

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	enum LinkedToFlag {
		kLinkedToNothing  = 0, ///< This door links to nothing.
		kLinkedToDoor     = 1, ///< This door links to another door.
		kLinkedToWaypoint = 2  ///< This door links to a waypoint.
	};

	Module *_module; ///< The module the door is in.

	uint32_t _genericType; ///< Index into the generic door types.

	State _state; ///< The current state of the door.

	LinkedToFlag    _linkedToFlag;   ///< Does this door link to anything?
	ObjectType      _linkedToType;   ///< The type of the object this door links to.
	Common::UString _linkedTo;       ///< The object tag this door links to.
	Common::UString _linkedToModule; ///< The module the object this door links to is in.

	/** A localized string describing where this door leads to. */
	Common::UString _transitionDestination;

	/** Load from a door instance. */
	void load(const Aurora::GFF3Struct &door);

	/** Load the appearance from this 2DA row. */
	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32_t id);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_DOOR_H
