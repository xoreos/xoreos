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

#include "graphics/aurora/fps.h"
#include "graphics/aurora/font.h"

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
}

void FPS::init() {
	float posX = (-(GfxMan.getScreenWidth()  / 2.0)) / 100.0;
	float posY = ( (GfxMan.getScreenHeight() / 2.0)) / 100.0  - getHeight();

	setPosition(posX, posY);
}

void FPS::render() {
	uint32 fps = GfxMan.getFPS();

	if (fps != _fps) {
		_fps = fps;

		set_internal(Common::UString::sprintf("%d fps", _fps));
		destroy();
		rebuild();
	}

	Text::render();
}

} // End of namespace Aurora

} // End of namespace Graphics
