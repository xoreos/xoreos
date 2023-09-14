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
 *  Render queue, for efficient OpenGL render calls.
 */

#include <cassert>

#include "external/glm/gtc/type_ptr.hpp"

#include "src/graphics/render/renderqueue.h"
#include "src/common/util.h"

#include <algorithm>

namespace Graphics {

namespace Render {

bool compareDepth(const RenderQueue::RenderQueueNode &a, const RenderQueue::RenderQueueNode &b) {
	return (a.reference <= b.reference);
	//return (a.reference.lengthSquared() <= b.reference.lengthSquared());
}

RenderQueue::RenderQueue(uint32_t precache) : _nodeArray(precache), _cameraReference(0.0f, 0.0f, 0.0f) {
}

RenderQueue::~RenderQueue()
{
	_nodeArray.clear();
}

void RenderQueue::setCameraReference(const glm::vec3 &reference) {
	_cameraReference = reference;
}

void RenderQueue::queueNode(const Graphics::Aurora::ModelNode *node) {
	glm::vec3 ref(node->getRenderTransform()[3]);
	ref -= _cameraReference;
	// Length squared of ref serves as a suitable depth sorting value.
	_nodeArray.push_back(RenderQueueNode(node, glm::dot(ref, ref)));
}

void RenderQueue::sortShader() {
	// Do nothing for now.
}

void RenderQueue::sortDepth() {
	if (_nodeArray.size() > 1) {
		std::sort(_nodeArray.begin(), _nodeArray.end(), compareDepth);
	}
}

void RenderQueue::render() {
	if (_nodeArray.size() == 0) {
		return;
	}
	for (size_t i = 0; i < _nodeArray.size(); ++i) {
		_nodeArray[i].node->renderImmediate();
	}
}

void RenderQueue::clear() {
	_nodeArray.clear();
}

} // namespace Render

} // namespace Graphics
