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
 *  A waypoint in a Jade Empire area.
 */

#ifndef ENGINES_JADE_WAYPOINT_H
#define ENGINES_JADE_WAYPOINT_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/object.h"

namespace Engines {

namespace Jade {

class Waypoint : public Object {
public:
	/** Load from a waypoint instance. */
	Waypoint(const Aurora::GFF3Struct &waypoint);
	~Waypoint();

	// Basic properties

	/** Does this waypoint have a map note? */
	bool hasMapNote() const;
	/** Return the waypoint's map note text. */
	Common::UString getMapNote() const;

	/** Enable/Disable the waypoint's map note. */
	void enableMapNote(bool enabled);

	/** Enable/Disable the display of the waypoint's map note. */
	void enableMapNoteDisplay(bool enabled);

private:
	bool _hasMapNote;         ///< Does this waypoint have a map note?
	bool _displayMapNote;     ///< Does this waypoint show the map note?
	int _mapNoteType;         ///< Type of map note.
	Common::UString _mapNote; ///< The waypoint's map note text.

	/** Load from a waypoint instance. */
	void load(const Aurora::GFF3Struct &waypoint);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_WAYPOINT_H
