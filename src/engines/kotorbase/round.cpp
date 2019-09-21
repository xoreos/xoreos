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
 *  Round controller for KotOR games.
 */

#include "src/common/util.h"

#include "src/events/events.h"

#include "src/engines/kotorbase/round.h"
#include "src/engines/kotorbase/module.h"

namespace Engines {

namespace KotORBase {

const int kCombatRoundLength = 3000; // ms
const int kRoundLength = 2 * kCombatRoundLength; // ms

RoundController::RoundController(Module *module) :
		_module(module),
		_startTimestamp(EventMan.getTimestamp()),
		_combatRound(0) {
}

int RoundController::getNextCombatRound() const {
	return _combatRound ? 0 : 1;
}

void RoundController::update() {
	switch (_combatRound) {
		case 0:
			if (hasTimePassed(kCombatRoundLength)) {
				raiseCombatRoundEnded();
				_combatRound++;
				raiseCombatRoundBegan();
			}
			break;
		case 1:
			if (hasTimePassed(kRoundLength)) {
				raiseCombatRoundEnded();
				_startTimestamp = EventMan.getTimestamp();
				_combatRound = 0;
				raiseCombatRoundBegan();
				raiseHeartbeat();
			}
			break;
		default:
			break;
	}
}

inline bool RoundController::hasTimePassed(int ms) {
	return EventMan.getTimestamp() > _startTimestamp + ms;
}

inline void RoundController::raiseCombatRoundEnded() {
	_module->notifyCombatRoundEnded(_combatRound);
}

inline void RoundController::raiseCombatRoundBegan() {
	_module->notifyCombatRoundBegan(_combatRound);
}

void RoundController::raiseHeartbeat() {
	_module->_runScriptVar = 2001;
	_module->_partyController.raiseHeartbeatEvent();
}

} // End of namespace KotORBase

} // End of namespace Engines
