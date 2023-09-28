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

#ifndef GRAPHICS_RENDER_RENDERMANAGER_H
#define GRAPHICS_RENDER_RENDERMANAGER_H

#include "external/glm/vec3.hpp"
#include "external/glm/mat4x4.hpp"

#include "src/graphics/render/renderqueue.h"

#include "src/common/singleton.h"

namespace Graphics {

namespace Render {

class RenderManager : public Common::Singleton<RenderManager> {
public:

	enum SortingHints {
		SORT_HINT_NORMAL = 0,   // Set normal render order sorting.
		SORT_HINT_ALLDEPTH = 1  // Sort _everything_ by depth. Not optimal for rendering, but some games require it.
	};

	RenderManager();
	~RenderManager();

	void setSortingHint(SortingHints hint);

	/**
	 * Not required? Can pull the modelview directly from CameraMan, unless
	 * the reference point for some reason shouldn't match.
	 */
	void setCameraReference(const glm::vec3 &reference);

	/**
	 * Queue a modelnode for rendering, with provided hints to determine which queue
	 * the node should be placed into.
	 * @param node  The modelnode to queue for rendering.
	 * @param flags  Rendering hints, see ShaderMaterial for these.
	 */
	void queueNode(const Graphics::Aurora::ModelNode *node, uint32_t flags = 0);

	void sort();

	void render();

	void clear();

	void init() {}
	void deinit() {}
	void cleanup() {}

private:
	RenderQueue _queueDecal;
	RenderQueue _queueTransparent;

	SortingHints _sortingHints;
};

} // namespace Render

} // namespace Graphics

/** Shortcut for accessing the render manager. */
#define RenderMan Graphics::Render::RenderManager::instance()

#endif // GRAPHICS_RENDER_RENDERMANAGER_H
