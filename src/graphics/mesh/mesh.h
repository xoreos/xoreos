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
 *  Generic mesh handling class.
 */

#ifndef GRAPHICS_MESH_MESH_H
#define GRAPHICS_MESH_MESH_H

#include <vector>

#include "src/common/ustring.h"

#include "src/graphics/graphics.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/indexbuffer.h"
#include "src/graphics/vertexbuffer.h"

namespace Graphics {

namespace Mesh {

class Mesh : public GLContainer {
public:
	Mesh(GLuint type = GL_TRIANGLES, GLuint hint = GL_STATIC_DRAW);
	~Mesh();

	VertexBuffer *getVertexBuffer();
	IndexBuffer *getIndexBuffer();

	void setName(const Common::UString &name);
	const Common::UString &getName() const;

	void setType(GLuint type);
	GLuint getType() const;

	void setHint(GLuint hint);
	GLuint getHint() const;

	/** General mesh initialisation, queuing the mesh for GL resource creation. */
	void init();

	void initGL();
	void updateGL();
	void destroyGL();

	void renderImmediate();

	/** Follows the steps of renderImmediate, but broken into different functions. */
	void renderBind();
	void render();
	void renderUnbind();

	void useIncrement();
	void useDecrement();
	uint32_t useCount() const;

	const glm::vec3 &getCentre() const;

	float getRadius() const;

	const glm::mat4 *getBindPosePtr() const { return _bindPosePtr; }
	void setBindPosePtr(const glm::mat4 *value) { _bindPosePtr = value; }

	/** Get a list of bone transformation matrices represented as an array of values. */
	std::vector<float> &getBoneTransforms() { return _boneTransforms; }

protected:
	// For each attrib, when calculating VBOs, do (attrib address) - (base data address) to find initial data offset.
	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

	/** Initialise GL components with data taken from vertex buffers. */
	virtual void doRebuild();

	/** Free GL resources. */
	virtual void doDestroy();

	GLuint _type;
	GLuint _hint;

private:
	Common::UString _name;
	uint32_t _usageCount;

	GLuint _vao;  ///< Vertex Array Object handle. GL3.x only.

	glm::vec3 _centre;
	glm::vec3 _max;
	glm::vec3 _min;
	float _radius;

	const glm::mat4 *_bindPosePtr;
	std::vector<float> _boneTransforms;
};

} // End of namespace Mesh

} // End of namespace Graphics

#endif // GRAPHICS_MESH_MESH_H
