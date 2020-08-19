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
 *  Buffer for handling actionscript byte code.
 */

#include "src/common/uuid.h"

#include "src/events/events.h"

#include "src/engines/eclipse/externalhandler.h"

namespace Engines {

namespace Eclipse {

uint32_t ExternalHandler::setInterval(double interval, Aurora::ActionScript::IntervalFunction fun) {
	// Generate an id for accessing the timer later.
	uint32_t id = Common::generateIDNumber();

	auto intervalFun = [fun](uint32_t  currentInterval) -> uint32_t {
		fun();
		return currentInterval;
	};

	TimerMan.addTimer(interval, _timers[id], intervalFun);

	return id;
}

void ExternalHandler::clearInterval(uint32_t id) {
	TimerMan.removeTimer(_timers[id]);
}

uint32_t ExternalHandler::getTime() {
	return EventMan.getTimestamp();
}

} // End of namespace Eclipse

} // End of namespace Engines
