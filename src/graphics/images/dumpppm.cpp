/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dumpppm.cpp
 *  A simple PPM image dumper.
 */

#include <cstdio>
#include <cstring>

#include "common/error.h"
#include "common/file.h"

#include "graphics/images/decoder.h"

namespace Graphics {

static void writePixel(Common::DumpFile &file, const byte *data, PixelFormat format) {
	if (format == kPixelFormatRGB) {
		file.writeByte(data[0]);
		file.writeByte(data[1]);
		file.writeByte(data[2]);
	} else if (format == kPixelFormatBGR) {
		file.writeByte(data[2]);
		file.writeByte(data[1]);
		file.writeByte(data[0]);
	} else
		throw Common::Exception("Unsupported pixel format: %d", (int) format);
}

void dumpPPM(const std::string &fileName, const byte *data, int width, int height, PixelFormat format) {
	if ((width <= 0) || (height <= 0) || !data)
		throw Common::Exception("Invalid image data (%dx%d %d)", width, height, data != 0);

	Common::DumpFile file;

	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	char header[4096];

	std::snprintf(header, 4096, "P6\n%d %d 255\n", width, height);
	header[4095] = '\0';

	file.write(header, std::strlen(header));

	uint32 count = width * height;
	for (uint32 i = 0; i < count; i++, data += 3)
		writePixel(file, data, format);

	if (file.err())
		throw Common::Exception("Write error");

	file.close();
}

void dumpPPM(const std::string &fileName, const ImageDecoder *image) {
	if (!image || (image->getMipMapCount() < 1))
		throw Common::Exception("No image");

	const ImageDecoder::MipMap &mipMap = image->getMipMap(0);

	dumpPPM(fileName, mipMap.data, mipMap.width, mipMap.height, image->getFormat());
}

} // End of namespace Graphics
