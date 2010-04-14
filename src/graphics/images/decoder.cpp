/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/decoder.cpp
 *  Generic image decoder interface.
 */

#include "graphics/images/decoder.h"

namespace Graphics {

ImageDecoder::MipMap::MipMap() {
	width  = 0;
	height = 0;
	size   = 0;
	data   = 0;
}

ImageDecoder::MipMap::~MipMap() {
	delete[] data;
}

ImageDecoder::ImageDecoder() {
}

ImageDecoder::~ImageDecoder() {
}

} // End of namespace Graphics
