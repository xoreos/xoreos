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
 *  A waypoint in a Dragon Age: Origins area.
 */

#ifndef ENGINES_DRAGONAGE_WAYPOINT_H
#define ENGINES_DRAGONAGE_WAYPOINT_H

#include "src/aurora/types.h"
#include "src/aurora/locstring.h"

#include "src/engines/dragonage/object.h"

namespace Engines {

namespace DragonAge {

class Waypoint : public Object {
public:
	/** Load from a waypoint instance. */
	Waypoint(const Aurora::GFF3Struct &waypoint);
	~Waypoint();

	// Basic properties

	/** Return the ID of the group this waypoint belongs to. */
	int32_t getGroup() const;

	/** Does this waypoint have a map note? */
	bool hasMapNote() const;
	/** Is the map note enabled? */
	bool enabledMapNote() const;

	/** Enable/Disable the waypoint's map note. */
	void enableMapNote(bool enabled);

	/** Return the waypoint's map note text. */
	const Aurora::LocString &getMapNote() const;

	/** Return the icon representing the waypoint on the map. */
	const Common::UString &getIcon() const;


private:
	/** The ID of the group this waypoint belongs to. */
	int32_t _group;

	bool _hasMapNote;     ///< Does this waypoint have a map note?
	bool _enabledMapNote; ///< Is this waypoint's map note enabled?

	/** The waypoint's map note text. */
	Aurora::LocString _mapNote;

	/** Index into the Waypoints MGDA. */
	uint32_t _type; ///< The type of the map note.

	/** The icon to show in the mini map for this waypoint. */
	Common::UString _icon;


	void load(const Aurora::GFF3Struct &waypoint);
};

} // End of namespace Dragon Age

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_WAYPOINT_H
