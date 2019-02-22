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
 *  Event engine type for KotOR games.
 */

#include "src/engines/kotorbase/script/event.h"

namespace Engines {

namespace KotORBase {

Event::Event(EventType type) :
		_type(type),
		_userDefinedNumber(0) {
}

void Event::setUserDefinedNumber(int number) {
	_userDefinedNumber = number;
}

EventType Event::getType() const {
	return _type;
}

int Event::getUserDefinedNumber() const {
	return _userDefinedNumber;
}

Aurora::NWScript::EngineType *Event::clone() const {
	Event *e = new Event(_type);
	e->_userDefinedNumber = _userDefinedNumber;
	return e;
}

} // End of namespace KotORBase

} // End of namespace Engines
