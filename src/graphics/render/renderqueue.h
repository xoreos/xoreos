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

#ifndef GRAPHICS_RENDER_RENDERQUEUE_H
#define GRAPHICS_RENDER_RENDERQUEUE_H

#include "external/glm/vec3.hpp"
#include "external/glm/mat4x4.hpp"

#include "src/graphics/graphics.h"
#include "src/graphics/aurora/modelnode.h"

#include <vector>

namespace Graphics {

namespace Render {

class RenderQueue {
public:
	struct RenderQueueNode {
		const Graphics::Aurora::ModelNode *node;  ///< The modelnode to be rendered.
		float reference;  ///< Reference point to the camera location, primarily used for depth sorting.

		RenderQueueNode() : node(0), reference(0.0f) {}
		RenderQueueNode(const RenderQueueNode &src) : node(src.node), reference(src.reference) {}
		RenderQueueNode(const Graphics::Aurora::ModelNode *n, float ref = 0.0f) : node(n), reference(ref) {}

		inline const RenderQueueNode &operator=(const RenderQueueNode &src) {
			node = src.node; reference = src.reference; return *this;
		}
	};

	RenderQueue(uint32_t precache = 1000);
	~RenderQueue();

	void setCameraReference(const glm::vec3 &reference);

	void queueNode(const Graphics::Aurora::ModelNode *node);

	void sortShader(); ///< Sort queue elements by shader program.
	void sortDepth();  ///< Sort queue elements by depth.
	void sortHints();  ///< Sort queue by hints provided from the data files.

	void render();  ///< Render all queued items.

	void clear();  ///< Clear the queue of all items.

private:

	std::vector<RenderQueueNode>_nodeArray;
	glm::vec3 _cameraReference;
};

} // namespace Render

} // namespace Graphics

#endif // GRAPHICS_RENDER_RENDERQUEUE
