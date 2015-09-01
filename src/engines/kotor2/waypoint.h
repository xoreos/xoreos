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
 *  A waypoint in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#ifndef ENGINES_KOTOR2_WAYPOINT_H
#define ENGINES_KOTOR2_WAYPOINT_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/kotor2/types.h"
#include "src/engines/kotor2/object.h"

namespace Engines {

namespace KotOR2 {

class Waypoint : public Object {
public:
	/** Load from a waypoint instance. */
	Waypoint(const Aurora::GFF3Struct &waypoint);
	~Waypoint();

	// Basic properties

	/** Does this waypoint have a map note? */
	bool hasMapNote() const;
	/** Is this waypoint's map note currently enabled? */
	bool enabledMapNote() const;

	/** Enable/Disable the waypoint's map note. */
	void enableMapNote(bool enabled);

	/** Return the waypoint's map note text. */
	Common::UString getMapNote() const;

private:
	bool _hasMapNote;     ///< Does this waypoint have a map note?
	bool _enabledMapNote; ///< Is this waypoint's map note enabled?

	/** The waypoint's map note text. */
	Common::UString _mapNote;

	/** Load from a waypoint instance. */
	void load(const Aurora::GFF3Struct &waypoint);
	/** Load the waypoint from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	/** Load general waypoint properties. */
	void loadProperties(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_WAYPOINT_H
