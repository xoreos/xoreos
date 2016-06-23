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
	_queueColorSolidPrimary.setCameraReference(reference);
	_queueColorSolidSecondary.setCameraReference(reference);
	_queueColorTransparentPrimary.setCameraReference(reference);
	_queueColorTransparentSecondary.setCameraReference(reference);
}

void RenderManager::queueRenderable(Shader::ShaderRenderable *renderable, const Common::Matrix4x4 *transform, float alpha) {

	if (renderable->getStateFlags() & SHADER_RENDER_REALLY_TRANSPARENT) {
		if (renderable->getMesh()->getVertexBuffer()->getCount() > 6) {
			_queueColorTransparentPrimary.queueItem(renderable, transform, alpha);
		} else {
			_queueColorTransparentSecondary.queueItem(renderable, transform, alpha);
		}
	} else {
		if (renderable->getMesh()->getVertexBuffer()->getCount() > 6) {
			_queueColorSolidPrimary.queueItem(renderable, transform, alpha);
		} else {
			_queueColorSolidSecondary.queueItem(renderable, transform, alpha);
		}
	}
}

void RenderManager::sort() {
	_queueColorSolidPrimary.sortShader();
	_queueColorSolidSecondary.sortShader();
	//_queueColorTransparent.sortDepth();
	_queueColorTransparentPrimary.sortDepth();
	_queueColorTransparentSecondary.sortDepth();
}

void RenderManager::render() {
	_queueColorSolidPrimary.render();
	_queueColorSolidSecondary.render();
	_queueColorTransparentPrimary.render();
	_queueColorTransparentSecondary.render();
}

void RenderManager::clear() {
	_queueColorSolidPrimary.clear();
	_queueColorSolidSecondary.clear();
	_queueColorTransparentPrimary.clear();
	_queueColorTransparentSecondary.clear();
}

} // namespace Render

} // namespace Graphics
