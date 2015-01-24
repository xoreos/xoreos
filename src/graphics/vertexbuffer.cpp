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

VertexBuffer::VertexBuffer() : _count(0), _size(0), _data(0) {
}

VertexBuffer::VertexBuffer(const VertexBuffer &other) : _data(0) {
	*this = other;
}

VertexBuffer::~VertexBuffer() {
	delete[] _data;
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

	delete[] _data;
	_data = 0;

	if (_count && _size)
		_data = new byte[_count * _size];
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

}
