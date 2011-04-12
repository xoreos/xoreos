/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	void readHeader(Common::SeekableReadStream &txb);
	void readData(Common::SeekableReadStream &txb);
	void readTXIData(Common::SeekableReadStream &txb);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXB_H
