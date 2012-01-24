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
	hide();
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
