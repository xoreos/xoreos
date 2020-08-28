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
 *  Nintendo's NFTR font, found in Sonic.
 */

#ifndef GRAPHICS_AURORA_NFTRFONT_H
#define GRAPHICS_AURORA_NFTRFONT_H

#include <vector>
#include <map>

#include "src/common/types.h"

#include "src/aurora/nitrofile.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/texturehandle.h"

namespace Graphics {

class Surface;

namespace Aurora {

class NFTRFont : public Graphics::Font, public ::Aurora::NitroFile {
public:
	NFTRFont(Common::SeekableReadStream *nftr, bool invertPalette = false);
	NFTRFont(const Common::UString &name, bool invertPalette = false);
	~NFTRFont();

	float getWidth (uint32_t c) const;
	float getHeight()           const;

	void draw(uint32_t c) const;

private:
	struct Header {
		uint8_t width;
		uint8_t height;
		uint8_t encoding;

		uint16_t palMode;

		uint32_t offsetCGLP;
		uint32_t offsetCWDH;
		uint32_t offsetCMAP;
	};

	struct Glyph {
		Common::SeekableReadStream *data;

		uint16_t palMode;

		uint8_t width;
		uint8_t height;
		uint8_t depth;

		uint8_t advance;

		uint32_t character;

		Glyph();
		Glyph(const Glyph &glyph) = default;
		~Glyph();
	};

	/** A font character. */
	struct Char {
		float width;

		float tX[4], tY[4];
		float vX[4], vY[4];
	};


	bool _invertPalette;

	Surface *_surface;
	TextureHandle _texture;

	std::map<uint32_t, Char> _chars;

	float _missingWidth;

	uint32_t _height;


	void load(Common::SeekableSubReadStreamEndian &nftr);

	void readHeader  (Common::SeekableSubReadStreamEndian &nftr, Header &header);
	void readInfo    (Common::SeekableSubReadStreamEndian &nftr, Header &header);
	void readGlyphs  (Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);
	void readWidths  (Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);
	void readCharMaps(Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);

	void drawGlyphs(const std::vector<Glyph> &glyphs);
	void drawGlyph(const Glyph &glyph, Surface &surface, uint32_t x, uint32_t y);

	void drawMissing() const;

	static uint32_t convertToUTF32(uint16_t codePoint, uint8_t encoding);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NFTRFONT_H
