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
 *  A scripting event.
 */

#include "src/engines/dragonage2/event.h"
#include "src/engines/dragonage2/object.h"

namespace Engines {

namespace DragonAge2 {

Event::Event(EventType type, Aurora::NWScript::Object *creator, Aurora::NWScript::Object *target) :
	_type(type), _creator(creator), _target(target) {

}

Event::~Event() {
}

Event *Event::clone() const {
	return new Event(*this);
}

EventType Event::getType() const {
	return _type;
}

void Event::setType(EventType type) {
	_type = type;
}

Aurora::NWScript::Object *Event::getCreator() const {
	return _creator;
}

void Event::setCreator(Aurora::NWScript::Object *creator) {
	_creator = creator;
}

Aurora::NWScript::Object *Event::getTarget() const {
	return _target;
}

void Event::setTarget(Aurora::NWScript::Object *target) {
	_target = target;
}

} // End of namespace DragonAge2

} // End of namespace Engines
