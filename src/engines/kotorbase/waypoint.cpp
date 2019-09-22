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
 *  Waypoint within an area in KotOR games.
 */

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/waypoint.h"

namespace Engines {

namespace KotORBase {

Waypoint::Waypoint(const Aurora::GFF3Struct &waypoint) :
		Object(kObjectTypeWaypoint),
		_hasMapNote(false),
		_enabledMapNote(false) {

	load(waypoint);
}

Waypoint::~Waypoint() {
	hide();
}

void Waypoint::load(const Aurora::GFF3Struct &waypoint) {
	_templateResRef = waypoint.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utw;
	if (!_templateResRef.empty())
		utw.reset(loadOptionalGFF3(_templateResRef, Aurora::kFileTypeUTW, MKTAG('U', 'T', 'W', ' ')));

	load(waypoint, utw ? &utw->getTopLevel() : 0);
}

bool Waypoint::hasMapNote() const {
	return _hasMapNote;
}

bool Waypoint::enabledMapNote() const {
	return _hasMapNote && _enabledMapNote;
}

void Waypoint::enableMapNote(bool enabled) {
	_enabledMapNote = enabled;
}

Common::UString Waypoint::getMapNote() const {
	return _mapNote;
}

void Waypoint::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
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

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Waypoint::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Map note

	_hasMapNote     = gff.getBool("HasMapNote"    , _hasMapNote);
	_enabledMapNote = gff.getBool("MapNoteEnabled", _enabledMapNote);

	_mapNote = gff.getString("MapNote", _mapNote);

	// Scripts
	readScripts(gff);
}

} // End of namespace KotORBase

} // End of namespace Engines
