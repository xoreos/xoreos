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

#include "src/common/ustring.h"
#include "src/common/mutex.h"
#include "src/common/matrix4x4.h"

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
	uint32 useCount() const;

protected:
	// For each attrib, when calculating VBOs, do (attrib address) - (base data address) to find initial data offset.
	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

	/** Initialise GL components with data taken from vertex buffers. */
	virtual void doRebuild();

	/** Free GL resources. */
	virtual void doDestroy();

private:
	GLuint _type;
	GLuint _hint;
	Common::UString _name;
	uint32 _usageCount;

	GLuint _vao;  ///< Vertex Array Object handle. GL3.x only.
};

} // End of namespace Mesh

} // End of namespace Graphics

#endif // GRAPHICS_MESH_MESH_H
