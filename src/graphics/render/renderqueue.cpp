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

#include "src/graphics/render/renderqueue.h"
#include "src/common/util.h"

#include <algorithm>

namespace Graphics {

namespace Render {

bool compareShader(const RenderQueue::RenderQueueNode &a, const RenderQueue::RenderQueueNode &b)
{
	if (a.program < b.program)
		return false;
	else if (a.program > b.program)
		return true;
	if (a.material < b.material)  // node_program values are equal.
		return false;
	else if (a.material > b.material)
		return true;
	else  // node material values are equal, compare mesh values next.
		return (a.mesh <= b.mesh ? false : true);
	// No, surfaces are not compared here.
}

bool compareDepth(const RenderQueue::RenderQueueNode &a, const RenderQueue::RenderQueueNode &b) {
	return (a.reference <= b.reference);
	//return (a.reference.lengthSquared() <= b.reference.lengthSquared());
}

RenderQueue::RenderQueue(uint32 precache) : _nodeArray(precache) {
//	_nodeArray.reserve(1000);
}

RenderQueue::~RenderQueue()
{
	_nodeArray.clear();
}

void RenderQueue::setCameraReference(const Common::Vector3 &reference) {
	_cameraReference = reference;
}

void RenderQueue::queueItem(Shader::ShaderProgram *program, Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh, const Common::Matrix4x4 *transform) {
	Common::Vector3 ref = transform->getPosition();
	ref -= _cameraReference;
	// Length squared of ref serves as a suitable depth sorting value.
	_nodeArray.push_back(RenderQueueNode(program, surface, material, mesh, transform, ref.dot(ref)));
}

void RenderQueue::queueItem(Shader::ShaderRenderable *renderable, const Common::Matrix4x4 *transform) {
	Common::Vector3 ref = transform->getPosition();
	ref -= _cameraReference;
	// Length squared of ref serves as a suitable depth sorting value.
	_nodeArray.push_back(RenderQueueNode(renderable->getProgram(), renderable->getSurface(), renderable->getMaterial(), renderable->getMesh(), transform, ref.dot(ref)));
}

void RenderQueue::sortShader() {
	std::sort(_nodeArray.begin(), _nodeArray.end(), compareShader);
}

void RenderQueue::sortDepth() {
	std::sort(_nodeArray.begin(), _nodeArray.end(), compareDepth);
}

void RenderQueue::render() {
	if (_nodeArray.size() == 0) {
		return;
	}

	Shader::ShaderProgram *currentProgram = 0;
	Shader::ShaderMaterial *currentMaterial = 0;
	Shader::ShaderSurface *currentSurface = 0;
	Mesh::Mesh *currentMesh = 0;

	uint32 i = 0;
	uint32 limit = _nodeArray.size();
	while (i < limit) {
		assert(_nodeArray[i].program);
		if (currentProgram != _nodeArray[i].program) {
			currentProgram = _nodeArray[i].program;
			glUseProgram(currentProgram->glid);

			if (currentMaterial != 0) {
				currentMaterial->unbindGLState();
			}
			currentMaterial = 0;
			currentSurface = 0;
		}

		assert(_nodeArray[i].material);
		if (currentMaterial != _nodeArray[i].material) {
			if (currentMaterial != 0) {
				currentMaterial->unbindGLState();
			}
			currentMaterial = _nodeArray[i].material;
			currentMaterial->bindProgram(currentProgram);
			currentMaterial->bindGLState();
		}

		assert(_nodeArray[i].surface);
		assert(_nodeArray[i].mesh);

		currentSurface = _nodeArray[i].surface;
		currentMesh = _nodeArray[i].mesh;
		currentMesh->renderBind();  // Binds VAO ready for rendering.

		// There's at least one mesh to be rendering here.
		assert(_nodeArray[i].transform);
		currentSurface->bindProgram(currentProgram, _nodeArray[i].transform);
		currentMesh->render();

		++i;  // Move to next object.
		while ((i < limit) && (_nodeArray[i].mesh == currentMesh) && (_nodeArray[i].material == currentMaterial) && (_nodeArray[i].surface == currentSurface)) {
			// Next object is basically the same, but will have a different object modelview transform. So rebind that, and render again.
			assert(_nodeArray[i].transform);
			currentSurface->bindObjectModelview(currentProgram, _nodeArray[i].transform);
			currentMesh->render();
			++i;
		}
		// Done rendering, unbind the mesh, and onwards into the queue.
		currentMesh->renderUnbind();
	}

	// Restore OpenGL state on exit.
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void RenderQueue::clear() {
	_nodeArray.clear();
}

} // namespace Render

} // namespace Graphics
