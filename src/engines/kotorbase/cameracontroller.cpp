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
 *  Handles camera movement in KotOR games.
 */

#include "src/common/maths.h"

#include "src/graphics/camera.h"

#include "src/engines/aurora/flycamera.h"

#include "src/engines/kotorbase/cameracontroller.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/area.h"

namespace Engines {

namespace KotORBase {

static const float kRotationSpeed = M_PI / 2.f;

CameraController::CameraController(Module *module) : _module(module) {
}

bool CameraController::isFlyCamera() const {
	return _flycam;
}

void CameraController::toggleFlyCamera() {
	_flycam = !_flycam;
}

float CameraController::getYaw() const {
	return _yaw;
}

void CameraController::setYaw(float value) {
	_yaw = value;
	_dirty = true;
}

void CameraController::updateTarget() {
	float x, y, z;
	Creature *partyLeader = _module->getPartyLeader();

	partyLeader->getPosition(x, y, z);

	_target = glm::vec3(x, y, z + partyLeader->getCameraHeight());
	_dirty = true;
}

void CameraController::updateCameraStyle() {
	_module->getCurrentArea()->getCameraStyle(_distance, _pitch, _height);
	_dirty = true;
}

bool CameraController::handleEvent(const Events::Event &e) {
	if (_flycam)
		return FlyCam.handleCameraInput(e);

	switch (e.type) {
		case Events::kEventKeyDown:
		case Events::kEventKeyUp:
			switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_A:
					_counterClockwiseMovementWanted = (e.type == Events::kEventKeyDown);
					return true;

				case SDL_SCANCODE_D:
					_clockwiseMovementWanted = (e.type == Events::kEventKeyDown);
					return true;

				default:
					return false;
			}

		case Events::kEventControllerAxisMotion:
			switch (e.caxis.axis) {
				case Events::kControllerAxisLeftX:
				case Events::kControllerAxisRightX:
					_clockwiseMovementWanted = (e.caxis.value > 10000);
					_counterClockwiseMovementWanted = (e.caxis.value < -10000);
					return true;

				default:
					return false;
			}

		default:
			return false;
	}
}

void CameraController::processMovement(float frameTime) {
	if (_flycam) {
		CameraMan.update();
		return;
	}

	bool moveClockwise = _clockwiseMovementWanted && !_counterClockwiseMovementWanted;
	bool moveCounterClockwise = _counterClockwiseMovementWanted && !_clockwiseMovementWanted;

	if (moveClockwise) {
		_yaw -= kRotationSpeed * frameTime;
		_yaw = fmodf(_yaw, 2.0f * M_PI);
	} else if (moveCounterClockwise) {
		_yaw += kRotationSpeed * frameTime;
		_yaw = fmodf(_yaw, 2.0f * M_PI);
	}

	if (moveClockwise || moveCounterClockwise || _dirty) {
		float x = _target.x + _distance * sin(_yaw);
		float y = _target.y - _distance * cos(_yaw);
		float z = _target.z + _height;

		CameraMan.setPosition(x, y, z);
		CameraMan.setOrientation(_pitch, 0.0f, Common::rad2deg(_yaw));
		CameraMan.update();

		_dirty = false;
	}
}

void CameraController::stopMovement() {
	_clockwiseMovementWanted = false;
	_counterClockwiseMovementWanted = false;
}

} // End of namespace KotORBase

} // End of namespace Engines
