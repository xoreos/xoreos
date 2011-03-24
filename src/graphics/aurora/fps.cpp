/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/fps.cpp
 *  A text object displaying the current FPS.
 */

#include "common/ustring.h"

#include "graphics/graphics.h"
#include "graphics/font.h"

#include "graphics/aurora/fps.h"

namespace Graphics {

namespace Aurora {

FPS::FPS(const FontHandle &font) : Text(font, "0 fps"), _fps(0) {
	init();
}

FPS::FPS(const FontHandle &font, float r, float g, float b, float a) :
	Text(font, "0 fps", r, g, b, a), _fps(0) {

	init();
}

FPS::~FPS() {
	removeFromQueue();
}

void FPS::init() {
	setTag("FPS");
	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

void FPS::render(RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	uint32 fps = GfxMan.getFPS();

	if (fps != _fps) {
		_fps = fps;

		set(Common::UString::sprintf("%d fps", _fps));
	}

	Text::render(pass);
}

void FPS::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	float posX = -(newWidth  / 2.0);
	float posY =  (newHeight / 2.0) - getHeight();

	setPosition(posX, posY);
}

} // End of namespace Aurora

} // End of namespace Graphics
