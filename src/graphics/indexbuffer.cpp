/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/indexbuffer.cpp
 *  A index buffer implementation.
 */

#include <cstdlib>

#include "graphics/indexbuffer.h"

namespace Graphics {

IndexBuffer::IndexBuffer() : _count(0), _size(0), _type(GL_UNSIGNED_INT), _data(0) {
	//ctor
}

IndexBuffer::IndexBuffer(const IndexBuffer &other) {
	*this = other;
}

IndexBuffer::~IndexBuffer() {
	if (_data)
		std::free(_data);
}

IndexBuffer &IndexBuffer::operator=(const IndexBuffer &other) {
	if (this != &other) {
		setSize(other._count, other._size, other._type);
		memcpy(_data, other._data, other._count * other._size);
	}
	return *this;
}

void IndexBuffer::setSize(uint32 indexCount, uint32 indexSize, GLenum indexType) {
	_count = indexCount;
	_size = indexSize;
	_type = indexType;

	if (_data)
		std::free(_data);

	if (_count * _size)
		_data = std::malloc(_count * _size);
	else
		_data = NULL;
}

GLvoid *IndexBuffer::getData() {
	return _data;
}

const GLvoid *IndexBuffer::getData() const {
	return _data;
}

uint32 IndexBuffer::getCount() const {
	return _count;
}

GLenum IndexBuffer::getType() const {
	return _type;
}

}
