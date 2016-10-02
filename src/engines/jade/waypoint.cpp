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

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/locstring.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/jade/waypoint.h"

namespace Engines {

namespace Jade {

Waypoint::Waypoint(const Aurora::GFF3Struct &waypoint) : Object(kObjectTypeWaypoint),
	_hasMapNote(false), _displayMapNote(false) {

	load(waypoint);
}

Waypoint::~Waypoint() {
	hide();
}

void Waypoint::load(const Aurora::GFF3Struct &waypoint) {
	// Tag

	_tag = waypoint.getString("Tag", _tag);

	loadPositional(waypoint);

	// Map note

	_hasMapNote = waypoint.getBool("MapNoteEnabled", _hasMapNote);
	_displayMapNote = waypoint.getBool("MapNoteDisplay", _displayMapNote);
	_mapNoteType = waypoint.getSint("MapNoteType");
	if (waypoint.hasField("MapNote")) {
		_mapNote = waypoint.getString("MapNote");
	}
}

bool Waypoint::hasMapNote() const {
	return _hasMapNote && _displayMapNote;
}

Common::UString Waypoint::getMapNote() const {
	return _mapNote;
}

void Waypoint::enableMapNote(bool enabled) {
	_hasMapNote = enabled;
}

void Waypoint::enableMapNoteDisplay(bool enabled) {
	_displayMapNote = enabled;
}

} // End of namespace Jade

} // End of namespace Engines
