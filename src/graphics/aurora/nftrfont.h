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

	float getWidth (uint32 c) const;
	float getHeight()         const;

	void draw(uint32 c) const;

private:
	struct Header {
		uint8 width;
		uint8 height;
		uint8 encoding;

		uint16 palMode;

		uint32 offsetCGLP;
		uint32 offsetCWDH;
		uint32 offsetCMAP;
	};

	struct Glyph {
		Common::SeekableReadStream *data;

		uint16 palMode;

		uint8 width;
		uint8 height;
		uint8 depth;

		uint8 advance;

		uint32 character;

		Glyph();
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

	std::map<uint32, Char> _chars;

	float _missingWidth;

	uint32 _height;


	void load(Common::SeekableSubReadStreamEndian &nftr);

	void readHeader  (Common::SeekableSubReadStreamEndian &nftr, Header &header);
	void readInfo    (Common::SeekableSubReadStreamEndian &nftr, Header &header);
	void readGlyphs  (Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);
	void readWidths  (Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);
	void readCharMaps(Common::SeekableSubReadStreamEndian &nftr, Header &header, std::vector<Glyph> &glyphs);

	void drawGlyphs(const std::vector<Glyph> &glyphs);
	void drawGlyph(const Glyph &glyph, Surface &surface, uint32 x, uint32 y);

	void drawMissing() const;

	static uint32 convertToUTF32(uint16 codePoint, uint8 encoding);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_NFTRFONT_H
