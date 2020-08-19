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

#include "src/engines/jade/event.h"

namespace Engines {

namespace Jade {

Event::Event(Script script, int32_t eventNumber) : _script(script), _eventNumber(eventNumber) {

}

Event::~Event() {
}

Event *Event::clone() const {
	return new Event(getScript(), getEventNumber());
}

Script Event::getScript() const {
	return _script;
}

int32_t Event::getEventNumber() const {
	return _eventNumber;
}

} // End of namespace Jade

} // End of namespace Engines
