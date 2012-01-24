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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/images/txb.h
 *  TXB (another one of BioWare's own texture formats) loading.
 */

#ifndef GRAPHICS_IMAGES_TXB_H
#define GRAPHICS_IMAGES_TXB_H

#include <vector>

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Another one of BioWare's own texture formats, TXB. */
class TXB : public ImageDecoder {
public:
	TXB(Common::SeekableReadStream &txb);
	~TXB();

	/** Return the enclosed TXI data. */
	Common::SeekableReadStream *getTXI() const;

private:
	uint32 _dataSize;

	byte  *_txiData;
	uint32 _txiDataSize;

	// Loading helpers
	void load(Common::SeekableReadStream &txb);
	void readHeader(Common::SeekableReadStream &txb, bool &needDeSwizzle);
	void readData(Common::SeekableReadStream &txb, bool needDeSwizzle);
	void readTXIData(Common::SeekableReadStream &txb);

	static void deSwizzle(byte *dst, const byte *src, uint32 width, uint32 height);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXB_H
