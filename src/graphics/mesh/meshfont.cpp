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

#include "src/graphics/mesh/meshfont.h"

namespace Graphics {

namespace Mesh {

MeshFont::MeshFont() : Mesh(GL_QUADS, GL_DYNAMIC_DRAW) {
	// Each vertex is { x, y, z, u, v, r, g, b, a }
	_vertexBuffer.setSize(4, 9 * sizeof(float));

	VertexDecl vertexDecl;
	vertexDecl.push_back(VertexAttrib(VPOSITION, 3, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VTCOORD, 2, GL_FLOAT));
	vertexDecl.push_back(VertexAttrib(VCOLOR, 4, GL_FLOAT));
	_vertexBuffer.setVertexDeclLinear(4, vertexDecl);

	// Fill in some valid data so that mesh init doesn't go beserk.
	float *verts = static_cast<float *>(_vertexBuffer.getData());

	// Position Data.
	*verts++ = -1.0f;
	*verts++ = 0.0f;
	*verts++ = 0.0f;

	*verts++ = 0.0f;
	*verts++ = 1.0f;
	*verts++ = 0.0f;

	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 0.0f;

	*verts++ = 1.0f;
	*verts++ = 0.0f;
	*verts++ = 0.0f;

	// UV coordinates.
	*verts++ = 0.0f;
	*verts++ = 0.0f;

	*verts++ = 0.0f;
	*verts++ = 1.0f;

	*verts++ = 1.0f;
	*verts++ = 1.0f;

	*verts++ = 1.0f;
	*verts++ = 0.0f;

	// Colour information.
	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;

	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;

	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;

	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;
	*verts++ = 1.0f;
}

void MeshFont::render(float *pos, float *uv, float *rgba) {
	/**
	 * This is somewhat simpler than the normal mesh rendering method. There are not
	 * indices into the vertex array, so that can be stripped out. It's also assumed
	 * that GL_ARRAY_BUFFER is bound, so it can be overwritten entirely with dynamic
	 * data. This is the same between GL3 and GL2, so there's no need to check for that.
	 */

	/**
	 * Also note that this is really a most inefficient way of updating the vertex buffer.
	 * Basically creating all vertices on the stack, passing them in here to be copied
	 * into an internal vertex buffer, and then updating that internal buffer to server
	 * side resources. It could happily be updated directly from stack values.
	 *
	 * Alternatively, sub-allocating from a very large pool of vertices and then using
	 * an offset (index value start) for the actual draw call is an even better way of
	 * doing things. Most text doesn't change per-frame.
	 */

	float *verts = static_cast<float *>(_vertexBuffer.getData());
	for (uint32_t i = 0; i < 12; ++i) {
		verts[i] = pos[i];
	}
	for (uint32_t i = 0; i < 8; ++i) {
		verts[i+12] = uv[i];
	}
	for (uint32_t i = 0; i < 16; ++i) {
		verts[i+20] = rgba[i];
	}
	_vertexBuffer.updateGLBound();
	glDrawArrays(_type, 0, _vertexBuffer.getCount());
}

} // End of namespace Mesh

} // End of namespace Graphics
