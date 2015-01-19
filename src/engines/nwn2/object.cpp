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

/** @file engines/nwn2/object.h
 *  An object within a NWN2 area.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/ssffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/dlgfile.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "sound/sound.h"

#include "engines/aurora/util.h"
#include "engines/aurora/tokenman.h"

#include "engines/nwn2/types.h"
#include "engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

Object::Object(ObjectType type) : _type(type),
	_soundSet(Aurora::kFieldIDInvalid), _ssf(0), _static(false), _usable(true),
	_area(0) {

	_position   [0] = 0.0;
	_position   [1] = 0.0;
	_position   [2] = 0.0;
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

Object::~Object() {
	delete _ssf;
}

ObjectType Object::getType() const {
	return _type;
}

void Object::loadModel() {
}

void Object::unloadModel() {
}

void Object::show() {
}

void Object::hide() {
}

const Common::UString &Object::getName() const {
	return _name;
}

const Common::UString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getConversation() const {
	return _conversation;
}

const Aurora::SSFFile *Object::getSSF() {
	loadSSF();

	return _ssf;
}

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

const std::list<uint32> &Object::getIDs() const {
	return _ids;
}

Area *Object::getArea() const {
	return _area;
}

void Object::setArea(Area *area) {
	_area = area;
}

Location Object::getLocation() const {
	// TODO: Object::getLocation(): Facing

	Location location;

	location.setArea(_area);
	location.setPosition(_position[0], _position[1], _position[2]);
	location.setFacing(0.0f);

	return location;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
}

void Object::loadSSF() {
	if (_ssf || (_soundSet == Aurora::kFieldIDInvalid))
		return;

	const Aurora::TwoDAFile &soundSets = TwoDAReg.get("soundset");

	Common::UString ssfFile = soundSets.getRow(_soundSet).getString("RESREF");
	if (ssfFile.empty())
		return;

	try {
		_ssf = new Aurora::SSFFile(ssfFile);
	} catch (...) {
		warning("Failed to load SSF \"%s\" (object \"%s\")", ssfFile.c_str(), _tag.c_str());
	}
}

void Object::stopSound() {
	SoundMan.stopChannel(_sound);
}

void Object::playSound(const Common::UString &sound, bool pitchVariance) {
	stopSound();
	if (sound.empty())
		return;

	_sound = ::Engines::playSound(sound, Sound::kSoundTypeVoice, false, 1.0, pitchVariance);
}

} // End of namespace NWN2

} // End of namespace Engines
