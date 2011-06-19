/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/ttf.h
 *  A simple TTF renderer outputting BGRA.
 */

#ifndef GRAPHICS_TTF_H
#define GRAPHICS_TTF_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "common/types.h"
#include "common/stream.h"

namespace Graphics {

class TTFRenderer {
public:
	/** Create a TTF renderer with this TTF in the specified line height in pixels. */
	TTFRenderer(Common::SeekableReadStream &ttfFile, int height);
	~TTFRenderer();

	/** Return the height of a character in pixels. */
	int getHeight() const;
	/** Return the max width of a character in pixels. */
	int getMaxWidth() const;

	/** Return the width of a specific character in pixels. */
	int getCharWidth(uint32 ch) const;

	/** Draw a specific character onto a BGRA surface.
	 *
	 *  @param ch The character to draw.
	 *  @param dst The memory location of the surface to draw to.
	 *  @param The pitch of the surface in pixels.
	 *  @param The max width of the character to draw in pixels.
	 *  @param The max height of the character to draw in pixels.
	 */
	void drawCharacter(uint32 ch, uint8 *dst, uint32 pitch, int maxWidth, int maxHeight);

private:
	FT_Library _library;
	FT_Face _face;

	byte *_fileBuffer;

	int _width, _height;
	int _ascent, _descent;

	void getFaceMetrics(int &advance, int &yOffset, int &xMin) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_TTF_H
