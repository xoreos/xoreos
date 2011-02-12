/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/sbm.h
 *  Decoding SBM (font bitmap data).
 */

#ifndef GRAPHICS_IMAGES_SBM_H
#define GRAPHICS_IMAGES_SBM_H

#include "graphics/images/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** SBM font bitmap data. */
class SBM : public ImageDecoder {
public:
	SBM(Common::SeekableReadStream *sbm);
	~SBM();

	void load();

	bool isCompressed() const;

	PixelFormat    getFormat() const;
	PixelFormatRaw getFormatRaw() const;
	PixelDataType  getDataType() const;

	int getMipMapCount() const;

	const MipMap &getMipMap(int mipMap) const;

private:
	Common::SeekableReadStream *_sbm;

	MipMap _image;

	MipMap &getMipMap(int mipMap);

	// Loading helpers
	void readData(Common::SeekableReadStream &sbm);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_SBM_H
