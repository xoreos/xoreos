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

/** @file graphics/indexbuffer.cpp
 *  A index buffer implementation.
 */

#include <cstdlib>
#include <cstring>

#include "graphics/indexbuffer.h"

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
	memcpy(_data, other._data, other._count * other._size);
	return *this;
}

void IndexBuffer::setSize(uint32 indexCount, uint32 indexSize, GLenum indexType) {
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
	return _data;
}

const GLvoid *IndexBuffer::getData() const {
	return (const GLvoid *) _data;
}

uint32 IndexBuffer::getCount() const {
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

GLuint IndexBuffer::getIBO() {
	return _ibo;
}

}
