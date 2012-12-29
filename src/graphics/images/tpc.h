/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/images/tpc.h
 *  TPC (BioWare's own texture format) loading.
 */

#ifndef GRAPHICS_IMAGES_TPC_H
#define GRAPHICS_IMAGES_TPC_H

#include <vector>

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** BioWare's own texture format, TPC. */
class TPC : public ImageDecoder {
public:
	TPC(Common::SeekableReadStream &tpc);
	~TPC();

	/** Return the enclosed TXI data. */
	Common::SeekableReadStream *getTXI() const;

private:
	byte  *_txiData;
	uint32 _txiDataSize;

	// Loading helpers
	void load(Common::SeekableReadStream &tpc);
	void readHeader(Common::SeekableReadStream &tpc, bool &needDeSwizzle);
	void readData(Common::SeekableReadStream &tpc, bool needDeSwizzle);
	void readTXIData(Common::SeekableReadStream &tpc);

	static void deSwizzle(byte *dst, const byte *src, uint32 width, uint32 height);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TPC_H
