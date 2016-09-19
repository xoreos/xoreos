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
 *  An ABC/SBM font, as used by Jade Empire.
 */

#ifndef GRAPHICS_AURORA_ABCFONT_H
#define GRAPHICS_AURORA_ABCFONT_H

#include <map>

#include "src/common/types.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/texturehandle.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

/** An ABC/SBM font, as used by Jade Empire. */
class ABCFont : public Graphics::Font {
public:
	ABCFont(const Common::UString &name);
	~ABCFont();

	float getWidth (uint32 c) const;
	float getHeight()         const;

	void draw(uint32 c) const;

private:
	/** A font character. */
	struct Char {
		uint32 dataX;
		uint32 dataY;
		uint8  width;
		int8   spaceL;
		int8   spaceR;

		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	TextureHandle _texture;

	uint8 _base;

	Char _invalid;
	Char _ascii[128];
	std::map<uint32, Char> _extended;

	void load(const Common::UString &name);

	void readCharDesc(Char &c, Common::SeekableReadStream &abc);
	void calcCharVertices(Char &c);

	const Char &findChar(uint32 c) const;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ABCFONT_H
