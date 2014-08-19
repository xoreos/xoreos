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
	//ctor
}

VertexBuffer::VertexBuffer(const VertexBuffer &other) : _data(0) {
	*this = other;
}

VertexBuffer::~VertexBuffer() {
	if (_data)
		std::free(_data);
}

VertexBuffer &VertexBuffer::operator=(const VertexBuffer &other) {
	if (this != &other) {
		setVertexDecl(other._decl);
		setSize(other._count, other._size);
		memcpy(_data, other._data, other._count * other._size);
	}
	return *this;
}

void VertexBuffer::setSize(uint32 vertCount, uint32 vertSize) {
	_count = vertCount;
	_size = vertSize;

	if (_data)
		std::free(_data);

	if (_count * _size)
		_data = std::malloc(_count * _size);
	else
		_data = 0;
}

void VertexBuffer::setVertexDecl(const VertexDecl &decl) {
	_decl = decl;
}

GLvoid *VertexBuffer::getData() {
	return _data;
}

const GLvoid *VertexBuffer::getData() const {
	return _data;
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
