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
 *  Loading Nitro CoLoR palette files.
 */

/* Based heavily on the NCLR reader found in the NDS file viewer
 * and editor Tinke by pleoNeX (<https://github.com/pleonex/tinke>),
 * which is licensed under the terms of the GPLv3.
 *
 * Tinke in turn is based on the NCLR documentation by lowlines
 * (<http://llref.emutalk.net/docs/?file=xml/nclr.xml>).
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

#include <cstring>
#include <memory>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/error.h"

#include "src/graphics/images/nclr.h"

static const uint32_t kNCLRID = MKTAG('N', 'C', 'L', 'R');
static const uint32_t kPLTTID = MKTAG('P', 'L', 'T', 'T');

namespace Graphics {

const byte *NCLR::load(Common::SeekableReadStream &nclr) {
	std::unique_ptr<Common::SeekableSubReadStreamEndian> nclrEndian;
	std::unique_ptr<const byte[]> palette;

	try {
		nclrEndian.reset(open(nclr));
		palette.reset(loadNCLR(*nclrEndian));

	} catch (Common::Exception &e) {
		e.add("Failed reading NCLR file");
		throw;
	}

	return palette.release();
}

const byte *NCLR::loadNCLR(Common::SeekableSubReadStreamEndian &nclr) {
	readHeader(nclr);

	return readPalette(nclr);
}

void NCLR::readHeader(Common::SeekableSubReadStreamEndian &nclr) {
	const uint32_t tag = nclr.readUint32();
	if (tag != kNCLRID)
		throw Common::Exception("Invalid NCLR file (%s)", Common::debugTag(tag).c_str());

	const uint16_t bom = nclr.readUint16();
	if (bom != 0xFEFF)
		throw Common::Exception("Invalid BOM: %u", bom);

	const uint8_t versionMinor = nclr.readByte();
	const uint8_t versionMajor = nclr.readByte();
	if ((versionMajor != 1) || (versionMinor != 0))
		throw Common::Exception("Unsupported version %u.%u", versionMajor, versionMinor);

	const uint32_t fileSize = nclr.readUint32();
	if (fileSize > nclr.size())
		throw Common::Exception("Size too large (%u > %u)", fileSize, (uint)nclr.size());

	const uint16_t headerSize = nclr.readUint16();
	if (headerSize != 16)
		throw Common::Exception("Invalid header size (%u)", headerSize);

	const uint16_t sectionCount = nclr.readUint16();
	if ((sectionCount != 1) && (sectionCount != 2))
		throw Common::Exception("Invalid number of sections (%u)", sectionCount);
}

const byte *NCLR::readPalette(Common::SeekableSubReadStreamEndian &nclr) {
	const uint32_t tag = nclr.readUint32();
	if (tag != kPLTTID)
		throw Common::Exception("Invalid PLTT section (%s)", Common::debugTag(tag).c_str());

	const uint32_t size  = nclr.readUint32();

	const uint16_t depthValue = nclr.readUint16();
	if ((depthValue != 3) && (depthValue != 4))
		throw Common::Exception("Invalid palette depth %u", depthValue);

	const uint8_t depth = (depthValue == 3) ? 4 : 8;

	nclr.skip(6); // Unknown

	// Palette size. If not given or too big, calculate it from the section size
	uint32_t palSize = nclr.readUint32();
	if ((palSize == 0) || (palSize > size))
		palSize = size - 24;

	const uint32_t startOffset = nclr.readUint32() + 24;

	// Clamp the number of colors to the actual palette size
	const uint32_t colorCount = MIN<uint32_t>(1 << depth, palSize / 2) * 3;

	nclr.seek(startOffset);

	std::unique_ptr<byte[]> palette = std::make_unique<byte[]>(colorCount * 3);

	for (uint32_t i = 0; i < colorCount; i += 3) {
		const uint16_t color = nclr.readUint16();

		palette[i + 0] = ((color >> 10) & 0x1F) << 3;
		palette[i + 1] = ((color >>  5) & 0x1F) << 3;
		palette[i + 2] = ( color        & 0x1F) << 3;
	}

	// Make the rest of the palette pink, for high debug visibility
	static const byte kPink[3] = { 0xFF, 0x00, 0xFF };
	for (uint32_t i = colorCount; i < 768; i += sizeof(kPink))
		std::memcpy(palette.get() + i, kPink, sizeof(kPink));

	return palette.release();
}

} // End of namespace Graphics
