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
 *  A index buffer.
 */

#ifndef GRAPHICS_INDEXBUFFER_H
#define GRAPHICS_INDEXBUFFER_H

#include "src/graphics/types.h"

namespace Graphics {

/** Buffer containing indices data. */
class IndexBuffer {
public:
	IndexBuffer();

	IndexBuffer(const IndexBuffer &other);

	~IndexBuffer();

	IndexBuffer &operator=(const IndexBuffer &other);

	/** Change buffer size. Will allocate memory, free previous. */
	void setSize(uint32_t indexCount, uint32_t indexSize, GLenum indexType);

	/** Access buffer data. */
	GLvoid *getData();

	/** Access buffer data. */
	const GLvoid *getData() const;

	/** Get element count. */
	uint32_t getCount() const;

	/** Get element type. */
	GLenum getType() const;

	/** Initialise internal buffer object for GL handling. */
	void initGL(GLuint hint = GL_STATIC_DRAW);

	/** Update existing GL buffer object. Try not to call while rendering. */
	void updateGL();

	/** Clear (destroy) GL resources associated with the buffer. */
	void destroyGL();

	GLuint getIBO() const;

private:
	uint32_t _count; ///< Number of elements in buffer.
	uint32_t _size;  ///< Size of a buffer element in bytes.
	GLenum _type;  ///< Element type (GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, ...).
	byte  *_data;  ///< Buffer data.

	GLuint _ibo;   ///< "Index" Buffer Object.
	GLuint _hint;  ///< GL hint for static or dynamic data.
};

} // End of namespace Graphics

#endif // GRAPHICS_INDEXBUFFER_H
