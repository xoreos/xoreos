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

/** @file graphics/ttf.cpp
 *  A simple TTF renderer outputting BGRA.
 */

#include "common/util.h"
#include "common/error.h"

#include "graphics/ttf.h"

#include "graphics/images/surface.h"

namespace Graphics {

namespace {

inline int ftFloor26_6(FT_Pos x) {
	return x / 64;
}

inline int ftCeil26_6(FT_Pos x) {
	return (x + 63) / 64;
}

} // End of anonymous namespace

TTFRenderer::TTFRenderer(Common::SeekableReadStream &ttfFile, int height) :
	_library(0), _face(0), _fileBuffer(0),
	_width(0), _height(0), _ascent(0), _descent(0) {

	// Initialize a library object of FreeType2
	if (FT_Init_FreeType(&_library))
		throw Common::Exception("TTFRenderer: Could not init freetype2");

	const uint32 size = ttfFile.size();
	_fileBuffer = new uint8[size];
	if (!_fileBuffer) {
		FT_Done_FreeType(_library);

		throw Common::Exception("TTFRenderer: Out of memory");
	}

	if (ttfFile.read(_fileBuffer, size) != size) {
		FT_Done_FreeType(_library);
		delete[] _fileBuffer;

		throw Common::kReadError;
	}

	if (FT_New_Memory_Face(_library, _fileBuffer, size, 0, &_face)) {
		FT_Done_FreeType(_library);
		delete[] _fileBuffer;

		throw Common::Exception("TTFRenderer: Could not load font file");
	}

	// We only support scalable fonts.
	if (!FT_IS_SCALABLE(_face)) {
		FT_Done_Face(_face);
		FT_Done_FreeType(_library);
		delete[] _fileBuffer;

		throw Common::Exception("TTFRenderer: Font is not scalable");
	}

	// Set the font height
	if (FT_Set_Char_Size(_face, 0, height * 64, 0, 0)) {
		FT_Done_Face(_face);
		FT_Done_FreeType(_library);
		delete[] _fileBuffer;

		throw Common::Exception("TTFRenderer: Setting height to %d failed", height);
	}

	FT_Fixed yScale = _face->size->metrics.y_scale;

	_ascent  = ftCeil26_6(FT_MulFix(_face->ascender, yScale));
	_descent = ftCeil26_6(FT_MulFix(_face->descender, yScale));

	_width  = ftCeil26_6(FT_MulFix(_face->max_advance_width, _face->size->metrics.x_scale));
	_height = _ascent - _descent + 1;
}

TTFRenderer::~TTFRenderer() {
	FT_Done_Face(_face);
	delete[] _fileBuffer;
	FT_Done_FreeType(_library);
}

int TTFRenderer::getHeight() const {
	return _height;
}

int TTFRenderer::getMaxWidth() const {
	return _width;
}

void TTFRenderer::getFaceMetrics(int &advance, int &yOffset, int &xMin) const {
	FT_Glyph_Metrics &metrics = _face->glyph->metrics;

	    xMin = ftFloor26_6(metrics.horiBearingX);
	int xMax = xMin + ftCeil26_6(metrics.width);

	yOffset = _ascent - ftFloor26_6(metrics.horiBearingY);
	advance = ftCeil26_6(metrics.horiAdvance);

	// In case we got a negative xMin we adjust that, this might make some
	// characters look a bit odd, but it's the only way we can ensure no
	// invalid memory gets written to with the current font API
	if (xMin < 0) {
		xMax -= xMin;
		xMin = 0;

		if (xMax > advance)
			advance = xMax;
	}
}

bool TTFRenderer::hasChar(uint32 ch) const {
	return FT_Get_Char_Index(_face, ch) != 0;
}

int TTFRenderer::getCharWidth(uint32 ch) const {
	FT_UInt slot = FT_Get_Char_Index(_face, ch);
	if (!slot)
		throw Common::Exception("TTFRenderer: Font does not contain glyph %x", ch);

	if (FT_Load_Glyph(_face, slot, FT_LOAD_DEFAULT))
		throw Common::Exception("TTFRenderer: Could not load glyph %x", ch);

	int advance, yOffset, xMin;
	getFaceMetrics(advance, yOffset, xMin);

	return advance;
}

void TTFRenderer::drawCharacter(uint32 ch, Surface &surface, int x, int y) {
	FT_UInt slot = FT_Get_Char_Index(_face, ch);
	if (!slot)
		throw Common::Exception("TTFRenderer: Font does not contain glyph %x", ch);

	if (FT_Load_Glyph(_face, slot, FT_LOAD_DEFAULT))
		throw Common::Exception("TTFRenderer: Could not load glyph %x", ch);

	if (FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL))
		throw Common::Exception("TTFRenderer: Could not render glyph %x", ch);

	if (_face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		throw Common::Exception("TTFRenderer: Glyph %x is no bitmap", ch);

	int advance, yOffset, xMin;
	getFaceMetrics(advance, yOffset, xMin);

	const FT_Bitmap &bitmap = _face->glyph->bitmap;
	const uint8 *src = bitmap.buffer;
	int srcPitch = bitmap.pitch;
	if (srcPitch < 0) {
		src += (bitmap.rows - 1) * srcPitch;
		srcPitch = -srcPitch;
	}

	x += xMin;
	y += yOffset;

	const int width  = MIN(surface.getWidth () - x, bitmap.width);
	const int height = MIN(surface.getHeight() - y, bitmap.rows );

	byte *dst = surface.getData() + (y * surface.getWidth() + x) * 4;

	switch (bitmap.pixel_mode) {
	case FT_PIXEL_MODE_GRAY:
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j, dst += 4) {
				// Output BGRA
				dst[0] = dst[1] = dst[2] = 0xFF;
				dst[3] = src[j];
			}

			dst += (surface.getWidth() - width) * 4;
			src += srcPitch;
		}
		break;

	default:
		throw Common::Exception("TTFRenderer: Unsupported pixel mode %d", bitmap.pixel_mode);
	}
}

} // End of namespace Graphics
