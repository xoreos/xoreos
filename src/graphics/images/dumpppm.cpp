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

void dumpPPM(const std::string &fileName, const byte *data, int width, int height, bool hasAlpha) {
	if ((width <= 0) || (height <= 0) || !data)
		throw Common::Exception("Invalid image data (%dx%d %d)", width, height, data != 0);

	if (hasAlpha)
		throw Common::Exception("TODO: Alpha");

	Common::DumpFile file;

	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	char header[4096];

	std::snprintf(header, 4096, "P6\n%d %d 255\n", width, height);
	header[4095] = '\0';

	file.write(header, std::strlen(header));

	uint32 count = width * height * 3;
	while (count-- > 0)
		file.writeByte(*data++);

	if (file.err())
		throw Common::Exception("Write error");

	file.close();
}

void dumpPPM(const std::string &fileName, const ImageDecoder *image) {
	if (!image)
		throw Common::Exception("image == 0");

	dumpPPM(fileName, image->getData(), image->getWidth(), image->getHeight(), image->hasAlpha());
}

} // End of namespace Graphics
