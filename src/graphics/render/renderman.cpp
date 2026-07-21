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
	_queueDecal.setCameraReference(reference);
	_queueTransparent.setCameraReference(reference);
}

void RenderManager::queueNode(const Graphics::Aurora::ModelNode *node, uint32_t flags) {
	if (flags) {
		if (flags & Graphics::Aurora::ModelNode::RENDER_HINT_TRANSPARENT) {
			_queueTransparent.queueNode(node);
		} else if (flags & Graphics::Aurora::ModelNode::RENDER_HINT_DECAL) {
			_queueDecal.queueNode(node);
		}
	} else {
		node->renderImmediate();
	}
}

void RenderManager::sort() {
	switch (_sortingHints) {
	case SORT_HINT_NORMAL:
		_queueDecal.sortHints();
		_queueTransparent.sortHints();
		break;
	case SORT_HINT_ALLDEPTH:
		_queueDecal.sortDepth();
		_queueTransparent.sortDepth();
		break;
	default: break;
	}
}

void RenderManager::render() {
	_queueTransparent.render();
	_queueDecal.render();
}

void RenderManager::clear() {
	_queueDecal.clear();
	_queueTransparent.clear();
}

} // namespace Render

} // namespace Graphics
