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
	/**
	 * This is about the most inefficient way possible to build a list of lights
	 * that are applied to a mesh. There's no static/dynamic difference: this
	 * is done every single frame, for every single active modelnode with a mesh.
	 *
	 * There's no sorting of lights by distance, no priority checking when the
	 * list is filled.
	 *
	 * This gets much slower on larger levels with hundreds of lights. A lot of
	 * time can be wasted on just this step. There is however one glimmer (pun
	 * intended) of hope: levels fade in out of view around the player character.
	 * This means lights can be activated when a tile comes into view, and
	 * deactivated when it goes out of view. Not every mesh is rendered every
	 * frame, and not every light is in the global list to check against. That's
	 * the theory anyway, but this kind of behaviour is not yet implemented.
	 *
	 * Another approach to build on the above, if needed, is to store active
	 * lighting in a quadtree or similar. That will allow lists of lights to be
	 * built very quickly, especially if the lights are mostly static. Or keep
	 * a combination of the two: hierarchy for static lights (e.g tile main lights)
	 * and just a normal list for dynamic lights (e.g torches held by characters).
	 */

	glm::mat4 modelview = CameraMan.getModelview();
	_activeLights = 0;
	for (const auto *licht : _registered) {
		/**
		 * For comparing distances, it's just as easy to compare distance squared
		 * instead, which will avoid the use of sqrt.
		 */
		glm::vec3 d = pos - licht->position;
		float dsquared = glm::dot(d, d);
		float rsquared = (licht->radius + radius);
		rsquared *= rsquared;
		if (dsquared < rsquared) {
			auto &light = _lights[_activeLights];
			glm::vec4 modified = modelview * glm::vec4(licht->position, 1.0f);
			light.position[0] = modified.x;
			light.position[1] = modified.y;
			light.position[2] = modified.z;
			light.position[3] = 1.0f;

			light.colour[0] = licht->colour[0];
			light.colour[1] = licht->colour[1];
			light.colour[2] = licht->colour[2];
			light.colour[3] = 1.0f;

			/**
			 * @TODO: Some of the coefficients here could be pre-calculated as a performance
			 * benefit. How much benefit is a matter up for debate, but either way it's
			 * recommended to wait until later down the line to implement: get it all
			 * working first, then optimise and increase performance.
			 */
			light.coefficients[0] = licht->multiplier;
			light.coefficients[1] = 1.0f / (licht->radius * licht->radius * 0.25f);
			light.coefficients[2] = licht->ambient ? 1.0f : 0.0f;
			light.coefficients[3] = licht->ambient ? 0.0f : 1.0f;

			if (++_activeLights >= _maxLights) {
				break;
			}
		}
	}
}

} // End of namespace Graphics
