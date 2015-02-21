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
#include <string>

#include "graphics/graphics.h"
#include "graphics/types.h"
#include "graphics/glcontainer.h"
#include "graphics/indexbuffer.h"
#include "graphics/vertexbuffer.h"

#include "common/types.h"
#include "common/mutex.h"
#include "common/matrix.h"
#include "common/transmatrix.h"
#include "common/ustring.h"

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

	GLuint _vao;  ///< Vertex Array Object handle. GL3.x only.
};

} // namespace Mesh

} // namespace Graphics

#endif // GRAPHICS_MESH_MESH_H
