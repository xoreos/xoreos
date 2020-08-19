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
 *  Object within an area in KotOR games.
 */

#include "external/glm/vec2.hpp"
#include "external/glm/geometric.hpp"

#include "src/common/uuid.h"
#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/location.h"

namespace Engines {

namespace KotORBase {

Object::Object(ObjectType type) :
		_type(type),
		_room(0),
		_static(false),
		_usable(true),
		_faction(kFactionInvalid),
		_currentHitPoints(0),
		_maxHitPoints(0),
		_minOneHitPoint(false) {
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

const std::list<uint32_t> &Object::getIDs() const {
	return _ids;
}

bool Object::isVisible() const {
	return false;
}

void Object::show() {
}

void Object::hide() {
}

void Object::notifyNotSeen() {
	hide();
}

const Common::UString &Object::getTemplateResRef() const {
	return _templateResRef;
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

const Room *Object::getRoom() {
	return _room;
}

void Object::setRoom(const Room *room) {
	_room = room;
}

const std::vector<int> Object::getPossibleActions() const {
	return std::vector<int>();
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

void Object::setUsable(bool usable) {
	_usable = usable;
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

Location Object::getLocation() const {
	// TODO: Object::getLocation(): Area
	// TODO: Object::getLocation(): Facing

	Location location;

	location.setArea(nullptr);
	location.setPosition(_position[0], _position[1], _position[2]);
	location.setFacing(0.0f);

	return location;
}

float Object::getDistanceTo(const Object *other) const {
	glm::vec2 origin(_position[0], _position[1]);
	glm::vec2 target(other->_position[0], other->_position[1]);
	return glm::length(target - origin);
}

int Object::getMaxHitPoints() {
	return _maxHitPoints;
}

int Object::getCurrentHitPoints() {
	return _currentHitPoints;
}

bool Object::getMinOneHitPoints() const {
	return _minOneHitPoint;
}

void Object::setMaxHitPoints(int maxHP) {
	_maxHitPoints = maxHP;
}

void Object::setCurrentHitPoints(int hitpoints) {
	if (_minOneHitPoint)
		_currentHitPoints = MIN(1, MIN(hitpoints, _maxHitPoints));
	else
		_currentHitPoints = MIN(hitpoints, _maxHitPoints);
}

void Object::setMinOneHitPoints(bool enabled) {
	_minOneHitPoint = enabled;
}

Faction Object::getFaction() const {
	return _faction;
}

void Object::setFaction(Faction faction) {
	_faction = faction;
}

bool Object::isEnemy() const {
	return
		_faction == kFactionHostile1 ||
		_faction == kFactionHostile2
		// TODO: Probably more factions can be considered an enemy.
	;
}

const Common::UString &Object::getCursor() const {
	static Common::UString cursor("");
	return cursor;
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

void Object::getTooltipAnchor(float &x, float &y, float &z) const {
	getPosition(x, y, z);
}

bool Object::isDead() const {
	return false;
}

} // End of namespace KotORBase

} // End of namespace Engines
