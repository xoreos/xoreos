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
 *  An object in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#include "src/common/maths.h"
#include "src/common/uuid.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/engines/kotor2/object.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

namespace Engines {

namespace KotOR2 {

Object::Object(ObjectType type)
		: _type(type),
		  _static(false),
		  _usable(true),
		  _room(0) {
	_id = Common::generateIDNumber();
	ObjectMan.registerObject(this);

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;
}

Object::~Object() {
	ObjectMan.unregisterObject(this);
}

ObjectType Object::getType() const {
	return _type;
}

void Object::show() {
}

void Object::hide() {
}

void Object::hideSoft() {
	hide();
}

bool Object::isVisible() const {
	return false;
}

const Common::UString &Object::getName() const {
	return _name;
}

const Common::UString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getPortrait() const {
	return _portrait;
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

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z, float &angle) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];

	angle = _orientation[3];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z, float angle) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = angle;
}

void Object::makeLookAt(float x, float y) {
	float dx = x - _position[0];
	float dy = y - _position[1];
	setOrientation(0.0f, 0.0f, 1.0f, Common::rad2deg(std::atan2(dy, dx)) - 90.0f);
}

void Object::makeLookAt(Object *target) {
	makeLookAt(target->_position[0], target->_position[1]);
}

Room *Object::getRoom() {
	return _room;
}

void Object::setRoom(Room *room) {
	_room = room;
}

void Object::enter() {
}

void Object::leave() {
}

void Object::highlight(bool UNUSED(enabled)) {
}

bool Object::click(Object *UNUSED(triggerer)) {
	return true;
}

void Object::stopSound() {
	SoundMan.stopChannel(_sound);
}

void Object::playSound(const Common::UString &sound, bool pitchVariance) {
	stopSound();
	if (sound.empty())
		return;

	_sound = ::Engines::playSound(sound, Sound::kSoundTypeVoice, false, 1.0f, pitchVariance);
}

void Object::playAnimation(const Common::UString &UNUSED(anim), bool UNUSED(restart), float UNUSED(length), float UNUSED(speed)) {
}

} // End of namespace KotOR2

} // End of namespace Engines
