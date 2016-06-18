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
 *  Render queue manager.
 */

#include "glm/gtc/type_ptr.hpp"

#include "src/graphics/render/renderman.h"

DECLARE_SINGLETON(Graphics::Render::RenderManager)

namespace Graphics {

namespace Render {

RenderManager::RenderManager() {
}

RenderManager::~RenderManager() {
}

void RenderManager::setCameraReference(const glm::vec3 &reference) {
	_queueColorSolid.setCameraReference(reference);
	_queueColorTransparent.setCameraReference(reference);
}

void RenderManager::queueRenderable(Shader::ShaderRenderable *renderable, const glm::mat4 *transform, float alpha) {
	/*
	if (renderable->getMaterial()->getFlags() & SHADER_MATERIAL_TRANSPARENT) {
		_queueColorTransparent.queueItem(renderable, transform, alpha);
	} else {
		_queueColorSolid.queueItem(renderable, transform, alpha);
	}
	*/
	_queueColorSolid.queueItem(renderable, transform, alpha);
}

void RenderManager::sort() {
	_queueColorSolid.sortShader();
	_queueColorTransparent.sortDepth();
}

void RenderManager::render() {
	_queueColorSolid.render();
	_queueColorTransparent.render();
}

void RenderManager::clear() {
	_queueColorSolid.clear();
	_queueColorTransparent.clear();
}

} // namespace Render

} // namespace Graphics
