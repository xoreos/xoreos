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
 *  The global shader surface manager.
 */

#include "src/common/util.h"

#include "src/graphics/shader/surfaceman.h"

DECLARE_SINGLETON(Graphics::Shader::SurfaceManager)

namespace Graphics {

namespace Shader {

SurfaceManager::SurfaceManager() {
	ShaderSurface *surface = new ShaderSurface(ShaderMan.getShaderObject("default/default.vert", SHADER_VERTEX), "defaultSurface");
	_resourceMap[surface->getName()] = surface;
}

SurfaceManager::~SurfaceManager() {
	deinit();
}

void SurfaceManager::init() {
	status("Initialising default surfaces...");
}

void SurfaceManager::deinit() {
	for (std::map<Common::UString, ShaderSurface *>::iterator iter = _resourceMap.begin(); iter != _resourceMap.end(); ++iter) {
		delete iter->second;
	}
	_resourceMap.clear();
}

void SurfaceManager::cleanup() {
	std::map<Common::UString, ShaderSurface *>::iterator iter = _resourceMap.begin();
	while (iter != _resourceMap.end()) {
		ShaderSurface *surface = iter->second;
		iter++;
		if (surface->useCount() == 0) {
			iter = delResource(iter);
		} else {
			iter++;
		}
	}
}

void SurfaceManager::addSurface(ShaderSurface *surface) {
	if (!surface) {
		return;
	}

	std::map<Common::UString, ShaderSurface *>::iterator iter = _resourceMap.find(surface->getName());
	if (iter == _resourceMap.end()) {
		_resourceMap[surface->getName()] = surface;
	}
}

void SurfaceManager::delSurface(ShaderSurface *surface) {
	if (!surface) {
		return;
	}

	std::map<Common::UString, ShaderSurface *>::iterator iter = _resourceMap.find(surface->getName());
	if (iter != _resourceMap.end()) {
		delResource(iter);
	}
}

ShaderSurface *SurfaceManager::getSurface(const Common::UString &name) {
	std::map<Common::UString, ShaderSurface *>::iterator iter = _resourceMap.find(name);
	if (iter != _resourceMap.end()) {
		return iter->second;
	} else {
		return 0;
	}
}

std::map<Common::UString, ShaderSurface *>::iterator SurfaceManager::delResource(std::map<Common::UString, ShaderSurface *>::iterator iter) {
	std::map<Common::UString, ShaderSurface *>::iterator inext = iter;
	inext++;
	delete iter->second;
	_resourceMap.erase(iter);

	return inext;
}

} // End of namespace Shader

} // End of namespace Graphics
