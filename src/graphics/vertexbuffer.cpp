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

/** @file graphics/vertexbuffer.cpp
 *  Vertex buffer implementation.
 */

#include <cstdlib>
#include <cstring>

#include "graphics/vertexbuffer.h"

namespace Graphics {

VertexBuffer::VertexBuffer() : _count(0), _size(0), _data(0), _vbo(0) {
}

VertexBuffer::VertexBuffer(const VertexBuffer &other) : _data(0), _vbo(0) {
	*this = other;
}

VertexBuffer::~VertexBuffer() {
	destroyGL(); // Dangerous if GL components not already freed and we're not in the GL context thread.
	std::free(_data);
}

VertexBuffer &VertexBuffer::operator=(const VertexBuffer &other) {
	if (this == &other)
		return *this;

	setVertexDecl(other._decl);
	setSize(other._count, other._size);
	memcpy(_data, other._data, other._count * other._size);

	// Fix up the data pointers

	std::vector<VertexAttrib>::const_iterator src = other._decl.begin();
	std::vector<VertexAttrib>::iterator dest = _decl.begin();
	while ((src != other._decl.end()) && (dest != _decl.end())) {
		dest->pointer = (byte *)_data + ((byte *)src->pointer - (byte *)other._data);

		++src;
		++dest;
	}

	return *this;
}

void VertexBuffer::setSize(uint32 vertCount, uint32 vertSize) {
	_count = vertCount;
	_size  = vertSize;

	std::free(_data);
	_data = 0;

	if (_count && _size) {
		_data = (byte *)(std::malloc(_count * _size));  // Ensures correct alignment.
	}
}

void VertexBuffer::setVertexDecl(const VertexDecl &decl) {
	_decl = decl;
}

GLvoid *VertexBuffer::getData() {
	return _data;
}

const GLvoid *VertexBuffer::getData() const {
	return (const GLvoid *) _data;
}

const VertexDecl &VertexBuffer::getVertexDecl() const {
	return _decl;
}

uint32 VertexBuffer::getCount() const {
	return _count;
}

uint32 VertexBuffer::getSize() const {
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

void VertexBuffer::destroyGL() {
	if (_vbo != 0) {
		glDeleteBuffers(1, &_vbo);
		_vbo = 0;
	}
}

GLuint VertexBuffer::getVBO() {
	return _vbo;
}

}
