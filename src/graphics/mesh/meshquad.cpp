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
 *  Default quad mesh.
 */

#include "src/graphics/mesh/meshquad.h"

namespace Graphics {

namespace Mesh {

MeshQuad::MeshQuad() : Mesh(GL_TRIANGLES, GL_STATIC_DRAW) {
	VertexDecl vertexDecl;
	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VTCOORD, 2, GL_FLOAT));
	_vertexBuffer.setVertexDeclLinear(6, vertexDecl);

	_vertexBuffer.setSize(6, 5 * sizeof(float));
	float *verts = static_cast<float *>(_vertexBuffer.getData());

	// Front
	*verts++ = 1.0f; *verts++ = 1.0f; *verts++ = 0.0f;
	*verts++ = 0.0f; *verts++ = 1.0f; *verts++ = 0.0f;
	*verts++ = 0.0f; *verts++ = 0.0f; *verts++ = 0.0f;

	*verts++ = 0.0f; *verts++ = 0.0f; *verts++ = 0.0f;
	*verts++ = 1.0f; *verts++ = 0.0f; *verts++ = 0.0f;
	*verts++ = 1.0f; *verts++ = 1.0f; *verts++ = 0.0f;

	*verts++ = 1.0f; *verts++ = 1.0f;
	*verts++ = 0.0f; *verts++ = 1.0f;
	*verts++ = 0.0f; *verts++ = 0.0f;

	*verts++ = 0.0f; *verts++ = 0.0f;
	*verts++ = 1.0f; *verts++ = 0.0f;
	*verts++ = 1.0f; *verts++ = 1.0f;
}

} // End of namespace Mesh

} // End of namespace Graphics
