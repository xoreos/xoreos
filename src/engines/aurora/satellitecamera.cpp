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
		: _distance(0), _yaw(0), _pitch(0), _pitchSin(0), _pitchCos(1),
		  _leftBtnPressed(false), _rightBtnPressed(false), _dirty(true) {
}

void SatelliteCamera::setTarget(float x, float y, float z) {
	_target._x = x;
	_target._y = y;
	_target._z = z;
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
					break;
				case SDL_SCANCODE_D:
					_rightBtnPressed = e.type == Events::kEventKeyDown;
					break;
				default:
					return false;
			}
		default:
			return false;
	}
	return true;
}

void SatelliteCamera::update(float dt) {
	if (_dirty ||
			(_leftBtnPressed && !_rightBtnPressed) ||
			(_rightBtnPressed && !_leftBtnPressed)) {
		if (_leftBtnPressed && !_rightBtnPressed) {
			_yaw += kRotationSpeed * dt;
			_yaw = fmodf(_yaw, 2 * M_PI);
		}
		if (_rightBtnPressed && !_leftBtnPressed) {
			_yaw -= kRotationSpeed * dt;
			_yaw = fmodf(_yaw, 2 * M_PI);
		}

		float x = _target._x + _distance * sin(_yaw);
		float y = _target._y - _distance * cos(_yaw) * _pitchSin;
		float z = _target._z + _distance * _pitchCos;

		CameraMan.setPosition(x, y, z);
		CameraMan.setOrientation(_pitch, 0, Common::rad2deg(_yaw));
		CameraMan.update();

		_dirty = false;
	}
}

} // End of namespace Engines
