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
 *  A index buffer implementation.
 */

#include <cstdlib>
#include <cstring>

#include "src/graphics/indexbuffer.h"

namespace Graphics {

IndexBuffer::IndexBuffer() : _count(0), _size(0), _type(GL_UNSIGNED_INT), _data(0), _ibo(0), _hint(GL_STATIC_DRAW) {
}

IndexBuffer::IndexBuffer(const IndexBuffer &other) : _data(0), _ibo(0), _hint(GL_STATIC_DRAW) {
	*this = other;
}

IndexBuffer::~IndexBuffer() {
	destroyGL();
	delete[] _data;
}

IndexBuffer &IndexBuffer::operator=(const IndexBuffer &other) {
	if (this == &other)
		return *this;

	setSize(other._count, other._size, other._type);

	/* _data, _count, _size and _type are always only ever set by the setSize()
	 * method which is also called above to replicate the situation in the right-
	 * hand side IndexBuffer of the assignment. _data is set to a new allocated
	 * block of memory of count * size size, but only if both count and size > 0.
	 *
	 * We can never run into a situation here where _data == 0 and other._data
	 * != 0 or vice versa. Either both are == 0 (in which case count * size == 0)
	 * or both are != 0. */

	if (_data && other._data)
		memcpy(_data, other._data, other._count * other._size);

	return *this;
}

void IndexBuffer::setSize(uint32_t indexCount, uint32_t indexSize, GLenum indexType) {
	_count = indexCount;
	_size  = indexSize;
	_type  = indexType;

	delete[] _data;
	_data = 0;

	if (_count && _size) {
		_data = new byte[_count * _size];
	}
}

GLvoid *IndexBuffer::getData() {
	return static_cast<GLvoid *>(_data);
}

const GLvoid *IndexBuffer::getData() const {
	return static_cast<const GLvoid *>(_data);
}

uint32_t IndexBuffer::getCount() const {
	return _count;
}

GLenum IndexBuffer::getType() const {
	return _type;
}

void IndexBuffer::initGL(GLuint hint) {
	if (_ibo != 0) {
		return; // Already initialised.
	}

	_hint = hint;
	if (_count) {
		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * _size, _data, _hint);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Return to default buffer.
	}
}

void IndexBuffer::updateGL() {
	if (_count) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * _size, _data, _hint);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Return to default buffer. Maybe this isn't required.
	}
}

void IndexBuffer::destroyGL() {
	if (_ibo != 0) {
		glDeleteBuffers(1, &_ibo);
		_ibo = 0;
	}
}

GLuint IndexBuffer::getIBO() const {
	return _ibo;
}

} // End of namespace Graphics
