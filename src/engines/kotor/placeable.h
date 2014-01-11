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

/** @file engines/kotor/placeable.h
 *  KotOR placeable.
 */

#ifndef ENGINES_KOTOR_PLACEABLE_H
#define ENGINES_KOTOR_PLACEABLE_H

#include "aurora/types.h"

#include "engines/kotor/situated.h"

namespace Engines {

namespace KotOR {

class Placeable : public Situated {
public:
	/** The state of a placeable. */
	enum State {
		kStateDefault     = 0, ///< Default.
		kStateOpen        = 1, ///< Open.
		kStateClosed      = 2, ///< Closed.
		kStateDestroyed   = 3, ///< Destroyed.
		kStateActivated   = 4, ///< Activated.
		kStateDeactivated = 5  ///< Deactivated.
	};

	/** Load from a placeable instance. */
	Placeable(const Aurora::GFFStruct &placeable);
	~Placeable();

	/** Show/Hide the placeable's model. */
	void setVisible(bool visible);

	/** Is the placeable open? */
	bool isOpen() const;

protected:
	/** Load placeable-specific properties. */
	void loadObject(const Aurora::GFFStruct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	State _state; ///< The current state of the placeable.

	/** Load from a placeable instance. */
	void load(const Aurora::GFFStruct &placeable);

	/** Sync the model state with the placeable state. */
	void setModelState();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_PLACEABLE_H
