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
 *  Class for rendering swf fonts.
 */

#include "src/common/bitstream.h"
#include "src/common/encoding.h"
#include "src/common/rect.h"

#include "src/aurora/gfxfile.h"

#include "src/graphics/aurora/flashfont.h"

#ifdef ENABLE_CAIRO

static const cairo_user_data_key_t kUserDataKey = {0};

namespace Graphics {

namespace Aurora {

FlashFont::FlashFont(::Aurora::GFXCharacter::Font font) {
	_ascent = static_cast<double>(font.fontAscent.get());
	_descent = static_cast<double>(font.fontDescent.get());
	_advanceTable = font.advanceTable.get();
	_kernings = font.kerningCodes;

	for (const auto &glyph : font.glyphs) {
		_glyphs[glyph.code].glyph = glyph;

		_glyphs[glyph.code].width = 0.0;
		double x = 0.0;
		for (const auto &shapeRecord : glyph.shapeRecords) {
			if (shapeRecord.move.deltaX || shapeRecord.move.deltaY) {
				x = static_cast<double>(shapeRecord.move.deltaX) / 1024.0;
			} else if (shapeRecord.straightEdge.deltaX || shapeRecord.straightEdge.deltaY) {
				x += static_cast<double>(shapeRecord.straightEdge.deltaX) / 1024.0;
			} else if (shapeRecord.curvedEdge.anchorDeltaX || shapeRecord.curvedEdge.anchorDeltaY) {
				x += (static_cast<double>(shapeRecord.curvedEdge.anchorDeltaX) + static_cast<double>(shapeRecord.curvedEdge.controlDeltaX)) / 1024.0;
			}

			_glyphs[glyph.code].width = MAX(x, _glyphs[glyph.code].width);
		}
	}

	_font = cairo_user_font_face_create();

	if (cairo_font_face_set_user_data(_font, &kUserDataKey, this, 0) != CAIRO_STATUS_SUCCESS)
		throw Common::Exception("Unable to set cairo font face user data");

	cairo_user_font_face_set_render_glyph_func(_font, &FlashFont::renderGlyph);
	cairo_user_font_face_set_text_to_glyphs_func(_font, &FlashFont::textToGlyphs);
}

FlashFont::~FlashFont() {
	cairo_font_face_destroy(_font);
}

cairo_status_t FlashFont::renderGlyph(cairo_scaled_font_t *scaled_font, unsigned long glyph, cairo_t *cr,
	cairo_text_extents_t *extents) {
	FlashFont *font = reinterpret_cast<FlashFont *>(
			cairo_font_face_get_user_data(
					cairo_scaled_font_get_font_face(scaled_font),
					&kUserDataKey
			)
	);

	::Aurora::GFXCharacter::Glyph flashGlyph = font->_glyphs[glyph].glyph;

	const double ascent = font->_ascent / 20.0;
	const double descent = font->_descent / 20.0;

	const double ascentRatio = ascent / (ascent + descent);

	cairo_new_path(cr);

	double x = 0, y = 0;
	for (const auto &shapeRecord : flashGlyph.shapeRecords) {
		if (shapeRecord.move.deltaX || shapeRecord.move.deltaY) {
			x = static_cast<double>(shapeRecord.move.deltaX) / 1024.0;
			y = static_cast<double>(shapeRecord.move.deltaY) / (ascent + descent);
			cairo_move_to(cr, x, y + ascentRatio);
		} else if (shapeRecord.straightEdge.deltaX || shapeRecord.straightEdge.deltaY) {
			x += static_cast<double>(shapeRecord.straightEdge.deltaX) / 1024.0;
			y += static_cast<double>(shapeRecord.straightEdge.deltaY) / (ascent + descent);
			cairo_line_to(cr, x, y + ascentRatio);
		} else if (shapeRecord.curvedEdge.anchorDeltaX || shapeRecord.curvedEdge.anchorDeltaY) {
			double x0, y0;
			cairo_get_current_point(cr, &x0, &y0);

			double x1 = x + static_cast<double>(shapeRecord.curvedEdge.controlDeltaX) / 1024.0;
			double y1 = y + static_cast<double>(shapeRecord.curvedEdge.controlDeltaY) / (ascent + descent);
			double x2 = x + (static_cast<double>(shapeRecord.curvedEdge.anchorDeltaX)
			        + static_cast<double>(shapeRecord.curvedEdge.controlDeltaX)) / 1024.0;
			double y2 = y + (static_cast<double>(shapeRecord.curvedEdge.anchorDeltaY)
			        + static_cast<double>(shapeRecord.curvedEdge.controlDeltaY)) / (ascent + descent);

			x = x2;
			y = y2;

			y1 += ascentRatio;
			y2 += ascentRatio;

			cairo_curve_to(
					cr,
					(x0 + 2.0 * x1) / 3.0,
					(y0 + 2.0 * y1) / 3.0,
					(x2 + 2.0 * x1) / 3.0,
					(y2 + 2.0 * y1) / 3.0,
					x2,
					y2
			);
		}
	}

	cairo_close_path(cr);
	cairo_fill(cr);

	return CAIRO_STATUS_SUCCESS;
}

cairo_status_t FlashFont::textToGlyphs(cairo_scaled_font_t *scaled_font, const char *utf8, int utf8_len,
	cairo_glyph_t **glyphs, int *num_glyphs, cairo_text_cluster_t **UNUSED(clusters), int *UNUSED(num_clusters),
	cairo_text_cluster_flags_t *UNUSED(cluster_flags)) {
	FlashFont *font = reinterpret_cast<FlashFont *>(
			cairo_font_face_get_user_data(
					cairo_scaled_font_get_font_face(scaled_font),
					&kUserDataKey
			)
	);

	const std::map<uint16, Glyph> &glyphmap = font->_glyphs;
	const std::vector<::Aurora::GFXCharacter::KerningCode> &kernings = font->_kernings;

	if (utf8_len < 0)
		return CAIRO_STATUS_USER_FONT_ERROR;

	*glyphs = cairo_glyph_allocate(utf8_len);
	*num_glyphs = utf8_len;

	if (*glyphs == nullptr)
		return CAIRO_STATUS_USER_FONT_ERROR;

	for (int i = 0; i < utf8_len; ++i) {
		const char character = utf8[i];

		cairo_glyph_t &glyph = (*glyphs)[i];
		glyph.index = static_cast<uint16>(character);
		glyph.x = 0.0;
		glyph.y = 0.0;
	}

	double offset = 0.0;
	for (int i = 0; i < MAX(utf8_len - 1, 0); ++i) {
		const uint16 leftCharacter = static_cast<uint16>(utf8[i]);
		const uint16 rightCharacter = static_cast<uint16>(utf8[i + 1]);

		std::vector<::Aurora::GFXCharacter::KerningCode>::const_iterator kerningIter = std::find_if(
				kernings.begin(),
				kernings.end(),
				[leftCharacter, rightCharacter]
				(const ::Aurora::GFXCharacter::KerningCode &kerning){
					return kerning.code1 == leftCharacter && kerning.code2 == rightCharacter;
				}
		);

		cairo_glyph_t &leftGlyph = (*glyphs)[i];
		cairo_glyph_t &rightGlyph = (*glyphs)[i + 1];

		if (kerningIter != kernings.end()) {
			const ::Aurora::GFXCharacter::KerningCode &kerning = *kerningIter;

			offset += (static_cast<double>(kerning.adjustment) / 20.0) / 1024.0;
		}

		const double advance = (static_cast<double>(font->_advanceTable[leftCharacter]) / 20.0) / 1024.0;

		const Glyph glyph = glyphmap.at(leftGlyph.index);
		if (!glyph.width)
			offset += advance;
		else
			offset += glyph.width;
		rightGlyph.x = offset;
	}

	return CAIRO_STATUS_SUCCESS;
}

} // End of namespace Aurora

} // End of namespace Graphics

#endif // ENABLE_CAIRO
