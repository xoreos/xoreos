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
 *  Creature action in Star Wars: Knights of the Old Republic.
 */

#include "src/engines/kotor/action.h"

namespace Engines {

namespace KotOR {

Action::Action(ActionType type) : _type(type), _object(0), _range(0.0f) {
	_point[0] = _point[1] = _point[2] = 0.0f;
}

Action::Action(const Action &action) : _type(action._type) {
	_point[0] = action._point[0];
	_point[1] = action._point[1];
	_point[2] = action._point[2];

	_object = action._object;
	_range = action._range;
}

void Action::setPoint(float x, float y, float z) {
	_point[0] = x;
	_point[1] = y;
	_point[2] = z;
}

void Action::setObject(Object *o) {
	_object = o;
}

void Action::setRange(float range) {
	_range = range;
}

ActionType Action::getType() const {
	return _type;
}

void Action::getPoint(float &x, float &y, float &z) const {
	x = _point[0];
	y = _point[1];
	z = _point[2];
}

Object *Action::getObject() const {
	return _object;
}

float Action::getRange() const {
	return _range;
}

} // End of namespace KotOR

} // End of namespace Engines
