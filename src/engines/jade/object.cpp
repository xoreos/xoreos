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
 *  An object within a Jade Empire area.
 */

#include "src/common/util.h"
#include "src/common/uuid.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/engines/jade/object.h"
#include "src/engines/jade/types.h"

namespace Engines {

namespace Jade {

Object::Object(ObjectType type) : _type(type), _conversation(""), _static(false), _usable(true),
	_active(false), _noCollide(false), _pcSpeaker(0), _area(0), _lastTriggerer(0) {
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

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isActive() const {
	return _active;
}

bool Object::isNoCollide() const {
	return _noCollide;
}

void Object::setNoCollide(bool noCollide) {
	_noCollide = noCollide;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

const std::list<uint32_t> &Object::getIDs() const {
	return _ids;
}

Aurora::NWScript::Object *Object::getPCSpeaker() const {
	return _pcSpeaker;
}

void Object::setPCSpeaker(Aurora::NWScript::Object *pc) {
	_pcSpeaker = pc;
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

void Object::enter() {
}

void Object::leave() {
}

void Object::highlight(bool UNUSED(enabled)) {
}

bool Object::click(Object *triggerer) {
	bool result = false;
	if (hasScript(kScriptOnClick))
		result = runScript(kScriptOnClick, this, triggerer);
	if (hasScript(kScriptOnUse))
		result = runScript(kScriptOnUse, this, triggerer);

	if (result)
		_lastTriggerer = triggerer;

	return result;
}

Object *Object::getLastTriggerer() const {
	return _lastTriggerer;
}

void Object::speakString(int32_t strRef) {
	// TODO: Object::speakString(): Show the string in a speech bubble

	status("<%s> \"%s\"", getName().c_str(), TalkMan.getString(strRef).c_str());
}

void Object::playAnimation(const Common::UString &animation,
                           bool UNUSED(restart), int32_t UNUSED(loopCount)) {

	warning("TODO: Object::playAnimation(\"%s\")", animation.c_str());
}

void Object::playAnimation(Animation animation) {
	warning("TODO: Object::playAnimation(%d)", (int) animation);
	// playAnimation(kAnimations[animation]);
}

void Object::loadPositional (const Aurora::GFF3Struct& gff) {

	const Aurora::GFF3Struct &positional = gff.getStruct("Positional");

	double x, y, z;

	// Position

	positional.getVector("Position", x, y, z);
	setPosition(x, y, z);

	// Orientation

	positional.getVector("Orientation", x, y, z);
	setOrientation(x, y, z, 0);
}

} // End of namespace Jade

} // End of namespace Engines
