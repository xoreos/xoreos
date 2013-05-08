/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/door.h
 *  NWN door.
 */

#ifndef ENGINES_NWN_DOOR_H
#define ENGINES_NWN_DOOR_H

#include "aurora/types.h"

#include "engines/nwn/situated.h"

namespace Engines {

namespace NWN {

class Module;

class Waypoint;

class Door : public Situated {
public:
	/** The state of a door. */
	enum State {
		kStateClosed  = 0, ///< Closed.
		kStateOpened1 = 1, ///< Opened in one direction (normally clock-wise).
		kStateOpened2 = 2  ///< Opened in the other direction (normally counter-clock-wise).
	};

	/** Load from a door instance. */
	Door(Module &module, const Aurora::GFFStruct &door);
	~Door();


	void show(); ///< Show the door's model.
	void hide(); ///< Hide the door's model.


	void enter(); ///< The cursor entered the door.
	void leave(); ///< The cursor left the door.

	/** (Un)Highlight the door. */
	void highlight(bool enabled);

	/** The door was clicked. */
	bool click(Object *triggerer = 0);


	/** Is the door open? */
	bool isOpen() const;

	/** The opener object opens this door. */
	bool open(Object *opener);
	/** The closer object closes this door. */
	bool close(Object *closer);

	/** Lock/Unlock the door. */
	void setLocked(bool locked);

	/** Play a door animation. */
	void playAnimation(Animation animation);
	virtual void playAnimation(const Common::UString &animation = "", bool restart = true, int32 loopCount = 0) { Situated::playAnimation(animation, restart, loopCount); }

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFFStruct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	enum LinkedToFlag {
		kLinkedToNothing  = 0, ///< This door links to nothing.
		kLinkedToDoor     = 1, ///< This door links to another door.
		kLinkedToWaypoint = 2  ///< This door links to a waypoint.
	};

	Module *_module; ///< The module the door is in.

	bool _invisible; ///< Is the door invisible?

	uint32 _genericType; ///< Index into the generic door types.

	State _state; ///< The current state of the door.

	LinkedToFlag _linkedToFlag; ///< Does this door link to anything?
	Common::UString _linkedTo;  ///< The object tag this door links to.

	bool _evaluatedLink;       ///< Have we already tried to evaluate our link?
	Object   *_link;           ///< The object this door links to.
	Door     *_linkedDoor;     ///< The door this door links to.
	Waypoint *_linkedWaypoint; ///< The waypoint this door links to.

	/** Load from a door instance. */
	void load(const Aurora::GFFStruct &door);

	/** Load the appearance from this 2DA row. */
	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id);

	/** Sync the model's state with the door's state. */
	void setModelState();
	/** Evaluate our link. */
	void evaluateLink();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_DOOR_H
