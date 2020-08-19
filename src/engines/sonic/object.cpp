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
 *  An object in a Sonic Chronicles: The Dark Brotherhood area.
 */

#include "src/engines/sonic/object.h"

namespace Engines {

namespace Sonic {

Object::Object(ObjectType type) : _type(type), _modelID(0xFFFFFFFF) {
	_id = 0xFFFFFFFF;

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;
}

Object::~Object() {
}

ObjectType Object::getType() const {
	return _type;
}

void Object::show() {
}

void Object::hide() {
}

uint32_t Object::getModelID() const {
	return _modelID;
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

} // End of namespace Sonic

} // End of namespace Engines
