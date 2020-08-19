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

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/graphics/ttf.h"

#include "src/graphics/images/surface.h"

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
	_library(0), _face(0), _width(0), _height(0), _ascent(0), _descent(0) {

	// Initialize a library object of FreeType2
	if (FT_Init_FreeType(&_library))
		throw Common::Exception("TTFRenderer: Could not init freetype2");

	bool success = false;
	BOOST_SCOPE_EXIT( (&success) (&_library)) {
		if (!success)
			FT_Done_FreeType(_library);
	} BOOST_SCOPE_EXIT_END

	const size_t size = ttfFile.size();
	_fileBuffer = std::make_unique<uint8_t[]>(size);

	if (ttfFile.read(_fileBuffer.get(), size) != size)
		throw Common::Exception(Common::kReadError);

	if (FT_New_Memory_Face(_library, _fileBuffer.get(), size, 0, &_face))
		throw Common::Exception("TTFRenderer: Could not load font file");

	BOOST_SCOPE_EXIT( (&success) (&_face)) {
		if (!success)
			FT_Done_Face(_face);
	} BOOST_SCOPE_EXIT_END

	// We only support scalable fonts.
	if (!FT_IS_SCALABLE(_face))
		throw Common::Exception("TTFRenderer: Font is not scalable");

	// Set the font height
	if (FT_Set_Char_Size(_face, 0, height * 64, 0, 0))
		throw Common::Exception("TTFRenderer: Setting height to %d failed", height);

	FT_Fixed yScale = _face->size->metrics.y_scale;

	_ascent  = ftCeil26_6(FT_MulFix(_face->ascender, yScale));
	_descent = ftCeil26_6(FT_MulFix(_face->descender, yScale));

	_width  = ftCeil26_6(FT_MulFix(_face->max_advance_width, _face->size->metrics.x_scale));
	_height = _ascent - _descent + 1;

	success = true;
}

TTFRenderer::~TTFRenderer() {
	FT_Done_Face(_face);
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

	xMin    = ftFloor26_6(metrics.horiBearingX);
	yOffset = _ascent - ftFloor26_6(metrics.horiBearingY);
	advance = ftCeil26_6(metrics.horiAdvance);
}

bool TTFRenderer::hasChar(uint32_t ch) const {
	return FT_Get_Char_Index(_face, ch) != 0;
}

int TTFRenderer::getCharWidth(uint32_t ch) const {
	FT_UInt slot = FT_Get_Char_Index(_face, ch);
	if (!slot)
		throw Common::Exception("TTFRenderer: Font does not contain glyph 0x%X", ch);

	int error;
	if ((error = FT_Load_Glyph(_face, slot, FT_LOAD_DEFAULT)))
		throw Common::Exception("TTFRenderer: Could not load glyph 0x%X: 0x%X", ch, error);

	int advance, yOffset, xMin;
	getFaceMetrics(advance, yOffset, xMin);

	return advance;
}

void TTFRenderer::drawCharacter(uint32_t ch, Surface &surface, int x, int y) {
	FT_UInt slot = FT_Get_Char_Index(_face, ch);
	if (!slot)
		throw Common::Exception("TTFRenderer: Font does not contain glyph 0x%X", ch);

	int error;
	if ((error = FT_Load_Glyph(_face, slot, FT_LOAD_DEFAULT)))
		throw Common::Exception("TTFRenderer: Could not load glyph 0x%X: 0x%X", ch, error);

	if ((error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL)))
		throw Common::Exception("TTFRenderer: Could not render glyph 0x%X: 0x%X", ch, error);

	if (_face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		throw Common::Exception("TTFRenderer: Glyph 0x%X is no bitmap", ch);

	int advance, yOffset, xMin;
	getFaceMetrics(advance, yOffset, xMin);

	const FT_Bitmap &bitmap = _face->glyph->bitmap;
	const uint8_t *src = bitmap.buffer;
	int srcPitch = bitmap.pitch;
	if (srcPitch < 0) {
		src += (bitmap.rows - 1) * srcPitch;
		srcPitch = -srcPitch;
	}

	uint bitmapWidth = (uint) bitmap.width;
	if (xMin < 0) {
		bitmapWidth += xMin;
		src         -= xMin;

		xMin = 0;
	}

	x += xMin;
	y += yOffset;

	if ((x >= surface.getWidth()) || (y >= surface.getHeight()))
		return;

	const uint width  = MIN<uint>(surface.getWidth () - x, bitmapWidth);
	const uint height = MIN<uint>(surface.getHeight() - y, bitmap.rows);

	byte *dst = surface.getData() + (y * surface.getWidth() + x) * 4;

	switch (bitmap.pixel_mode) {
	case FT_PIXEL_MODE_GRAY:
		for (uint i = 0; i < height; ++i) {
			for (uint j = 0; j < width; ++j, dst += 4) {
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
