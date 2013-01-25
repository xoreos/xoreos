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

/** @file graphics/indexbuffer.h
 *  A index buffer.
 */

#ifndef GRAPHICS_INDEXBUFFER_H
#define GRAPHICS_INDEXBUFFER_H

#include "graphics/types.h"

namespace Graphics {

/** Buffer containing indices data */
class IndexBuffer {
public:
	IndexBuffer();

	IndexBuffer(const IndexBuffer &other);

	~IndexBuffer();

	IndexBuffer &operator=(const IndexBuffer &other);

	/** Change buffer size. Will allocate memory, free previous */
	void setSize(uint32 indexCount, uint32 indexSize, GLenum indexType);

	/** Access buffer data */
	GLvoid *getData();

	/** Access buffer data */
	const GLvoid *getData() const;

	/** Get element count */
	uint32 getCount() const;

	/** Get element type */
	GLenum getType() const;

private:
	uint32 _count; ///< Number of elements in buffer
	uint32 _size;  ///< Size of a buffer element in bytes
	GLenum _type;  ///< Element type (GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, ...)
	GLvoid *_data; ///< Buffer data
};

}

#endif // GRAPHICS_INDEXBUFFER_H
