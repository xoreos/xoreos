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

	ShaderMaterial *material = new ShaderMaterial(ShaderMan.getShaderObject("default/colour.frag", SHADER_FRAGMENT), "defaultWhite");
	material->setVariable("_colour", _colourWhite);
	_colourWhite[0] = 1.0f;
	_colourWhite[1] = 1.0f;
	_colourWhite[2] = 1.0f;
	_colourWhite[3] = 1.0f;
	_resourceMap[material->getName()] = material;

	material = new ShaderMaterial(ShaderMan.getShaderObject("default/colour.frag", SHADER_FRAGMENT), "defaultBlack");
	material->setVariable("_colour", _colourBlack);
	_colourBlack[0] = 0.0f;
	_colourBlack[1] = 0.0f;
	_colourBlack[2] = 0.0f;
	_colourBlack[3] = 1.0f;
	_resourceMap[material->getName()] = material;

	material = new ShaderMaterial(ShaderMan.getShaderObject("default/colour.frag", SHADER_FRAGMENT), "defaultBlack75");
	material->setVariable("_colour", _colourBlack75);
	_colourBlack75[0] = 0.0f;
	_colourBlack75[1] = 0.0f;
	_colourBlack75[2] = 0.0f;
	_colourBlack75[3] = 0.75f;
	_resourceMap[material->getName()] = material;

	material = new ShaderMaterial(ShaderMan.getShaderObject("default/colour.frag", SHADER_FRAGMENT), "defaultBlack50");
	material->setVariable("_colour", _colourBlack50);
	_colourBlack50[0] = 0.0f;
	_colourBlack50[1] = 0.0f;
	_colourBlack50[2] = 0.0f;
	_colourBlack50[3] = 0.5f;
	_resourceMap[material->getName()] = material;

	material = new ShaderMaterial(ShaderMan.getShaderObject("default/colour.frag", SHADER_FRAGMENT), "defaultGrey50");
	material->setVariable("_colour", _colourGrey50);
	_colourGrey50[0] = 0.5f;
	_colourGrey50[1] = 0.5f;
	_colourGrey50[2] = 0.5f;
	_colourGrey50[3] = 0.5f;
	_resourceMap[material->getName()] = material;
}

void MaterialManager::deinit() {
	for (std::map<Common::UString, ShaderMaterial *>::iterator iter = _resourceMap.begin(); iter != _resourceMap.end(); ++iter) {
		delete iter->second;
	}
	_resourceMap.clear();
}

void MaterialManager::cleanup() {
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
