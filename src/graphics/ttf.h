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
 *  A simple TTF renderer outputting BGRA.
 */

#ifndef GRAPHICS_TTF_H
#define GRAPHICS_TTF_H

#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "src/common/types.h"
#include "src/common/readstream.h"

namespace Graphics {

class Surface;

class TTFRenderer {
public:
	/** Create a TTF renderer with this TTF in the specified line height in pixels. */
	TTFRenderer(Common::SeekableReadStream &ttfFile, int height);
	~TTFRenderer();

	/** Return the height of a character in pixels. */
	int getHeight() const;
	/** Return the max width of a character in pixels. */
	int getMaxWidth() const;

	/** Does the font have the specified character? */
	bool hasChar(uint32_t ch) const;

	/** Return the width of a specific character in pixels. */
	int getCharWidth(uint32_t ch) const;

	/** Draw a specific character onto a BGRA surface. */
	void drawCharacter(uint32_t ch, Surface &surface, int x, int y);

private:
	FT_Library _library;
	FT_Face _face;

	std::unique_ptr<byte[]> _fileBuffer;

	int _width, _height;
	int _ascent, _descent;

	void getFaceMetrics(int &advance, int &yOffset, int &xMin) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_TTF_H
