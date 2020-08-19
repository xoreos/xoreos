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
 *  A simple TGA image dumper.
 */

#include <cstdio>

#include <memory>

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/writefile.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/dumptga.h"

namespace Graphics {

static void writePixel(Common::WriteStream &file, const byte *&data, PixelFormat format) {
	if (format == kPixelFormatRGB) {
		file.writeByte(data[2]);
		file.writeByte(data[1]);
		file.writeByte(data[0]);
		file.writeByte(0xFF);
		data += 3;
	} else if (format == kPixelFormatBGR) {
		file.writeByte(data[0]);
		file.writeByte(data[1]);
		file.writeByte(data[2]);
		file.writeByte(0xFF);
		data += 3;
	} else if (format == kPixelFormatRGBA) {
		file.writeByte(data[2]);
		file.writeByte(data[1]);
		file.writeByte(data[0]);
		file.writeByte(data[3]);
		data += 4;
	} else if (format == kPixelFormatBGRA) {
		file.writeByte(data[0]);
		file.writeByte(data[1]);
		file.writeByte(data[2]);
		file.writeByte(data[3]);
		data += 4;
	} else
		throw Common::Exception("Unsupported pixel format: %d", (int) format);

}

static void writeHeader(Common::WriteStream &stream, int width, int height) {
	stream.writeByte(0);     // ID Length
	stream.writeByte(0);     // Palette size
	stream.writeByte(2);     // Unmapped RGB
	stream.writeUint32LE(0); // Color map
	stream.writeByte(0);     // Color map
	stream.writeUint16LE(0); // X
	stream.writeUint16LE(0); // Y

	stream.writeUint16LE(width);
	stream.writeUint16LE(height);

	stream.writeByte(32); // Pixel depths

	stream.writeByte(0);
}

static void writeMipMap(Common::WriteStream &stream, const ImageDecoder::MipMap &mipMap, PixelFormat format) {
	const byte *data = mipMap.data.get();

	uint32_t count = mipMap.width * mipMap.height;
	while (count-- > 0)
		writePixel(stream, data, format);
}

void dumpTGA(const Common::UString &fileName, const ImageDecoder *image) {
	std::unique_ptr<Common::WriteStream> file = std::make_unique<Common::WriteFile>(fileName);

	dumpTGA(*file, image);
}

void dumpTGA(Common::WriteStream &stream, const ImageDecoder *image) {
	if (!image || (image->getLayerCount() < 1) || (image->getMipMapCount() < 1))
		throw Common::Exception("No image");

	int32_t width  = image->getMipMap(0, 0).width;
	int32_t height = 0;

	for (size_t i = 0; i < image->getLayerCount(); i++) {
		const ImageDecoder::MipMap &mipMap = image->getMipMap(0, i);

		if (mipMap.width != width)
			throw Common::Exception("dumpTGA(): Unsupported image with variable layer width");

		height += mipMap.height;
	}

	writeHeader(stream, width, height);

	for (size_t i = 0; i < image->getLayerCount(); i++)
		writeMipMap(stream, image->getMipMap(0, i), image->getFormat());

	stream.flush();
}

} // End of namespace Graphics
