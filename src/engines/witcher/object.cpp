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
 *  An object in a The Witcher area.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/uuid.h"

#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/witcher/object.h"

namespace Engines {

namespace Witcher {

Object::Object(ObjectType type) : _type(type),
	_static(false), _usable(true), _area(0) {
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

const Common::UString &Object::getUniqueID() const {
	return _uniqueID;
}

const Aurora::LocString &Object::getName() const {
	return _name;
}

const Aurora::LocString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getConversation() const {
	return _conversation;
}

void Object::refreshLocalized() {
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

const std::list<uint32_t> &Object::getIDs() const {
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

void Object::speakString(const Common::UString &string, uint32_t UNUSED(volume)) {
	// TODO: Object::speakString(): Show the string in a speech bubble

	status("<%s> \"%s\"", getName().getString().c_str(), string.c_str());
}

void Object::speakOneLiner(Common::UString conv, Object *UNUSED(tokenTarget)) {
	if (conv.empty())
		conv = _conversation;
	if (conv.empty())
		return;

	Common::UString text;
	Common::UString sound;


	try {
		Aurora::DLGFile dlg(conv, this);

		const Aurora::DLGFile::Line *line = dlg.getOneLiner();
		if (line) {
			text  = line->text.getString();
			sound = line->sound;
		}

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed evaluating one-liner from conversation \"%s\"", conv.c_str());
	}

	if (!text.empty())
		speakString(text, 0);
	if (!sound.empty())
		playSound(sound);
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

bool Object::click(Object *UNUSED(triggerer)) {
	return true;
}

} // End of namespace Witcher

} // End of namespace Engines
