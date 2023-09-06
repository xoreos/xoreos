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
 *  The light manager.
 */

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/graphics/camera.h"
#include "src/graphics/lightman.h"
#include "src/graphics/renderable.h"

DECLARE_SINGLETON(Graphics::LightManager)

namespace Graphics {

LightManager::LightManager() : _maxLights(8), _activeLights(0) {
	_lights.resize(_maxLights);
}

LightManager::~LightManager() {
}

bool LightManager::addLight(const Light &light) {
	if (_activeLights > _maxLights) {
		return false;
	}

	_lights[_activeLights++] = light;
	return true;
}

void LightManager::registerLight(const LightNode *light) {
	if (!isLightRegistered(light)) {
		_registered.push_back(light);
	}
}

void LightManager::deregisterLight(const LightNode *light) {
	for (size_t i = 0; i < _registered.size(); ++i) {
		if (_registered[i] == light) {
			_registered[i] = _registered.back();
			_registered.pop_back();
			break;
		}
	}
}

bool LightManager::isLightRegistered(const LightNode *light) {
	for (const auto *reg : _registered) {
		if (reg == light) {
			return true;
		}
	}
	return false;
}

void LightManager::buildActiveLights(const glm::vec3 &pos, float radius) {
	// linear = multiplier;
	// quadratic = 1.0f / (radius * radius);

	glm::mat4 modelview = CameraMan.getModelview();
	_activeLights = 0;
	//radius = 0;
	for (const auto *licht : _registered) {
		if (glm::distance(pos, licht->position) < (licht->radius + radius)) {
			auto &light = _lights[_activeLights];
			glm::vec4 modified = modelview * glm::vec4(licht->position, 1.0f);
			light.position[0] = modified.x;
			light.position[1] = modified.y;
			light.position[2] = modified.z;
			light.position[3] = 1.0f;

			light.ambient[0] = licht->colour[0];
			light.ambient[1] = licht->colour[1];
			light.ambient[2] = licht->colour[2];
			light.ambient[3] = 1.0f;

			light.diffuse[0] = 0.0f;
			light.diffuse[1] = 0.0f;
			light.diffuse[2] = 0.0f;
			light.diffuse[3] = 0.0f;

			light.specular[0] = licht->multiplier;
			light.specular[1] = 1.0f / (licht->radius * licht->radius);
			light.specular[2] = 0.0f;
			light.specular[3] = 0.0f;

			if (++_activeLights >= _maxLights) {
				break;
			}
		}
	}
}

} // End of namespace Graphics
