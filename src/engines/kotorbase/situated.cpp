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
 *  Situated object within an area in KotOR games.
 */

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/kotorbase/situated.h"

namespace Engines {

namespace KotORBase {

Situated::Situated(ObjectType type) :
		Object(type),
		_appearanceID(Aurora::kFieldIDInvalid),
		_soundAppType(Aurora::kFieldIDInvalid),
		_locked(false),
		_lastOpenedBy(0),
		_lastClosedBy(0),
		_lastUsedBy(0) {

}

void Situated::show() {
	if (_model)
		_model->show();
}

void Situated::hide() {
	if (_model)
		_model->hide();
}

bool Situated::isVisible() const {
	return _model && _model->isVisible();
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

void Situated::playAnimation(const Common::UString &anim, bool restart, float length, float speed) {
	if (_model)
		_model->playAnimation(anim, restart, length, speed);
}

void Situated::getTooltipAnchor(float &x, float &y, float &z) const {
	if (!_model) {
		Object::getTooltipAnchor(x, y, z);
		return;
	}

	_model->getTooltipAnchor(x, y, z);
}

bool Situated::isLocked() const {
	return _locked;
}

bool Situated::isKeyRequired() const {
	return _keyRequired;
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

const Common::UString &Situated::getConversation() const {
	return _conversation;
}

const Common::UString &Situated::getModelName() const {
	return _modelName;
}

const std::vector<int> Situated::getPossibleActions() const {
	std::vector<int> actions;
	if (_locked && !_keyRequired)
		actions.push_back(kActionOpenLock);

	return actions;
}

void Situated::setUsable(bool usable) {
	_usable = usable;
	if (_model)
		_model->setClickable(isClickable());
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
		warning("Situated object \"%s\" without an appearance", _tag.c_str());

	loadAppearance();
	loadSounds();

	// Model

	if (!_modelName.empty()) {
		_model.reset(loadModelObject(_modelName));

		if (!_model)
			throw Common::Exception("Failed to load situated object model \"%s\"",
			                        _modelName.c_str());
	} else
		warning("Situated object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());

	if (_model) {
		// Clickable
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		// ID
		_ids.push_back(_model->getID());
	}

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

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);

	// Locked
	_locked = gff.getBool("Locked", _locked);

	// Key required
	_keyRequired = gff.getBool("KeyRequired", _keyRequired);

	// Conversation
	_conversation = gff.getString("Conversation", _conversation);

	// Faction
	_faction = Faction(gff.getUint("FactionID"));

	// Scripts
	readScripts(gff, false);
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

} // End of namespace KotORBase

} // End of namespace Engines
