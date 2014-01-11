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

/** @file engines/kotor/situated.cpp
 *  KotOR situated object.
 */

#include "common/error.h"
#include "common/maths.h"
#include "common/util.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model_kotor.h"

#include "engines/aurora/util.h"

#include "engines/kotor/situated.h"

namespace Engines {

namespace KotOR {

Situated::Situated(ObjectType type) : Object(type), _appearanceID(Aurora::kFieldIDInvalid),
	_soundAppType(Aurora::kFieldIDInvalid), _locked(false), _model(0) {

}

Situated::~Situated() {
	destroyModel(_model);
}

void Situated::loadModel() {
	if (_model)
		return;

	if (_modelName.empty()) {
		warning("Situated object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());
		return;
	}

	try {
		_model = createWorldModel(_modelName);
	} catch (Common::Exception &e) {
		e.add("Failed to load situated object \"%s\" (\"%s\")", _tag.c_str(), _name.c_str());
		throw;
	}

	_modelIDs.push_back(_model->getID());

	if (!isStatic())
		_model->setSelectable(true);

	// Positioning
	float radian, x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(radian, x, y, z);
	setOrientation(radian, x, y, z);
}

void Situated::unloadModel() {
	setVisible(false);

	_modelIDs.clear();

	destroyModel(_model);
	_model = 0;
}

void Situated::setVisible(bool visible) {
	if (_model)
		_model->setVisible(visible);
}

void Situated::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, z, -y);
}

void Situated::setOrientation(float radian, float x, float y, float z) {
	Object::setOrientation(radian, x, y, z);
	Object::getOrientation(radian, x, y, z);

	if (_model)
		_model->setOrientation(radian, x, y, z);
}

void Situated::move(float x, float y, float z) {
	Object::move(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Situated::rotate(float radian, float x, float y, float z) {
	Object::rotate(radian, x, y, z);
	Object::getOrientation(radian, x, y, z);

	if (_model)
		_model->setOrientation(radian, x, z, -y);
}

void Situated::setHighlight(bool highlight) {
	if (_model)
		_model->showBoundingBox(highlight);

	Object::setHighlight(highlight);
}

bool Situated::isLocked() const {
	return _locked;
}

void Situated::setLocked(bool locked) {
	_locked = locked;
}

void Situated::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance


	// Specialized object properties

	if (blueprint)
		loadObject(*blueprint); // Blueprint
	loadObject(instance);    // Instance


	// Appearance

	if (_appearanceID == Aurora::kFieldIDInvalid)
		throw Common::Exception("Situated object without an appearance");

	loadAppearance();
	loadSounds();


	// Position and Orientation

	setPosition(instance.getDouble("X"), instance.getDouble("Y"), instance.getDouble("Z"));
	setOrientation(instance.getDouble("Bearing"), 0.0, 0.0, 1.0);
}

void Situated::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	if (gff.hasField("LocName")) {
		Aurora::LocString name;
		gff.getLocString("LocName", name);

		_name = name.getString();
	}

	// Description
	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Portrait
	loadPortrait(gff);

	// Appearance
	_appearanceID = gff.getUint("Appearance", _appearanceID);

	// Conversation
	_conversation = gff.getString("Conversation", _conversation);

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);

	// Locked
	_locked = gff.getBool("Locked", _locked);
}

void Situated::loadPortrait(const Aurora::GFFStruct &gff) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Situated::loadSounds() {
	if (_soundAppType == Aurora::kFieldIDInvalid)
		return;

	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeableobjsnds");

	_soundOpened    = twoda.getRow(_soundAppType).getString("Opened");
	_soundClosed    = twoda.getRow(_soundAppType).getString("Closed");
	_soundDestroyed = twoda.getRow(_soundAppType).getString("Destroyed");
	_soundUsed      = twoda.getRow(_soundAppType).getString("Used");
	_soundLocked    = twoda.getRow(_soundAppType).getString("Locked");
}

} // End of namespace KotOR

} // End of namespace Engines
