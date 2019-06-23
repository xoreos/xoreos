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
 *  Game round handler for KotOR games.
 */

#include "src/common/util.h"

#include "src/events/events.h"

#include "src/engines/kotorbase/round.h"
#include "src/engines/kotorbase/module.h"

namespace Engines {

namespace KotORBase {

const int kRoundLength = 6000; // ms

Round::Round(Module *module) : _module(module) {
}

void Round::update() {
	uint32 now = EventMan.getTimestamp();
	if (_startTimestamp + kRoundLength < now) {
		_startTimestamp = now;
		onStart();
	}
}

void Round::onStart() {
	raiseHeartbeat();
}

void Round::raiseHeartbeat() {
	_module->_runScriptVar = 2001;
	_module->_partyController.raiseHeartbeatEvent();
}

} // End of namespace KotORBase

} // End of namespace Engines
