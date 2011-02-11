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

FPS::FPS(const FontHandle &font) : Text(font, "") {
	float posX = (-(GfxMan.getScreenWidth()  / 2.0))                             / 100.0;
	float posY = ( (GfxMan.getScreenHeight() / 2.0) - font.getFont().getScale()) / 100.0;

	setPosition(posX, posY);
}

FPS::~FPS() {
}

void FPS::render() {
	set_internal(Common::UString::sprintf("%d fps", GfxMan.getFPS()));

	Text::render();
}

} // End of namespace Aurora

} // End of namespace Graphics
