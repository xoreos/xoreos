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
#include <cstring>

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/writefile.h"

#include "src/graphics/images/decoder.h"

namespace Graphics {

static void writePixel(Common::WriteFile &file, const byte *&data, PixelFormat format) {
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

void dumpTGA(const Common::UString &fileName, const byte *data, int width, int height, PixelFormat format) {
	if ((width <= 0) || (height <= 0) || !data)
		throw Common::Exception("Invalid image data (%dx%d %d)", width, height, data != 0);

	Common::WriteFile file;

	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	file.writeByte(0); // ID Length
	file.writeByte(0); // Palette size
	file.writeByte(2); // Unmapped RGB
	file.writeUint32LE(0); // Color map
	file.writeByte(0);     // Color map
	file.writeUint16LE(0); // X
	file.writeUint16LE(0); // Y

	file.writeUint16LE(width);
	file.writeUint16LE(height);

	file.writeByte(32); // Pixel depths

	file.writeByte(0);

	uint32 count = width * height;
	for (uint32 i = 0; i < count; i++)
		writePixel(file, data, format);

	file.close();
}

void dumpTGA(const Common::UString &fileName, const ImageDecoder *image) {
	if (!image || (image->getMipMapCount() < 1))
		throw Common::Exception("No image");

	const ImageDecoder::MipMap &mipMap = image->getMipMap(0);

	dumpTGA(fileName, mipMap.data, mipMap.width, mipMap.height, image->getFormat());
}

} // End of namespace Graphics
