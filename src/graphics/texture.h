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
 *  Virtual base class of a texture.
 */

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"

namespace Graphics {

/** A texture. */
class Texture : public GLContainer {
public:
	Texture();
	~Texture();

	TextureID getID() const;

protected:
	TextureID _textureID; ///< OpenGL texture ID.
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXTURE_H
