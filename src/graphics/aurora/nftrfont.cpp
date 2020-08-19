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

/* Based heavily on the NFTR reader found in the NDS file viewer
 * and editor Tinke by pleoNeX (<https://github.com/pleonex/tinke>),
 * which is licensed under the terms of the GPLv3.
 *
 * Tinke in turn is based on the NFTR documentation by CUE and
 * Lyan53 in the Spanish romxhack forums
 * (<http://romxhack.esforos.com/fuentes-nftr-de-nds-t67>).
 *
 * The original copyright note in Tinke reads as follows:
 *
 * Copyright (C) 2011  pleoNeX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <cstring>

#include <memory>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/ustring.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/resman.h"

#include "src/graphics/images/surface.h"

#include "src/graphics/aurora/nftrfont.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

static const uint32_t kNFTRID = MKTAG('N', 'F', 'T', 'R');
static const uint32_t kFINFID = MKTAG('F', 'I', 'N', 'F');
static const uint32_t kCGLPID = MKTAG('C', 'G', 'L', 'P');
static const uint32_t kCWDHID = MKTAG('C', 'W', 'D', 'H');
static const uint32_t kCMAPID = MKTAG('C', 'M', 'A', 'P');

namespace Graphics {

namespace Aurora {

NFTRFont::Glyph::Glyph() : data(0) {
}

NFTRFont::Glyph::~Glyph() {
	delete data;
}


NFTRFont::NFTRFont(Common::SeekableReadStream *nftr, bool invertPalette) :
	_invertPalette(invertPalette), _surface(0) {

	assert(nftr);

	std::unique_ptr<Common::SeekableSubReadStreamEndian> nftrEndian(open(nftr));

	load(*nftrEndian);
}

NFTRFont::NFTRFont(const Common::UString &name, bool invertPalette) :
	_invertPalette(invertPalette), _surface(0) {

	Common::SeekableReadStream *nftr = ResMan.getResource(name, ::Aurora::kFileTypeNFTR);
	if (!nftr)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	std::unique_ptr<Common::SeekableSubReadStreamEndian> nftrEndian(open(nftr));

	load(*nftrEndian);
}

NFTRFont::~NFTRFont() {
}

void NFTRFont::load(Common::SeekableSubReadStreamEndian &nftr) {
	Header header;
	readHeader(nftr, header);

	std::vector<Glyph> glyphs;
	readGlyphs  (nftr, header, glyphs);
	readWidths  (nftr, header, glyphs);
	readCharMaps(nftr, header, glyphs);

	drawGlyphs(glyphs);

	_height = header.height;

	// Try to find the width of an m. Alternatively, take half of a line's height.
	std::map<uint32_t, Char>::const_iterator m = _chars.find('m');
	_missingWidth = (m != _chars.end()) ? m->second.width : MAX<float>(2.0f, _height / 2);
}

void NFTRFont::readHeader(Common::SeekableSubReadStreamEndian &nftr, Header &header) {
	const uint32_t tag = nftr.readUint32();
	if (tag != kNFTRID)
		throw Common::Exception("Invalid NFTR file (%s)", Common::debugTag(tag).c_str());

	const uint16_t bom = nftr.readUint16();
	if (bom != 0xFEFF)
		throw Common::Exception("Invalid BOM: 0x%04X", (uint) bom);

	const uint8_t versionMinor = nftr.readByte();
	const uint8_t versionMajor = nftr.readByte();
	if ((versionMajor != 1) || ((versionMinor != 0) && (versionMinor != 1)))
		throw Common::Exception("Unsupported version %u.%u", versionMajor, versionMinor);

	const uint32_t fileSize = nftr.readUint32();
	if (fileSize > nftr.size())
		throw Common::Exception("Size too large (%u > %u)", fileSize, (uint)nftr.size());

	nftr.skip(2); // Header size

	header.palMode = nftr.readUint16();

	readInfo(nftr, header);
}

void NFTRFont::readInfo(Common::SeekableSubReadStreamEndian &nftr, Header &header) {
	/* This section contains global information about the font. */

	const uint32_t tag = nftr.readUint32();
	if (tag != kFINFID)
		throw Common::Exception("Invalid info section (%s)", Common::debugTag(tag).c_str());

	nftr.skip(4); // Section size
	nftr.skip(1); // Unknown

	header.height = nftr.readByte();

	nftr.skip(3); // Unknown

	header.width = nftr.readByte();

	nftr.skip(1); // Unknown

	header.encoding = nftr.readByte();

	// We only support UTF-32 (0), UTF-16 (1) and CP1252 (3)
	if ((header.encoding == 2) || (header.encoding > 3))
		throw Common::Exception("Unsupported encoding (%u)", header.encoding);

	// Offsets to the different sections
	header.offsetCGLP = nftr.readUint32() - 8;
	header.offsetCWDH = nftr.readUint32() - 8;
	header.offsetCMAP = nftr.readUint32() - 8;
}

void NFTRFont::readGlyphs(Common::SeekableSubReadStreamEndian &nftr, Header &header,
                          std::vector<Glyph> &glyphs) {

	/* This section contains information about the glyphs, as well as their graphical data. */

	nftr.seek(header.offsetCGLP);

	const uint32_t tag = nftr.readUint32();
	if (tag != kCGLPID)
		throw Common::Exception("Invalid glyph section (%s)", Common::debugTag(tag).c_str());

	const uint32_t sectionSize = nftr.readUint32();

	const uint8_t  width  = nftr.readByte();   // Width of the glyph in pixels
	const uint8_t  height = nftr.readByte();   // Height of the glyph in pixels
	const uint16_t size   = nftr.readUint16(); // Size of the glyph data in bytes

	nftr.skip(2); // Unknown

	const uint8_t depth  = nftr.readByte();

	if ((depth != 1) && (depth != 2) && (depth != 4) && (depth != 8))
		throw Common::Exception("Unsupported glyph depth %u", depth);

	if ((width * height * depth) > (size * 8))
		throw Common::Exception("Glyph can't fit (%u * %u * %u > %d", width, height, depth, size * 8);

	const uint8_t rotateMode = nftr.readByte();
	if (rotateMode != 0)
		throw Common::Exception("Unsupported glyph rotation %u", rotateMode);

	const uint32_t count = (sectionSize - 16) / size;
	glyphs.resize(count);

	size_t offset = nftr.pos();
	for (uint32_t i = 0; i < count; i++, offset += size) {
		glyphs[i].data = new Common::SeekableSubReadStream(&nftr, offset, offset + size);

		// Copy this information into the glyph to make glyph drawing easier later
		glyphs[i].palMode = header.palMode;
		glyphs[i].width   = width;
		glyphs[i].height  = height;
		glyphs[i].depth   = depth;

		// Default advance value, in case the widths section doesn't cover this glyph
		glyphs[i].advance = header.width + 1;

		// In case the character map section doesn't cover this glyph
		glyphs[i].character = 0;
	}
}

void NFTRFont::readWidths(Common::SeekableSubReadStreamEndian &nftr, Header &header,
                          std::vector<Glyph> &glyphs) {

	/* This section provides widths for each glyph, which seems to include the number
	 * of pixels in each row, in which column the glyph starts in and the number of
	 * pixels to advance to the next character.
	 *
	 * Or at least that would be logical. Sometimes, the information doesn't quite
	 * fit. Something fishy is going on...
	 */

	nftr.seek(header.offsetCWDH);

	const uint32_t tag = nftr.readUint32();
	if (tag != kCWDHID)
		throw Common::Exception("Invalid widths section (%s)", Common::debugTag(tag).c_str());

	nftr.skip(4); // Section size

	const uint16_t firstGlyph = nftr.readUint16();
	const uint16_t lastGlyph  = nftr.readUint16();

	nftr.skip(4); // Unknown

	for (uint32_t i = firstGlyph; i <= lastGlyph; i++) {
		if (i >= glyphs.size())
			break;

		nftr.skip(1); // pixel start

		const uint8_t pixelWidth  = nftr.readByte() + 1;
		const uint8_t pixelLength = nftr.readByte() + 1;

		glyphs[i].advance = (pixelWidth == 1) ? pixelLength : pixelWidth;
	}
}

void NFTRFont::readCharMaps(Common::SeekableSubReadStreamEndian &nftr, Header &header,
                            std::vector<Glyph> &glyphs) {

	/* This section provides several character maps, each mapping a range of code points
	 * (in the encoding specified in the header) onto a glyph, in one of three ways:
	 *
	 * - Type 0: Map all code points in the range onto glyphs in order.
	 *           For example: map [40, 45] to glyphs [62, 67].
	 * - Type 1: Map all code points in the range onto each one specific glyph
	 *           For example: map [40, 45] to { 0, 1, 6, 23, 42, 5 }.
	 * - Type 2: Map each a specific code point onto each a specific glyph
	 *           For example. map 40 -> 0, 62 -> 252, 7525 -> 2.
	 *
	 * To make things easier for us, we unroll this information and store the character
	 * it shows (converted to an UTF-32 code point) directly in the Glyph struct.
	 */

	uint32_t nextOffset = header.offsetCMAP;
	while ((nextOffset != 0) && (nextOffset < nftr.size())) {
		nftr.seek(nextOffset);

		const uint32_t tag = nftr.readUint32();
		if (tag != kCMAPID)
			throw Common::Exception("Invalid character map section (%s)", Common::debugTag(tag).c_str());

		const uint32_t sectionSize = nftr.readUint32();

		const uint16_t firstChar = nftr.readUint16();
		const uint16_t lastChar  = nftr.readUint16();
		if (firstChar > lastChar)
			throw Common::Exception("Invalid character map range (%u - %u)", firstChar, lastChar);

		const uint32_t type = nftr.readUint32();
		if (type > 2)
			throw Common::Exception("Invalid character map type %u", type);

		nextOffset = nftr.readUint32() - 8;

		uint32_t count, to, from;
		switch (type) {
			case 0:
				count = lastChar - firstChar + 1;

				to = nftr.readUint16();
				if ((to + count) > glyphs.size())
					throw Common::Exception("Invalid character map range (%u + %u > %u)",
					                        to, count, (uint)glyphs.size());

				for (uint32_t i = 0; i < count; i++)
					glyphs[to + i].character = convertToUTF32(firstChar + i, header.encoding);
				break;

			case 1:
				count = (sectionSize - 20 - 2) / 2;

				for (uint32_t i = 0; i < count; i++) {
					to = nftr.readUint16();
					if (to == 0xFFFF)
						continue;

					if (to >= glyphs.size())
						throw Common::Exception("Invalid character map range (%u > %u)",
						                        to, (uint)glyphs.size());

					glyphs[to].character = convertToUTF32(firstChar + i, header.encoding);
				}
				break;

			case 2:
				count = nftr.readUint16();

				for (uint32_t i = 0; i < count; i++) {
					from = nftr.readUint16();
					to   = nftr.readUint16();
					if (to == 0xFFFF)
						continue;

					if (to >= glyphs.size())
						throw Common::Exception("Invalid character map range (%u > %u)",
						                        to, (uint)glyphs.size());

					glyphs[to].character = convertToUTF32(from, header.encoding);
				}
				break;
		}
	}
}

float NFTRFont::getWidth(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end())
		return _missingWidth;

	return cC->second.width;
}

float NFTRFont::getHeight() const {
	return _height;
}

void NFTRFont::drawMissing() const {
	TextureMan.set();

	const float width = _missingWidth - 1.0f;

	glBegin(GL_QUADS);
		glVertex2f(0.0f ,    0.0f);
		glVertex2f(width,    0.0f);
		glVertex2f(width, _height);
		glVertex2f(0.0f , _height);
	glEnd();

	glTranslatef(width + 1.0f, 0.0f, 0.0f);
}

void NFTRFont::draw(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end()) {
		drawMissing();
		return;
	}

	TextureMan.set(_texture);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC->second.tX[i], cC->second.tY[i]);
		glVertex2f  (cC->second.vX[i], cC->second.vY[i]);
	}
	glEnd();

	glTranslatef(cC->second.width, 0.0f, 0.0f);
}

void NFTRFont::drawGlyphs(const std::vector<Glyph> &glyphs) {
	if (glyphs.empty())
		return;

	/* Calculate the optimal-ish size of a texture holding all glyphs.
	 * This assumes that all glyphs are of the same size, which is true
	 * for NFTR fonts. Or at least, they are bound to a maximal size,
	 * which is good enough for us. Due to the fishy-ness in the glyph
	 * widths section (see above), we don't really know the true sizes
	 * of the glyphs anyway.
	 */

	const uint32_t width  = glyphs[0].width;
	const uint32_t height = glyphs[0].height;
	const uint32_t pixels = glyphs.size() * width * height;

	const uint32_t textureLength = NEXTPOWER2((uint32_t) ceil(sqrt(pixels)));
	if (textureLength > 2048)
		throw Common::Exception("Too many glyphs (%u @ %ux%u)", (uint)glyphs.size(), width, height);

	assert(((textureLength / width) * (textureLength / height)) >= glyphs.size());

	_surface = new Surface(textureLength, textureLength);
	_texture = TextureMan.add(Texture::create(_surface));

	uint32_t x = 0, y = 0;
	for (std::vector<Glyph>::const_iterator g = glyphs.begin(); g != glyphs.end(); ++g) {
		drawGlyph(*g, *_surface, x, y);

		std::pair<std::map<uint32_t, Char>::iterator, bool> result;
		result = _chars.insert(std::make_pair(g->character, Char()));

		Char &ch = result.first->second;

		ch.width = g->advance;

		ch.vX[0] = 0.00f; ch.vY[0] = 0.00f;
		ch.vX[1] = width; ch.vY[1] = 0.00f;
		ch.vX[2] = width; ch.vY[2] = height;
		ch.vX[3] = 0.00f; ch.vY[3] = height;

		const float tX = (float) x      / (float) textureLength;
		const float tY = (float) y      / (float) textureLength;
		const float tW = (float) width  / (float) textureLength;
		const float tH = (float) height / (float) textureLength;

		ch.tX[0] = tX;      ch.tY[0] = tY + tH;
		ch.tX[1] = tX + tW; ch.tY[1] = tY + tH;
		ch.tX[2] = tX + tW; ch.tY[2] = tY;
		ch.tX[3] = tX;      ch.tY[3] = tY;

		x += width;
		if ((x + width) > textureLength) {
			x = 0;
			y += height;
		}
	}

	_texture.getTexture().rebuild();
}

void NFTRFont::drawGlyph(const Glyph &glyph, Surface &surface, uint32_t x, uint32_t y) {
	glyph.data->seek(0);

	const uint32_t maxColors = 1 << glyph.depth;

	byte *dest = surface.getData() + (y * surface.getWidth() + x) * 4;

	uint16_t data = 0xFF00;
	for (uint32_t i = 0; i < glyph.height; i++, dest += 4 * surface.getWidth()) {
		byte *destRow = dest;

		for (uint32_t j = 0; j < glyph.width; j++) {
			// If there's only our canaries left, read the next byte of data
			if (data == 0xFF00)
				data = (glyph.data->readByte() << 8) | 0x00FF;

			const uint16_t pixel = data >> (16 - glyph.depth);

			/* Depending on the NFTR header, the glyphs' palette can differ:
			 * In the one mode, we have a full white to black gradient. This
			 * is used in fonts that use an outline or a shadow. In the other
			 * mode, the lightest color is a gray, so we have more gray values
			 * to work with. This is used in fonts that anti-alias, especially
			 * in higher-res Asian fonts.
			 *
			 * Note that we also guard against a potential division by 0. */
			uint16_t value = 0;
			if (glyph.palMode >= 7)
				value = (maxColors == 2) ? 0xFF : (255 * (pixel    ) / (maxColors - 1));
			else
				value = (maxColors == 2) ? 0xFF : (255 * (pixel - 1) / (maxColors - 2));

			/* If we were requested to, invert the value. Note: the uninverted color
			 * of a font is neither definitely black or definitely white. Both exists,
			 * so an NFTR itself doesn't know how it's supposed to be used. */
			if (_invertPalette)
				value = 255 - value;

			*destRow++ = value;
			*destRow++ = value;
			*destRow++ = value;

			// No matter the inversion or the palette mode, 0 is always transparent
			*destRow++ = pixel == 0 ? 0 : 0xFF;

			data <<= glyph.depth;
		}
	}
}

uint32_t NFTRFont::convertToUTF32(uint16_t codePoint, uint8_t encoding) {
	/* A rather hacky, ugly and slow way to convert a code point in various
	 * encodings to a UTF-32 code point. Yes, this calls iconv() for every
	 * single code point not already in UTF-32.
	 *
	 * This bit is also the sole reason we don't support NFTR in Shift-JIS
	 * encodings. Sonic doesn't seem to use them... */

	if (encoding == 0)
		return codePoint;

	byte data[4];
	memset(data, 0, sizeof(data));

	Common::Encoding e = Common::kEncodingInvalid;

	if (encoding == 1) {
		e = Common::kEncodingUTF16LE;

		WRITE_LE_UINT16(data, codePoint);
	} else if (encoding == 3) {
		e = Common::kEncodingCP1252;

		data[0] = codePoint;
	}

	Common::UString str = Common::readString(data, 4, e);
	if (str.empty())
		return 0;

	return *str.begin();
}

} // End of namespace Aurora

} // End of namespace Graphics
