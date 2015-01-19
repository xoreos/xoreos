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

/** @file engines/nwn/waypoint.cpp
 *  NWN waypoint.
 */

#include "common/util.h"
#include "common/maths.h"

#include "aurora/locstring.h"
#include "aurora/resman.h"
#include "aurora/gfffile.h"

#include "engines/aurora/util.h"

#include "engines/nwn/waypoint.h"

namespace Engines {

namespace NWN {

Waypoint::Waypoint(const Aurora::GFFStruct &waypoint) : Object(kObjectTypeWaypoint),
	_hasMapNote(false) {

	load(waypoint);
}

Waypoint::~Waypoint() {
	hide();
}

void Waypoint::load(const Aurora::GFFStruct &waypoint) {
	Common::UString temp = waypoint.getString("TemplateResRef");

	Aurora::GFFFile *utw = 0;
	if (!temp.empty()) {
		try {
			utw = new Aurora::GFFFile(temp, Aurora::kFileTypeUTW, MKTAG('U', 'T', 'W', ' '));
		} catch (...) {
		}
	}

	load(waypoint, utw ? &utw->getTopLevel() : 0);

	delete utw;
}

bool Waypoint::hasMapNote() const {
	return _hasMapNote;
}

Common::UString Waypoint::getMapNote() const {
	return _mapNote;
}

void Waypoint::enableMapNote(bool enabled) {
	_hasMapNote = enabled;
}

void Waypoint::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	float o[3];
	Common::vector2orientation(bearingX, bearingY, o[0], o[1], o[2]);

	setOrientation(o[0], o[1], o[2]);
}

void Waypoint::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Map note

	_hasMapNote = gff.getBool("MapNoteEnabled", _hasMapNote);
	if (gff.hasField("MapNote")) {
		Aurora::LocString mapNote;
		gff.getLocString("MapNote", mapNote);

		_mapNote = mapNote.getString();
	}
}

} // End of namespace NWN

} // End of namespace Engines
