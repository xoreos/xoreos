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
 *  A handle to an Aurora texture.
 */

#ifndef GRAPHICS_AURORA_TEXTUREHANDLE_H
#define GRAPHICS_AURORA_TEXTUREHANDLE_H

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Graphics {

namespace Aurora {

class Texture;

/** A managed texture, storing how often it's referenced. */
struct ManagedTexture {
	Texture *texture;
	uint32_t referenceCount;

	ManagedTexture(Texture *t);
	~ManagedTexture();
};

typedef std::map<Common::UString, ManagedTexture *> TextureMap;

/** A handle to a texture. */
class TextureHandle {
public:
	TextureHandle();
	TextureHandle(const TextureHandle &right);
	~TextureHandle();

	TextureHandle &operator=(const TextureHandle &right);

	bool empty() const;
	const Common::UString &getName() const;

	void clear();

	Texture &getTexture() const;

private:
	bool _empty;
	TextureMap::iterator _it;

	TextureHandle(TextureMap::iterator &i);

	friend class TextureManager;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXTUREHANDLE_H
