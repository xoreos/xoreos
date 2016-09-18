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

#include "src/graphics/graphics.h"
#include "src/graphics/shader/shaderrenderable.h"

#include <vector>

namespace Graphics {

namespace Render {

class RenderQueue {
public:
	struct RenderQueueNode {
		Shader::ShaderProgram *program;
		Shader::ShaderSurface *surface;
		Shader::ShaderMaterial *material;
		Mesh::Mesh *mesh;
		const Common::Matrix4x4 *transform;
		float reference;  ///< Reference point to the camera location, primarily used for depth sorting.
		float padding;    ///< Padding for 64bit architectures.

		RenderQueueNode() : program(0), surface(0), material(0), mesh(0), transform(0), reference(0.0f) {}
		RenderQueueNode(const RenderQueueNode &src) : program(src.program), surface(src.surface), material(src.material), mesh(src.mesh), transform(src.transform), reference(src.reference) {}
		RenderQueueNode(Shader::ShaderProgram *prog, Shader::ShaderSurface *sur, Shader::ShaderMaterial *mat, Mesh::Mesh *mes, const Common::Matrix4x4 *t) : program(prog), surface(sur), material(mat), mesh(mes), transform(t), reference(0.0f) {}
		RenderQueueNode(Shader::ShaderProgram *prog, Shader::ShaderSurface *sur, Shader::ShaderMaterial *mat, Mesh::Mesh *mes, const Common::Matrix4x4 *t, float ref) : program(prog), surface(sur), material(mat), mesh(mes), transform(t), reference(ref) {}

		inline const RenderQueueNode &operator=(const RenderQueueNode &src) { material = src.material; surface = src.surface; mesh = src.mesh; transform = src.transform; reference = src.reference; return *this; }
	};

	RenderQueue(uint32 precache = 1000);
	~RenderQueue();

	void setCameraReference(const Common::Vector3 &reference);

	void queueItem(Shader::ShaderProgram *program, Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh, const Common::Matrix4x4 *transform);
	void queueItem(Shader::ShaderRenderable *renderable, const Common::Matrix4x4 *transform);

	void sortShader(); ///< Sort queue elements by shader program.
	void sortDepth();  ///< Sort queue elements by depth.

	void render();  ///< Render all queued items.

	void clear();  ///< Clear the queue of all items.

private:

	std::vector<RenderQueueNode>_nodeArray;
	Common::Vector3 _cameraReference;
};

} // namespace Render

} // namespace Graphics

#endif // GRAPHICS_RENDER_RENDERQUEUE
