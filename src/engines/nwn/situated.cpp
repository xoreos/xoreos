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
 *  A situated object in a Neverwinter Nights area.
 */

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn/situated.h"

namespace Engines {

namespace NWN {

Situated::Situated(ObjectType type) : Object(type), _appearanceID(Aurora::kFieldIDInvalid),
	_soundAppType(Aurora::kFieldIDInvalid), _locked(false),
	_lastOpenedBy(0), _lastClosedBy(0), _lastUsedBy(0) {

}

Situated::~Situated() {
}

void Situated::loadModel() {
	if (_model)
		return;

	if (_modelName.empty()) {
		warning("Situated object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());
		return;
	}

	_model.reset(loadModelObject(_modelName));
	if (!_model)
		throw Common::Exception("Failed to load situated object model \"%s\"",
		                        _modelName.c_str());

	// Positioning

	float x, y, z, angle;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z, angle);
	setOrientation(x, y, z, angle);

	// Clickable

	_model->setTag(_tag);
	_model->setClickable(isClickable());

	_ids.push_back(_model->getID());
}

void Situated::unloadModel() {
	hide();

	destroyTooltip();

	_model.reset();
}

void Situated::show() {
	if (_model)
		_model->show();
}

void Situated::hide() {
	hideTooltip();

	if (_model)
		_model->hide();
}

void Situated::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Situated::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

bool Situated::isLocked() const {
	return _locked;
}

void Situated::setLocked(bool locked) {
	_locked = locked;
}

Object *Situated::getLastOpenedBy() const {
	return _lastOpenedBy;
}

Object *Situated::getLastClosedBy() const {
	return _lastClosedBy;
}

Object *Situated::getLastUsedBy() const {
	return _lastUsedBy;
}

const Common::UString &Situated::getModelName() const {
	return _modelName;
}

void Situated::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
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


	// Position

	setPosition(instance.getDouble("X"),
	            instance.getDouble("Y"),
	            instance.getDouble("Z"));

	// Orientation

	float bearing = instance.getDouble("Bearing");

	setOrientation(0.0f, 0.0f, 1.0f, Common::rad2deg(bearing));
}

void Situated::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	_name = gff.getString("LocName", _name);

	// Description
	_description = gff.getString("Description", _description);

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

	// Scripts
	readScripts(gff);
}

void Situated::loadPortrait(const Aurora::GFF3Struct &gff) {
	uint32_t portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Situated::loadSounds() {
	if (_soundAppType == Aurora::kFieldIDInvalid)
		return;

	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("placeableobjsnds");

	_soundOpened    = twoda.getRow(_soundAppType).getString("Opened");
	_soundClosed    = twoda.getRow(_soundAppType).getString("Closed");
	_soundDestroyed = twoda.getRow(_soundAppType).getString("Destroyed");
	_soundUsed      = twoda.getRow(_soundAppType).getString("Used");
	_soundLocked    = twoda.getRow(_soundAppType).getString("Locked");
}

bool Situated::createTooltip(Tooltip::Type type) {
	if (!_model)
		return false;

	if (!_tooltip) {
		_tooltip = std::make_unique<Tooltip>(type, *_model);

		_tooltip->setAlign(0.5f);
		_tooltip->setPortrait(_portrait);
	}

	return true;
}

} // End of namespace NWN

} // End of namespace Engines
