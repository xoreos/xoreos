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
 *  The global shader material manager.
 */

#include "src/common/util.h"

#include "src/graphics/shader/materialman.h"

DECLARE_SINGLETON(Graphics::Shader::MaterialManager)

namespace Graphics {

namespace Shader {

MaterialManager::MaterialManager() {
}

MaterialManager::~MaterialManager() {
	deinit();
}

void MaterialManager::init() {
	status("Initialising default materials...");

	ShaderMaterial *material = new ShaderMaterial(ShaderMan.getShaderObject("default/color.frag", SHADER_FRAGMENT), "defaultWhite");
	float *color = (float *)(material->getVariableData("color"));
	if (color) {
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
	}
	_resourceMap[material->getName()] = material;
}

void MaterialManager::deinit() {
	for (std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.begin(); iter != _resourceMap.end(); ++iter) {
		delete iter->second;
	}
	_resourceMap.clear();
}

void MaterialManager::cleanup() {
	std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.begin();
	while (iter != _resourceMap.end()) {
		ShaderMaterial *material = iter->second;
		if (material->useCount() == 0) {
			iter = delResource(iter);
		} else {
			iter++;
		}
	}
}

void MaterialManager::addMaterial(ShaderMaterial *material) {
	if (!material) {
		return;
	}

	std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.find(material->getName());
	if (iter == _resourceMap.end()) {
		_resourceMap[material->getName()] = material;
	}
}

void MaterialManager::delMaterial(ShaderMaterial *material) {
	if (!material) {
		return;
	}

	std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.find(material->getName());
	if (iter != _resourceMap.end()) {
		delResource(iter);
	}
}

ShaderMaterial *MaterialManager::getMaterial(const Common::UString &name) {
	std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.find(name);
	if (iter != _resourceMap.end()) {
		return iter->second;
	} else {
		return 0;
	}
}

std::map<Common::UString, ShaderMaterial *>::iterator MaterialManager::delResource(std::map<Common::UString, ShaderMaterial *>::iterator iter) {
	std::map<Common::UString, ShaderMaterial *>::iterator inext = iter;
	inext++;
	delete iter->second;
	_resourceMap.erase(iter);

	return inext;
}

} // End of namespace Shader

} // End of namespace Graphics
