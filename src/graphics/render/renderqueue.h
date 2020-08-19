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
		const glm::mat4 *transform;
		float reference;  ///< Reference point to the camera location, primarily used for depth sorting.
		float alpha;      ///< Custom alpha value applied per-object.

		RenderQueueNode() : program(0), surface(0), material(0), mesh(0), transform(0), reference(0.0f), alpha(1.0f) {}
		RenderQueueNode(const RenderQueueNode &src) : program(src.program), surface(src.surface), material(src.material), mesh(src.mesh), transform(src.transform), reference(src.reference), alpha(src.alpha) {}
		RenderQueueNode(Shader::ShaderProgram *prog, Shader::ShaderSurface *sur, Shader::ShaderMaterial *mat, Mesh::Mesh *mes, const glm::mat4 *t, float a = 1.0f, float ref = 0.0f) : program(prog), surface(sur), material(mat), mesh(mes), transform(t), reference(ref), alpha(a) {}

		inline const RenderQueueNode &operator=(const RenderQueueNode &src) { program = src.program; material = src.material; surface = src.surface; mesh = src.mesh; transform = src.transform; reference = src.reference; alpha = src.alpha; return *this; }
	};

	RenderQueue(uint32_t precache = 1000);
	~RenderQueue();

	void setCameraReference(const glm::vec3 &reference);

	void queueItem(Shader::ShaderProgram *program, Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh, const glm::mat4 *transform, float alpha);
	void queueItem(Shader::ShaderRenderable *renderable, const glm::mat4 *transform, float alpha);

	void sortShader(); ///< Sort queue elements by shader program.
	void sortDepth();  ///< Sort queue elements by depth.

	void render();  ///< Render all queued items.

	void clear();  ///< Clear the queue of all items.

private:

	std::vector<RenderQueueNode>_nodeArray;
	glm::vec3 _cameraReference;

	void bindBoneUniforms(Shader::ShaderProgram *program, Shader::ShaderSurface *surface, Mesh::Mesh *mesh);
};

} // namespace Render

} // namespace Graphics

#endif // GRAPHICS_RENDER_RENDERQUEUE
