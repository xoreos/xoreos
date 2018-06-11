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
 *  Engine utility class for camera handling where camera rotates around PC.
 */

#include "src/common/maths.h"

#include "src/engines/aurora/satellitecamera.h"

#include "src/graphics/camera.h"

DECLARE_SINGLETON(Engines::SatelliteCamera)

namespace Engines {

static const float kRotationSpeed = M_PI / 2.f;

SatelliteCamera::SatelliteCamera()
		: _distance(0.0f),
		  _yaw(0.0f),
		  _pitch(0.0f),
		  _pitchSin(0.0f),
		  _pitchCos(1.0f),
		  _height(0.0f),
		  _leftBtnPressed(false),
		  _rightBtnPressed(false),
		  _dirty(true) {
}

void SatelliteCamera::setTarget(float x, float y, float z) {
	_target.x = x;
	_target.y = y;
	_target.z = z;
	_dirty = true;
}

void SatelliteCamera::setDistance(float value) {
	_distance = value;
	_dirty = true;
}

void SatelliteCamera::setPitch(float value) {
	_pitch = value;
	float pitchRad = Common::deg2rad(_pitch);
	_pitchSin = sin(pitchRad);
	_pitchCos = cos(pitchRad);
	_dirty = true;
}

void SatelliteCamera::setHeight(float value) {
	_height = value;
	_dirty = true;
}

void SatelliteCamera::setYaw(float value) {
	_yaw = value;
	_dirty = true;
}

float SatelliteCamera::getYaw() const {
	return _yaw;
}

bool SatelliteCamera::handleCameraInput(const Events::Event &e) {
	switch (e.type) {
		case Events::kEventKeyDown:
		case Events::kEventKeyUp:
			switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_A:
					_leftBtnPressed = e.type == Events::kEventKeyDown;
					return true;

				case SDL_SCANCODE_D:
					_rightBtnPressed = e.type == Events::kEventKeyDown;
					return true;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return false;
}

void SatelliteCamera::update(float dt) {
	if (_dirty ||
			(_leftBtnPressed && !_rightBtnPressed) ||
			(_rightBtnPressed && !_leftBtnPressed)) {
		if (_leftBtnPressed && !_rightBtnPressed) {
			_yaw += kRotationSpeed * dt;
			_yaw = fmodf(_yaw, 2.0f * M_PI);
		}
		if (_rightBtnPressed && !_leftBtnPressed) {
			_yaw -= kRotationSpeed * dt;
			_yaw = fmodf(_yaw, 2.0f * M_PI);
		}

		float x = _target.x + _distance * sin(_yaw);
		float y = _target.y - _distance * cos(_yaw);
		float z = _target.z + _height;

		CameraMan.setPosition(x, y, z);
		CameraMan.setOrientation(_pitch, 0.0f, Common::rad2deg(_yaw));
		CameraMan.update();

		_dirty = false;
	}
}

void SatelliteCamera::clearInput() {
	_leftBtnPressed = false;
	_rightBtnPressed = false;
}

} // End of namespace Engines
