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

} // End of namespace Graphics
