/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/fps.h
 *  A text object displaying the current FPS.
 */

#ifndef GRAPHICS_AURORA_FPS_H
#define GRAPHICS_AURORA_FPST_H

#include "events/notifyable.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/text.h"

namespace Graphics {

namespace Aurora {

/** An autonomous FPS display. */
class FPS : public Text, public Events::Notifyable {
public:
	FPS(const FontHandle &font);
	FPS(const FontHandle &font, float r, float g, float b, float a);
	~FPS();

	// Renderable
	void render();

private:
	uint32 _fps;

	void init();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_FPS_H
