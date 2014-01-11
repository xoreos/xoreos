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

/** @file engines/kotor/door.h
 *  KotOR door.
 */

#ifndef ENGINES_KOTOR_DOOR_H
#define ENGINES_KOTOR_DOOR_H

#include "aurora/types.h"

#include "engines/kotor/situated.h"

namespace Engines {

namespace KotOR {

class Door : public Situated {
public:
	/** The state of a door. */
	enum State {
		kStateClosed  = 0, ///< Closed.
		kStateOpened1 = 1, ///< Opened in one direction (normally clock-wise).
		kStateOpened2 = 2  ///< Opened in the other direction (normally counter-clock-wise).
	};

	/** Load from a door instance. */
	Door(const Aurora::GFFStruct &door);
	~Door();

	/** Show/Hide the door's model. */
	void setVisible(bool visible);

	/** Is the door open? */
	bool isOpen() const;

	/** The opener object opens this door. */
	bool open(Object *opener);
	/** The closer object closes this door. */
	bool close(Object *closer);

	/** Lock/Unlock the door. */
	void setLocked(bool locked);

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFFStruct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	bool _invisible; ///< Is the door invisible?

	uint32 _genericType; ///< Index into the generic door types.

	State _state; ///< The current state of the door.

	/** Load from a door instance. */
	void load(const Aurora::GFFStruct &door);

	/** Load the appearance from this 2DA row. */
	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id);

	/** Sync the model state with the door state. */
	void setModelState();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_DOOR_H
