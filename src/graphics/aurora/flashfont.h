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

#ifndef GRAPHICS_AURORA_FLASHFONT_H
#define GRAPHICS_AURORA_FLASHFONT_H

#ifdef ENABLE_CAIRO

#include <map>
#include <vector>

#include <cairo/cairo.h>

#include "src/common/readstream.h"

#include "src/aurora/gfxfile.h"

#include "src/graphics/font.h"
#include "src/graphics/images/surface.h"
#include "src/graphics/aurora/texturehandle.h"

#ifndef CAIRO_HAS_USER_FONT
#error "Cairo needs to be compiled with user font support"
#endif // CAIRO_HAS_USER_FONT

namespace Graphics {

namespace Aurora {

class FlashFont {
public:
	/** Create a new flash font based on a font extracted from a gfx
	 *  file
	 *
	 *  @param font the font to render
	 */
	explicit FlashFont(::Aurora::GFXCharacter::Font font);
	~FlashFont();

private:
	/** A helper struct for the glyphs. */
	struct Glyph {
		::Aurora::GFXCharacter::Glyph glyph;
		double width;
	};

	/** Bindable render_glyph method for cairo user font. */
	static cairo_status_t renderGlyph(
			cairo_scaled_font_t *scaled_font,
			unsigned long  glyph,
			cairo_t *cr,
			cairo_text_extents_t *extents
	);

	/** Bindable text_to_glyphs method for cairo user font. */
	static cairo_status_t textToGlyphs(
			cairo_scaled_font_t *scaled_font,
			const char *utf8,
			int utf8_len,
			cairo_glyph_t **glyphs,
			int *num_glyphs,
			cairo_text_cluster_t **clusters,
			int *num_clusters,
			cairo_text_cluster_flags_t *cluster_flags
	);

	/** The used defined cairo font. */
	cairo_font_face_t *_font;

	/** Distance between the font baseline and the top. */
	double _ascent;
	/** Distance between the font baseline and the bottom. */
	double _descent;

	/** Advance values of font. */
	std::vector<int16> _advanceTable;
	/** Kerning values of the font. */
	std::vector<::Aurora::GFXCharacter::KerningCode> _kernings;

	std::map<uint16, Glyph> _glyphs;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // ENABLE_CAIRO

#endif // GRAPHICS_AURORA_FLASHFONT_H
