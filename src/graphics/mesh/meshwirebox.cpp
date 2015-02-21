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
 *  Wireframe box mesh.
 */

#include "src/graphics/mesh/meshwirebox.h"

namespace Graphics {

namespace Mesh {

MeshWireBox::MeshWireBox() : Mesh(GL_LINES, GL_STATIC_DRAW) {
	_vertexBuffer.setSize(24, 3 * sizeof(float));
	float *verts = static_cast<float *>(_vertexBuffer.getData());

	// TODO: maybe VertexBuffer should allow direct VertexDecl access.
	VertexDecl decl(1);
	decl[0].pointer = verts;
	decl[0].stride = 0;
	decl[0].type = GL_FLOAT;
	decl[0].size = 3;
	decl[0].index = VPOSITION;
	_vertexBuffer.setVertexDecl(decl);

	// Front
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ =  1.0f;
	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ =  1.0f;

	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ =  1.0f;
	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ =  1.0f;

	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ =  1.0f;
	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ =  1.0f;

	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ =  1.0f;
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ =  1.0f;

	// Back
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ = -1.0f;
	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ = -1.0f;

	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ = -1.0f;
	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ = -1.0f;

	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ = -1.0f;
	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ = -1.0f;

	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ = -1.0f;
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ = -1.0f;

	// Sides
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ =  1.0f;
	*verts++ = -1.0f; *verts++ =  1.0f; *verts++ = -1.0f;

	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ =  1.0f;
	*verts++ = -1.0f; *verts++ = -1.0f; *verts++ = -1.0f;

	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ =  1.0f;
	*verts++ =  1.0f; *verts++ = -1.0f; *verts++ = -1.0f;

	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ =  1.0f;
	*verts++ =  1.0f; *verts++ =  1.0f; *verts++ = -1.0f;
}

MeshWireBox::~MeshWireBox() {
}

} // End of namespace Mesh

} // End of namespace Graphics
