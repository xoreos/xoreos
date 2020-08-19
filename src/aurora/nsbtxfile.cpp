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
 *  Treat Nintendo NSBTX files, which contain multiple textures as an
 *  archive of intermediate textures.
 */

/* Based heavily on the BTX0 reader found in the NDS file viewer
 * and editor Tinke by pleoNeX (<https://github.com/pleonex/tinke>),
 * which is licensed under the terms of the GPLv3.
 *
 * Tinke in turn is based on the NSBTX documentation by lowlines
 * (<http://llref.emutalk.net/docs/?file=xml/btx0.xml>) and the
 * Nintendo DS technical information GBATEK by Martin Korth
 * (<http://problemkaputt.de/gbatek.htm>).
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

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/encoding.h"

#include "src/aurora/nsbtxfile.h"

static const uint32_t kXEOSID = MKTAG('X', 'E', 'O', 'S');
static const uint32_t kITEXID = MKTAG('I', 'T', 'E', 'X');

static const uint32_t kXEOSITEXHeaderSize       = 4 + 4 + 4 + 4 + 4 + 1 + 1 + 1 + 1 + 1 + 1;
static const uint32_t kXEOSITEXMipMapHeaderSize = 4 + 4 + 4;

static const uint32_t kBTX0ID = MKTAG('B', 'T', 'X', '0');
static const uint32_t kTEX0ID = MKTAG('T', 'E', 'X', '0');

namespace Aurora {

NSBTXFile::ReadContext::ReadContext(Common::SeekableSubReadStreamEndian &n, const Texture &t,
                                    Common::WriteStream &s) :
	texture(&t), nsbtx(&n), stream(&s) {
}

NSBTXFile::ReadContext::~ReadContext() {
}


NSBTXFile::NSBTXFile(Common::SeekableReadStream *nsbtx) {
	assert(nsbtx);

	_nsbtx.reset(open(nsbtx));

	load(*_nsbtx);
}

NSBTXFile::~NSBTXFile() {
}

const Archive::ResourceList &NSBTXFile::getResources() const {
	return _resources;
}

uint32_t NSBTXFile::getITEXSize(const Texture &texture) {
	return kXEOSITEXHeaderSize + kXEOSITEXMipMapHeaderSize + texture.width * texture.height * 4;
}

uint32_t NSBTXFile::getResourceSize(uint32_t index) const {
	if (index >= _textures.size())
		throw Common::Exception("Texture index out of range (%u/%u)", index, (uint)_textures.size());

	return getITEXSize(_textures[index]);
}

void NSBTXFile::writeITEXHeader(const ReadContext &ctx) {
	ctx.stream->writeUint32BE(kXEOSID);
	ctx.stream->writeUint32BE(kITEXID);
	ctx.stream->writeUint32LE(0); // Version
	ctx.stream->writeUint32LE(4); // Pixel format / bytes per pixel

	ctx.stream->writeByte((uint8_t) ctx.texture->wrapX);
	ctx.stream->writeByte((uint8_t) ctx.texture->wrapY);
	ctx.stream->writeByte((uint8_t) ctx.texture->flipX);
	ctx.stream->writeByte((uint8_t) ctx.texture->flipY);
	ctx.stream->writeByte((uint8_t) ctx.texture->coordTransform);

	ctx.stream->writeByte(0x00); // Don't filter the texture

	ctx.stream->writeUint32LE(1); // Number of mip maps

	ctx.stream->writeUint32LE(ctx.texture->width);
	ctx.stream->writeUint32LE(ctx.texture->height);
	ctx.stream->writeUint32LE(ctx.texture->width * ctx.texture->height * 4);
}

void NSBTXFile::writePixel(const ReadContext &ctx, byte r, byte g, byte b, byte a) {
	ctx.stream->writeByte(b);
	ctx.stream->writeByte(g);
	ctx.stream->writeByte(r);
	ctx.stream->writeByte(a);
}

void NSBTXFile::getTexture2bpp(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; ) {

			uint8_t pixels = ctx.nsbtx->readByte();
			for (uint32_t n = 0; n < 4; n++, x++, pixels >>= 2) {
				const uint8_t pixel = pixels & 3;

				const byte r = ctx.palette[pixel * 3 + 0];
				const byte g = ctx.palette[pixel * 3 + 1];
				const byte b = ctx.palette[pixel * 3 + 2];

				const byte a = (ctx.texture->alpha && (pixel == 0)) ? 0x00 : 0xFF;

				writePixel(ctx, r, g, b, a);
			}

		}
	}
}

void NSBTXFile::getTexture4bpp(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; ) {

			uint8_t pixels = ctx.nsbtx->readByte();
			for (uint32_t n = 0; n < 2; n++, x++, pixels >>= 4) {
				const uint8_t pixel = pixels & 0xF;

				const byte r = ctx.palette[pixel * 3 + 0];
				const byte g = ctx.palette[pixel * 3 + 1];
				const byte b = ctx.palette[pixel * 3 + 2];

				const byte a = (ctx.texture->alpha && (pixel == 0)) ? 0x00 : 0xFF;

				writePixel(ctx, r, g, b, a);
			}

		}
	}
}

void NSBTXFile::getTexture8bpp(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; x++) {
			const uint8_t pixel = ctx.nsbtx->readByte();

			const byte r = ctx.palette[pixel * 3 + 0];
			const byte g = ctx.palette[pixel * 3 + 1];
			const byte b = ctx.palette[pixel * 3 + 2];

			const byte a = (ctx.texture->alpha && (pixel == 0)) ? 0x00 : 0xFF;

			writePixel(ctx, r, g, b, a);
		}
	}
}

void NSBTXFile::getTexture16bpp(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; x++) {
			const uint16_t pixel = ctx.nsbtx->readUint16();

			const byte r = ( pixel        & 0x1F) << 3;
			const byte g = ((pixel >>  5) & 0x1F) << 3;
			const byte b = ((pixel >> 10) & 0x1F) << 3;

			const byte a = ((pixel >> 15) == 0) ? 0x00 : 0xFF;

			writePixel(ctx, r, g, b, a);
		}
	}
}

void NSBTXFile::getTextureA3I5(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; x++) {
			const uint8_t pixel = ctx.nsbtx->readByte();

			const uint8_t index = pixel & 0x1F;

			const byte r = ctx.palette[index * 3 + 0];
			const byte g = ctx.palette[index * 3 + 1];
			const byte b = ctx.palette[index * 3 + 2];

			const byte a = (((pixel >> 5) << 2) + (pixel >> 6)) << 3;

			writePixel(ctx, r, g, b, a);
		}
	}
}

void NSBTXFile::getTextureA5I3(const ReadContext &ctx) {
	for (uint32_t y = 0; y < ctx.texture->height; y++) {
		for (uint32_t x = 0; x < ctx.texture->width; x++) {
			const uint8_t pixel = ctx.nsbtx->readByte();

			const uint8_t index = pixel & 0x07;

			const byte r = ctx.palette[index * 3 + 0];
			const byte g = ctx.palette[index * 3 + 1];
			const byte b = ctx.palette[index * 3 + 2];

			const byte a = (pixel >> 3) << 3;

			writePixel(ctx, r, g, b, a);
		}
	}
}

const NSBTXFile::Palette *NSBTXFile::findPalette(const Texture &texture) const {
	std::vector<Common::UString> palNames;

	palNames.push_back(texture.name);
	palNames.push_back(texture.name + "_pl");
	palNames.push_back(texture.name + "_p");
	palNames.push_back(texture.name + "_");

	for (std::vector<Common::UString>::iterator n = palNames.begin(); n != palNames.end(); ++n)
		for (Palettes::const_iterator p = _palettes.begin(); p != _palettes.end(); ++p)
			if (p->name == *n)
				return &*p;

	return 0;
}

void NSBTXFile::getPalette(ReadContext &ctx) const {
	static const uint16_t kPaletteSize[] = { 0, 32, 4, 16, 256, 256, 8,  0 };

	const uint16_t size = kPaletteSize[(size_t)ctx.texture->format] * 3;
	if (size == 0)
		return;

	const Palette *palette = findPalette(*ctx.texture);
	if (!palette)
		throw Common::Exception("Couldn't find a palette for texture \"%s\"", ctx.texture->name.c_str());

	std::unique_ptr<byte[]> palData = std::make_unique<byte[]>(size);
	memset(palData.get(), 0, size);

	ctx.nsbtx->seek(palette->offset);

	const uint16_t palDataSize = MIN<size_t>(size, ((ctx.nsbtx->size() - ctx.nsbtx->pos()) / 2) * 3);

	for (uint16_t i = 0; i < palDataSize; i += 3) {
		const uint16_t pixel = ctx.nsbtx->readUint16();

		palData[i + 0] = ( pixel        & 0x1F) << 3;
		palData[i + 1] = ((pixel >>  5) & 0x1F) << 3;
		palData[i + 2] = ((pixel >> 10) & 0x1F) << 3;
	}

	ctx.palette.reset(palData.release());
}

void NSBTXFile::getTexture(const ReadContext &ctx) {
	ctx.nsbtx->seek(ctx.texture->offset);

	switch (ctx.texture->format) {
		case kFormat2bpp:
			getTexture2bpp(ctx);
			break;

		case kFormat4bpp:
			getTexture4bpp(ctx);
			break;

		case kFormat8bpp:
			getTexture8bpp(ctx);
			break;

		case kFormat16bpp:
			getTexture16bpp(ctx);
			break;

		case kFormatA3I5:
			getTextureA3I5(ctx);
			break;

		case kFormatA5I3:
			getTextureA5I3(ctx);
			break;

		default:
			throw Common::Exception("Unsupported texture format %d", (int) ctx.texture->format);
			break;
	}
}

Common::SeekableReadStream *NSBTXFile::getResource(uint32_t index, bool UNUSED(tryNoCopy)) const {
	if (index >= _textures.size())
		throw Common::Exception("Texture index out of range (%u/%u)", index, (uint)_textures.size());

	Common::MemoryWriteStreamDynamic stream(true, getITEXSize(_textures[index]));

	ReadContext ctx(*_nsbtx, _textures[index], stream);
	writeITEXHeader(ctx);

	getPalette(ctx);
	getTexture(ctx);

	stream.setDisposable(false);
	return new Common::MemoryReadStream(stream.getData(), stream.size(), true);
}

void NSBTXFile::load(Common::SeekableSubReadStreamEndian &nsbtx) {
	try {

		readHeader(nsbtx);
		readTextures(nsbtx);
		readPalettes(nsbtx);

		createResourceList();

	} catch (Common::Exception &e) {
		e.add("Failed reading NSBTX file");
		throw;
	}
}

void NSBTXFile::readHeader(Common::SeekableSubReadStreamEndian &nsbtx) {
	readFileHeader(nsbtx);
	readInfoHeader(nsbtx);
}

void NSBTXFile::readFileHeader(Common::SeekableSubReadStreamEndian &nsbtx) {
	const uint32_t tag = nsbtx.readUint32BE();
	if (tag != kBTX0ID)
		throw Common::Exception("Invalid NSBTX file (%s)", Common::debugTag(tag).c_str());

	const uint16_t bom = nsbtx.readUint16();
	if (bom != 0xFEFF)
		throw Common::Exception("Invalid BOM: 0x%04X", (uint) bom);

	const uint8_t versionMajor = nsbtx.readByte();
	const uint8_t versionMinor = nsbtx.readByte();
	if ((versionMajor != 1) || (versionMinor != 0))
		throw Common::Exception("Unsupported version %u.%u", versionMajor, versionMinor);

	const uint32_t fileSize = nsbtx.readUint32();
	if (fileSize > (uint32_t)nsbtx.size())
		throw Common::Exception("Size too large (%u > %u)", fileSize, (uint)nsbtx.size());

	const uint16_t headerSize = nsbtx.readUint16();
	if (headerSize != 16)
		throw Common::Exception("Invalid header size (%u)", headerSize);

	const uint16_t sectionCount = nsbtx.readUint16();
	if (sectionCount != 1)
		throw Common::Exception("Invalid number of sections (%u)", sectionCount);

	_textureOffset = nsbtx.readUint32();
}

void NSBTXFile::readInfoHeader(Common::SeekableSubReadStreamEndian &nsbtx) {
	nsbtx.seek(_textureOffset);

	const uint32_t tag = nsbtx.readUint32BE();
	if (tag != kTEX0ID)
		throw Common::Exception("Invalid NSBTX texture (%s)", Common::debugTag(tag).c_str());

	nsbtx.skip(4 + 4 + 2); // Section size + padding + data size

	_textureInfoOffset = _textureOffset + nsbtx.readUint16();

	nsbtx.skip(4); // Padding

	_textureDataOffset = _textureOffset + nsbtx.readUint32();

	nsbtx.skip(4);     // Padding
	nsbtx.skip(2 + 2); // Compressed data size and info offset
	nsbtx.skip(4);     // Padding
	nsbtx.skip(4 + 4); // Compressed data offset and info data offset
	nsbtx.skip(4);     // Padding

	nsbtx.skip(4); // Palette data size

	_paletteInfoOffset = _textureOffset + nsbtx.readUint32();
	_paletteDataOffset = _textureOffset + nsbtx.readUint32();
}

void NSBTXFile::readTextures(Common::SeekableSubReadStreamEndian &nsbtx) {
	nsbtx.seek(_textureInfoOffset);

	nsbtx.skip(1); // Unknown

	const uint8_t textureCount = nsbtx.readByte();

	nsbtx.skip(2); // Section size
	nsbtx.skip(2 + 2 + 4 + textureCount * (2 + 2)); // Unknown

	nsbtx.skip(2 + 2); // Header size + section size

	_textures.resize(textureCount);
	for (Textures::iterator t = _textures.begin(); t != _textures.end(); ++t) {
		t->offset = _textureDataOffset + nsbtx.readUint16() * 8;

		const uint16_t flags = nsbtx.readUint16();

		nsbtx.skip(1); // Unknown

		const uint8_t unknown = nsbtx.readByte();

		nsbtx.skip(2); // Unknown

		t->width  = 8 << ((flags >> 4) & 7);
		t->height = 8 << ((flags >> 7) & 7);

		t->format = (Format) ((flags >> 10) & 7);

		t->wrapX = ( flags        & 1) != 0;
		t->wrapY = ((flags >>  1) & 1) != 0;
		t->flipX = ((flags >>  2) & 1) != 0;
		t->flipY = ((flags >>  3) & 1) != 0;
		t->alpha = ((flags >> 13) & 1) != 0;

		t->coordTransform = (Transform) (flags >> 14);

		if (t->width == 0x00) {
			switch (unknown & 0x3) {
				case 2:
					t->width = 0x200;
					break;
				default:
					t->width = 0x100;
					break;
			}
		}

		if (t->height == 0x00) {
			switch ((unknown >> 4) & 0x3) {
				case 2:
					t->height = 0x200;
					break;
				default:
					t->height = 0x100;
					break;
			}
		}
	}

	for (Textures::iterator t = _textures.begin(); t != _textures.end(); ++t)
		t->name = Common::readStringFixed(nsbtx, Common::kEncodingASCII, 16).toLower();
}

void NSBTXFile::readPalettes(Common::SeekableSubReadStreamEndian &nsbtx) {
	nsbtx.seek(_paletteInfoOffset);

	nsbtx.skip(1); // Unknown

	const uint8_t paletteCount = nsbtx.readByte();

	nsbtx.skip(2); // Section size
	nsbtx.skip(2 + 2 + 4 + paletteCount * (2 + 2)); // Unknown

	nsbtx.skip(2 + 2); // Header size + section size

	_palettes.resize(paletteCount);
	for (Palettes::iterator p = _palettes.begin(); p != _palettes.end(); ++p) {
		const uint16_t offset = nsbtx.readUint16() & 0x1FFF;
		const uint16_t flags  = nsbtx.readUint16();

		const uint8_t paletteStep = ((flags & 1) != 0) ? 16 : 8;

		p->offset = _paletteDataOffset + offset * paletteStep;
	}

	for (Palettes::iterator p = _palettes.begin(); p != _palettes.end(); ++p)
		p->name = Common::readStringFixed(nsbtx, Common::kEncodingASCII, 16).toLower();
}

void NSBTXFile::createResourceList() {
	_resources.resize(_textures.size());

	ResourceList::iterator res = _resources.begin();
	Textures::iterator     tex = _textures.begin();

	uint32_t index = 0;
	for ( ; (res != _resources.end()) && (tex != _textures.end()); ++res, ++tex, ++index) {
		res->name  = tex->name;
		res->type  = kFileTypeXEOSITEX;
		res->index = index;
	}
}

} // End of namespace Aurora
