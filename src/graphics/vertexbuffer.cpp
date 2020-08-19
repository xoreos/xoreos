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
 *  Vertex buffer implementation.
 */

#include <cstdlib>
#include <cstring>
#include <cassert>

#include "src/graphics/vertexbuffer.h"
#include "src/graphics/indexbuffer.h"

namespace Graphics {

GLvoid *VertexAttrib::getData() {
	return const_cast<GLvoid *>(pointer);
}

const GLvoid *VertexAttrib::getData() const {
	return pointer;
}

void VertexAttrib::enable() const {
	switch (index) {
		case VPOSITION:
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(size, type, stride, pointer);
			break;

		case VNORMAL:
			assert(size == 3);
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(type, stride, pointer);
			break;

		case VCOLOR:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(size, type, stride, pointer);
			break;

		// Unused (experimental renderer only)
		case VBONEINDICES:
		case VBONEWEIGHTS:
			break;

		default:
			assert(index >= VTCOORD);
			glClientActiveTextureARB(GL_TEXTURE0 + index - VTCOORD);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(size, type, stride, pointer);
			break;
	}
}

void VertexAttrib::disable() const {
	switch (index) {
		case VPOSITION:
			glDisableClientState(GL_VERTEX_ARRAY);
			break;

		case VNORMAL:
			glDisableClientState(GL_NORMAL_ARRAY);
			break;

		case VCOLOR:
			glDisableClientState(GL_COLOR_ARRAY);
			break;

		// Unused (experimental renderer only)
		case VBONEINDICES:
		case VBONEWEIGHTS:
			break;

		default:
			assert(index >= VTCOORD);
			glClientActiveTextureARB(GL_TEXTURE0 + index - VTCOORD);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
	}
}


VertexBuffer::VertexBuffer() : _count(0), _size(0), _data(0), _vbo(0), _hint(GL_STATIC_DRAW) {
}

VertexBuffer::VertexBuffer(const VertexBuffer &other) : _data(0), _vbo(0), _hint(GL_STATIC_DRAW) {
	*this = other;
}

VertexBuffer::~VertexBuffer() {
	destroyGL(); // Dangerous if GL components not already freed and we're not in the GL context thread.
	delete[] _data;
}

VertexBuffer &VertexBuffer::operator=(const VertexBuffer &other) {
	if (this == &other)
		return *this;

	setVertexDecl(other._decl);
	setSize(other._count, other._size);

	/* _data, _count and _size are always only ever set by the setSize() method,
	 * which is also called above to replicate the situation in the right-hand
	 * side VertexBuffer of the assignment. _data is set to a new allocated block
	 * of memory of count * size size, but only if both count and size > 0.
	 *
	 * We can never run into a situation here where _data == 0 and other._data
	 * != 0 or vice versa. Either both are == 0 (in which case count * size == 0)
	 * or both are != 0. */

	if (!_data || !other._data)
		return *this;

	memcpy(_data, other._data, other._count * other._size);

	// Fix up the data pointers

	std::vector<VertexAttrib>::const_iterator src = other._decl.begin();
	std::vector<VertexAttrib>::iterator dest = _decl.begin();
	while ((src != other._decl.end()) && (dest != _decl.end())) {
		const byte *ptr = _data + (reinterpret_cast<const byte *>(src->pointer) - other._data);

		dest->pointer = reinterpret_cast<const GLvoid *>(ptr);

		++src;
		++dest;
	}

	return *this;
}

void VertexBuffer::setSize(uint32_t vertCount, uint32_t vertSize) {
	_count = vertCount;
	_size  = vertSize;

	delete[] _data;
	_data = 0;

	if (_count && _size) {
		_data = new byte[_count * _size];
	}
}

void VertexBuffer::setVertexDecl(const VertexDecl &decl) {
	_decl = decl;
}

uint32_t VertexBuffer::getTypeSize(GLenum type) {
	switch (type) {
		case GL_UNSIGNED_BYTE:
			return 1;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_2_BYTES:
			return 2;
		case GL_3_BYTES:
			return 3;
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
		case GL_4_BYTES:
			return 4;
		case GL_DOUBLE:
			return 8;
		default:
			break;
	}

	return 0;
}

void VertexBuffer::setVertexDeclLinear(uint32_t vertCount, VertexDecl &decl) {
	uint32_t vertSize = 0;
	_decl.clear();
	for (VertexDecl::iterator a = decl.begin(); a != decl.end(); ++a)
		vertSize += a->size * getTypeSize(a->type);

	setSize(vertCount, vertSize);

	byte *data = _data;
	for (VertexDecl::iterator a = decl.begin(); a != decl.end(); ++a) {
		a->stride  = 0;
		a->pointer = data;

		data += vertCount * a->size * getTypeSize(a->type);
		_decl.push_back(*a);
	}
}

void VertexBuffer::setVertexDeclInterleave(uint32_t vertCount, VertexDecl &decl) {
	uint32_t vertSize = 0;
	_decl.clear();
	for (VertexDecl::iterator a = decl.begin(); a != decl.end(); ++a)
		vertSize += a->size * getTypeSize(a->type);

	setSize(vertCount, vertSize);

	uint32_t offset = 0;
	for (VertexDecl::iterator a = decl.begin(); a != decl.end(); ++a) {
		a->stride  = vertSize;
		a->pointer = _data + offset;

		offset += a->size * getTypeSize(a->type);
		_decl.push_back(*a);
	}
}

GLvoid *VertexBuffer::getData() {
	return static_cast<GLvoid *>(_data);
}

const GLvoid *VertexBuffer::getData() const {
	return static_cast<const GLvoid *>(_data);
}

GLvoid *VertexBuffer::getData(size_t vertexDecl) {
	assert(vertexDecl < _decl.size());

	return _decl[vertexDecl].getData();
}

const GLvoid *VertexBuffer::getData(size_t vertexDecl) const {
	assert(vertexDecl < _decl.size());

	return _decl[vertexDecl].getData();
}

const VertexDecl &VertexBuffer::getVertexDecl() const {
	return _decl;
}

uint32_t VertexBuffer::getCount() const {
	return _count;
}

uint32_t VertexBuffer::getSize() const {
	return _size;
}

void VertexBuffer::initGL(GLuint hint) {
	if (_vbo != 0) {
		return; // Already initialised.
	}

	_hint = hint;
	if (_count) {
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _count * _size, _data, _hint);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Return to default buffer.
	}
}

void VertexBuffer::updateGL() {
	if (_count) {
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _count * _size, _data, _hint);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Return to default buffer. Maybe this isn't required.
	}
}

void VertexBuffer::updateGLBound() const {
	glBufferSubData(GL_ARRAY_BUFFER, 0, _count * _size, _data);
}

void VertexBuffer::destroyGL() {
	if (_vbo != 0) {
		glDeleteBuffers(1, &_vbo);
		_vbo = 0;
	}
}

GLuint VertexBuffer::getVBO() const {
	return _vbo;
}

void VertexBuffer::draw(GLenum mode, const IndexBuffer &indexBuffer) const {
	if ((getCount() == 0) || (indexBuffer.getCount() == 0))
		return;

	for (VertexDecl::const_iterator d = _decl.begin(); d != _decl.end(); ++d)
		d->enable();

	glDrawElements(mode, indexBuffer.getCount(), indexBuffer.getType(), indexBuffer.getData());

	for (VertexDecl::const_iterator d = _decl.begin(); d != _decl.end(); ++d)
		d->disable();
}

} // End of namespace Graphics
