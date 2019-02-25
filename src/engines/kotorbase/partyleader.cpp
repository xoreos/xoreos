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
 *  Handles the party leader movement in KotOR games.
 */

#include "src/common/maths.h"

#include "src/engines/aurora/satellitecamera.h"

#include "src/engines/kotorbase/partyleader.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/area.h"

namespace Engines {

namespace KotORBase {

PartyLeaderController::PartyLeaderController(Module *module) :
		_module(module),
		_forwardMovementWanted(false),
		_backwardMovementWanted(false),
		_moving(false) {
}

void PartyLeaderController::stopMovement() {
	_forwardMovementWanted = false;
	_backwardMovementWanted = false;
	_module->getPartyLeader()->playDefaultAnimation();
	_moving = false;
}

bool PartyLeaderController::handleEvent(const Events::Event &e) {
	switch (e.type) {
		case Events::kEventKeyDown:
		case Events::kEventKeyUp:
			if (e.key.keysym.scancode == SDL_SCANCODE_W) {
				_forwardMovementWanted = (e.type == Events::kEventKeyDown);
				return true;
			} else if (e.key.keysym.scancode == SDL_SCANCODE_S) {
				_backwardMovementWanted = (e.type == Events::kEventKeyDown);
				return true;
			}
			return false;

		case Events::kEventControllerAxisMotion:
			if (e.caxis.axis == Events::kControllerAxisLeftY) {
				_forwardMovementWanted = (e.caxis.value < -10000);
				_backwardMovementWanted = (e.caxis.value > 10000);
				return true;
			}
			return false;

		default:
			return false;
	}
}

bool PartyLeaderController::processMovement(float frameTime) {
	Creature *partyLeader = _module->getPartyLeader();

	bool moveForwards = _forwardMovementWanted && !_backwardMovementWanted;
	bool moveBackwards = !_forwardMovementWanted && _backwardMovementWanted;

	if (!moveForwards && !moveBackwards) {
		if (_moving) {
			partyLeader->playDefaultAnimation();
			_moving = false;
		}
		return false;
	}

	float x, y, _;
	partyLeader->getPosition(x, y, _);
	float yaw = SatelliteCam.getYaw();
	float newX, newY;
	float moveRate = partyLeader->getRunRate();

	if (moveForwards) {
		partyLeader->setOrientation(0.0f, 0.0f, 1.0f, Common::rad2deg(yaw));
		newX = x - moveRate * sin(yaw) * frameTime;
		newY = y + moveRate * cos(yaw) * frameTime;
	} else {
		partyLeader->setOrientation(0.0f, 0.0f, 1.0f, 180 + Common::rad2deg(yaw));
		newX = x + moveRate * sin(yaw) * frameTime;
		newY = y - moveRate * cos(yaw) * frameTime;
	}

	float z = _module->getCurrentArea()->evaluateElevation(newX, newY);
	if (z != FLT_MIN) {
		if (_module->getCurrentArea()->walkable(glm::vec3(x, y, z + 0.1f),
		                                        glm::vec3(newX, newY, z + 0.1f))) {
			partyLeader->setPosition(newX, newY, z);
			_module->movedPartyLeader();
		}
	}

	if (!_moving) {
		partyLeader->playAnimation(Common::UString("run"), false, -1.0f);
		_moving = true;
	}

	return true;
}

} // End of namespace KotORBase

} // End of namespace Engines
