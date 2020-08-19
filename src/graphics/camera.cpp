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
 *  Camera management.
 */

#include <cstring>

#include "external/glm/vec3.hpp"
#include "external/glm/mat4x4.hpp"
#include "external/glm/gtc/type_ptr.hpp"
#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/graphics/camera.h"
#include "src/graphics/graphics.h"

#include "src/events/events.h"
#include "src/events/notifications.h"

DECLARE_SINGLETON(Graphics::CameraManager)

namespace Graphics {

CameraManager::CameraManager() : _lastChanged(0), _needUpdate(false) {
	_minPosition[0] = -FLT_MAX;
	_minPosition[1] = -FLT_MAX;
	_minPosition[2] = -FLT_MAX;
	_maxPosition[0] =  FLT_MAX;
	_maxPosition[1] =  FLT_MAX;
	_maxPosition[2] =  FLT_MAX;

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;

	_positionCache   [0] = 0.0f;
	_positionCache   [1] = 0.0f;
	_positionCache   [2] = 0.0f;
	_orientationCache[0] = 0.0f;
	_orientationCache[1] = 0.0f;
	_orientationCache[2] = 0.0f;
}

void CameraManager::update() {
	GfxMan.lockFrame();

	if (!_needUpdate) {
		GfxMan.unlockFrame();
		return;
	}

	_needUpdate = false;

	memcpy(_positionCache   , _position   , sizeof(_positionCache));
	memcpy(_orientationCache, _orientation, sizeof(_orientationCache));

	GfxMan.recalculateObjectDistances();
	NotificationMan.cameraMoved();

	GfxMan.unlockFrame();
}

const float *CameraManager::getPosition() const {
	return _positionCache;
}

const float *CameraManager::getOrientation() const {
	return _orientationCache;
}

void CameraManager::reset() {
	_minPosition[0] = -FLT_MAX;
	_minPosition[1] = -FLT_MAX;
	_minPosition[2] = -FLT_MAX;
	_maxPosition[0] =  FLT_MAX;
	_maxPosition[1] =  FLT_MAX;
	_maxPosition[2] =  FLT_MAX;

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;

	_lastChanged = EventMan.getTimestamp();

	_needUpdate = true;
}

void CameraManager::limit(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
	_minPosition[0] = minX;
	_minPosition[1] = minY;
	_minPosition[2] = minZ;
	_maxPosition[0] = maxX;
	_maxPosition[1] = maxY;
	_maxPosition[2] = maxZ;
}

void CameraManager::setPosition(float x, float y, float z) {
	_position[0] = CLIP(x, _minPosition[0], _maxPosition[0]);
	_position[1] = CLIP(y, _minPosition[1], _maxPosition[1]);
	_position[2] = CLIP(z, _minPosition[2], _maxPosition[2]);

	_lastChanged = EventMan.getTimestamp();

	_needUpdate = true;
}

void CameraManager::setOrientation(float x, float y, float z) {
	_orientation[0] = fmodf(x, 360.0f);
	_orientation[1] = fmodf(y, 360.0f);
	_orientation[2] = fmodf(z, 360.0f);

	_lastChanged = EventMan.getTimestamp();

	_needUpdate = true;
}

void CameraManager::turn(float x, float y, float z) {
	setOrientation(_orientation[0] + x, _orientation[1] + y, _orientation[2] + z);
}

void CameraManager::move(float x, float y, float z) {
	setPosition(_position[0] + x, _position[1] + y, _position[2] + z);
}

void CameraManager::moveRelative(float x, float y, float z) {
	glm::mat4 orientation;

	orientation = glm::rotate(orientation, Common::deg2rad(_orientation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
	orientation = glm::rotate(orientation, Common::deg2rad(_orientation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	orientation = glm::rotate(orientation, Common::deg2rad(_orientation[0]), glm::vec3(1.0f, 0.0f, 0.0f));

	const glm::vec4 relative = orientation * glm::vec4(x, y, z, 0);

	move(relative[0], relative[1], relative[2]);
}

uint32_t CameraManager::lastChanged() const {
	return _lastChanged;
}

} // End of namespace Graphics
