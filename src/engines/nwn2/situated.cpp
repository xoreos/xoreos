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
 *  A situated object in a Neverwinter Nights 2 area.
 */

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/model_nwn2.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn2/situated.h"
#include "src/engines/nwn2/util.h"

namespace Engines {

namespace NWN2 {

Situated::Situated(ObjectType type) : Object(type), _appearanceID(Aurora::kFieldIDInvalid),
	_soundAppType(Aurora::kFieldIDInvalid), _locked(false),
	_lockable(false), _keyRequired(false), _autoRemove(false),
	_openLockDC(18), _closeLockDC(0), _currentHP(15), _baseHP(16),
	_hardness(5), _fortSave(16), _refSave(0), _willSave(0),
	_lastOpenedBy(0), _lastClosedBy(0), _lastUsedBy(0) {

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			_tint[i][j] = 1.0f;
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

	// Tinting
	if (ConfigMan.getBool("tint"))
		dynamic_cast<Graphics::Aurora::Model_NWN2 &>(*_model).setTint(_tint);

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

	_model.reset();
}

void Situated::show() {
	if (_model)
		_model->show();
}

void Situated::hide() {
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

bool Situated::isLockable() const {
	return _lockable;
}

bool Situated::isKeyRequired() const {
	return _keyRequired;
}

uint8_t Situated::getLockLockDC() const {
	return _openLockDC;
}

uint8_t Situated::getLockUnlockDC() const {
	return _closeLockDC;
}

const Common::UString &Situated::getLockKeyTag() const {
	return _keyTag;
}

const Common::UString &Situated::getKeyRequiredFeedbackMessage() const {
	return _keyFeedback;
}

void Situated::setLocked(bool locked) {
	_locked = locked;
}

void Situated::setLockable(bool lockable) {
	_lockable = lockable;
}

void Situated::setKeyRequired(bool keyRequired) {
	_keyRequired = keyRequired;
}

void Situated::setLockLockDC(uint8_t lockDC) {
	// lockDC must be between 0 and 250
	if (lockDC <= 250)
		_openLockDC = lockDC;
}

void Situated::setLockUnlockDC(uint8_t unlockDC) {
	// unlockDC must be between 0 and 250
	if (unlockDC <= 250)
		_closeLockDC = unlockDC;
}

void Situated::setLockKeyTag(const Common::UString &keyTag) {
	_keyTag = keyTag;
}

void Situated::setKeyRequiredFeedbackMessage(const Common::UString &feedback) {
	_keyFeedback = feedback;
}

int32_t Situated::getCurrentHP() const {
	return _currentHP;
}

int32_t Situated::getMaxHP() const {
	return _baseHP;
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

	float posX = instance.getDouble("X");
	float posY = instance.getDouble("Y");
	float posZ = instance.getDouble("Z");

	if (instance.hasField("Position")) {
		const Aurora::GFF3Struct &pos = instance.getStruct("Position");

		posX = pos.getDouble("x");
		posY = pos.getDouble("y");
		posZ = pos.getDouble("z");
	}

	setPosition(posX, posY, posZ);

	// Orientation

	float bearing = instance.getDouble("Bearing");

	float rotX = 0.0f;
	float rotY = 0.0f;
	float rotZ = 1.0f;
	float rotW = Common::rad2deg(bearing);

	if (instance.hasField("Orientation")) {
		const Aurora::GFF3Struct &o = instance.getStruct("Orientation");

		rotX = o.getDouble("x");
		rotY = o.getDouble("y");
		rotZ = o.getDouble("z");
		rotW = -Common::rad2deg(acos(o.getDouble("w")) * 2.0);
	}

	setOrientation(rotX, rotY, rotZ, rotW);
}

void Situated::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	_name = gff.getString("LocName", _name);

	// Description
	_description = gff.getString("Description", _description);

	// Appearance
	_appearanceID = gff.getUint("Appearance", _appearanceID);

	// Conversation
	_conversation = gff.getString("Conversation", _conversation);

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);

	// Lock and key
	_locked = gff.getBool("Locked", _locked);
	_lockable = gff.getBool("Lockable", _lockable);
	_keyRequired = gff.getBool("KeyRequired", _keyRequired);
	_autoRemove = gff.getBool("AutoRemoveKey", _autoRemove);

	_openLockDC = gff.getUint("OpenLockDC", _openLockDC);
	_closeLockDC = gff.getUint("CloseLockDC", _closeLockDC);

	_keyTag = gff.getString("KeyName", _keyTag);
	_keyFeedback = gff.getString("KeyReqFeedback", _keyFeedback);

	// Defenses
	_currentHP = gff.getUint("CurrentHP", _currentHP);
	_baseHP = gff.getUint("HP", _baseHP);
	_hardness = gff.getUint("Hardness", _hardness);
	_fortSave = gff.getSint("Fort", _fortSave);
	_refSave = gff.getSint("Ref", _refSave);
	_willSave = gff.getSint("Will", _willSave);

	// Tint
	readTint(gff, _tint);

	// Scripts and variables
	readScripts(gff);
	readVarTable(gff);
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

} // End of namespace NWN2

} // End of namespace Engines
