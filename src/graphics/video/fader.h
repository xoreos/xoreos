/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/fader.h
 *  Testing implementation for the VideoDecoder interface.
 */

#ifndef GRAPHICS_VIDEO_FADER_H
#define GRAPHICS_VIDEO_FADER_H

#include "graphics/video/decoder.h"

namespace Graphics {

/** A generic interface for video decoders. */
class Fader : public VideoDecoder {
public:
	Fader(uint32 width, uint32 height, int n);
	~Fader();

	void update();

private:
	byte _c;
	int _n;

	bool _firstFrame;
	uint32 _lastUpdate;
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_FADER_H
