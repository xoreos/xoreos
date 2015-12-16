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
 *  A texture font, as used by NWN and KotOR/KotOR2.
 */

#ifndef GRAPHICS_AURORA_TEXTUREFONT_H
#define GRAPHICS_AURORA_TEXTUREFONT_H

#include <map>

#include "src/common/types.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/texturehandle.h"

namespace Common {
	class UString;
}

namespace Graphics {

namespace Aurora {

class Texture;

/** A texture font, as used by NWN and KotOR/KotOR2. */
class TextureFont : public Graphics::Font {
public:
	TextureFont(const Common::UString &name);
	~TextureFont();

	float getWidth (uint32 c) const;
	float getHeight()         const;

	float getLineSpacing() const;

	void draw(uint32 c) const;

private:
	/** A font character. */
	struct Char {
		float width;
		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	TextureHandle _texture;

	std::map<uint32, Char> _chars;

	float _height;
	float _spaceR;
	float _spaceB;

	void load();

	void drawMissing() const;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXTUREFONT_H
