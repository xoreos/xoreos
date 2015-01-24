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

IndexBuffer::IndexBuffer() : _count(0), _size(0), _type(GL_UNSIGNED_INT), _data(0) {
}

IndexBuffer::IndexBuffer(const IndexBuffer &other) : _data(0) {
	*this = other;
}

IndexBuffer::~IndexBuffer() {
	delete[] _data;
}

IndexBuffer &IndexBuffer::operator=(const IndexBuffer &other) {
	if (this == &other)
	return *this;

	setSize(other._count, other._size, other._type);
	memcpy(_data, other._data, other._count * other._size);
}

void IndexBuffer::setSize(uint32 indexCount, uint32 indexSize, GLenum indexType) {
	_count = indexCount;
	_size  = indexSize;
	_type  = indexType;

	delete[] _data;
	_data = 0;

	if (_count && _size)
		_data = new byte[_count * _size];
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

}
