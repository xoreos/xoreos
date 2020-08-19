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
 *  A waypoint in a Dragon Age II area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gdafile.h"

#include "src/engines/dragonage2/waypoint.h"
#include "src/engines/dragonage2/util.h"

namespace Engines {

namespace DragonAge2 {

Waypoint::Waypoint(const Aurora::GFF3Struct &waypoint) : Object(kObjectTypeWaypoint) {
	load(waypoint);
}

Waypoint::~Waypoint() {
}

int32_t Waypoint::getGroup() const {
	return _group;
}

bool Waypoint::hasMapNote() const {
	return _hasMapNote;
}

bool Waypoint::enabledMapNote() const {
	return _enabledMapNote;
}

void Waypoint::enableMapNote(bool enabled) {
	_enabledMapNote = enabled;
}

const Aurora::LocString &Waypoint::getMapNote() const {
	return _mapNote;
}

const Common::UString &Waypoint::getIcon() const {
	return _icon;
}

void Waypoint::load(const Aurora::GFF3Struct &waypoint) {
	// Tag
	_tag = waypoint.getString("Tag");

	// Group
	_group = waypoint.getSint("Group", -1);

	// Map Note
	_hasMapNote     = waypoint.getBool("HasMapNote");
	_enabledMapNote = waypoint.getBool("MapNoteEnabled");

	waypoint.getLocString("MapNote", _mapNote);

	// Type
	_type = (uint32_t) ((int32_t) waypoint.getSint("MapNoteType", -1));

	// Position
	const float position[3] = {
		(float) waypoint.getDouble("XPosition"),
		(float) waypoint.getDouble("YPosition"),
		(float) waypoint.getDouble("ZPosition")
	};

	setPosition(position[0], position[1], position[2]);

	// Orientation
	const float orientation[4] = {
		(float) waypoint.getDouble("XOrientation"),
		(float) waypoint.getDouble("YOrientation"),
		(float) waypoint.getDouble("ZOrientation"),
		(float) Common::rad2deg(acos(waypoint.getDouble("WOrientation")) * 2.0)
	};

	setOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);

	const Aurora::GDAFile &gda = getMGDA(kWorksheetWaypoints);

	// Icon
	_icon = gda.getString(_type, "Icon");

	// Variables and script
	readVarTable(waypoint);
	readScript(waypoint);
	enableEvents(true);
}

} // End of namespace DragonAge2

} // End of namespace Engines
