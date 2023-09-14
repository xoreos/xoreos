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

#include "external/glm/gtc/type_ptr.hpp"

#include "src/graphics/render/renderman.h"

DECLARE_SINGLETON(Graphics::Render::RenderManager)

namespace Graphics {

namespace Render {

RenderManager::RenderManager() : _sortingHints(SORT_HINT_NORMAL) {
}

RenderManager::~RenderManager() {
}

void RenderManager::setSortingHint(SortingHints hint) {
	_sortingHints = hint;
}

void RenderManager::setCameraReference(const glm::vec3 &reference) {
	_queueColorSolidPrimary.setCameraReference(reference);
	_queueColorSolidSecondary.setCameraReference(reference);
	_queueColorSolidDecal.setCameraReference(reference);
	_queueColorTransparentPrimary.setCameraReference(reference);
	_queueColorTransparentSecondary.setCameraReference(reference);
}

void RenderManager::queueNode(const Graphics::Aurora::ModelNode *node, uint32_t flags) {
	if (flags & Shader::ShaderMaterial::MATERIAL_DECAL) {
		_queueColorSolidDecal.queueNode(node);
	} else if (flags & Shader::ShaderMaterial::MATERIAL_TRANSPARENT) {
		if (flags & Shader::ShaderMaterial::MATERIAL_TRANSPARENT_B) {
			_queueColorTransparentSecondary.queueNode(node);
		} else {
			_queueColorTransparentPrimary.queueNode(node);
		}
	} else {
		if (flags & Shader::ShaderMaterial::MATERIAL_OPAQUE) {
			if (flags & Shader::ShaderMaterial::MATERIAL_OPAQUE_B) {
				_queueColorSolidSecondary.queueNode(node);
			} else {
				_queueColorSolidPrimary.queueNode(node);
			}
		} else {
			_queueColorSolidDecal.queueNode(node);
		}
	}
}

void RenderManager::sort() {
	switch (_sortingHints) {
	case SORT_HINT_NORMAL:
		_queueColorSolidPrimary.sortShader();
		_queueColorSolidSecondary.sortShader();
		_queueColorSolidDecal.sortShader();
		_queueColorTransparentPrimary.sortDepth();
		_queueColorTransparentSecondary.sortDepth();
		break;
	case SORT_HINT_ALLDEPTH:
		_queueColorSolidPrimary.sortDepth();
		_queueColorSolidSecondary.sortDepth();
		_queueColorSolidDecal.sortDepth();
		_queueColorTransparentPrimary.sortDepth();
		_queueColorTransparentSecondary.sortDepth();
		break;
	default: break;
	}
}

void RenderManager::render() {
	_queueColorSolidPrimary.render();
	_queueColorSolidSecondary.render();
	_queueColorSolidDecal.render();
	_queueColorTransparentPrimary.render();
	_queueColorTransparentSecondary.render();
}

void RenderManager::clear() {
	_queueColorSolidPrimary.clear();
	_queueColorSolidSecondary.clear();
	_queueColorSolidDecal.clear();
	_queueColorTransparentPrimary.clear();
	_queueColorTransparentSecondary.clear();
}

} // namespace Render

} // namespace Graphics
