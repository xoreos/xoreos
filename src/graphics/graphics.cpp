/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/graphics.cpp
 *  The global graphics manager.
 */

#include "common/util.h"

#include "graphics/graphics.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

GraphicsManager::GraphicsManager() {
	_ready = false;

	_screen = 0;
}

bool GraphicsManager::init() {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
		warning("GraphicsManager::init(): Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (!createThread()) {
		warning("GraphicsManager::init(): Failed to create graphics thread: %s", SDL_GetError());
		return false;
	}

	_ready = true;
	return true;
}

void GraphicsManager::deinit() {
	if (!_ready)
		return;

	if (!destroyThread())
		warning("GraphicsManager::deinit(): Graphics thread had to be killed");

	SDL_Quit();

	_ready = false;
}

bool GraphicsManager::ready() const {
	return _ready;
}

bool GraphicsManager::initSize(int width, int height, bool fullscreen) {
	int bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if ((bpp != 24) && (bpp != 32)) {
		warning("GraphicsManager::initSize(): Need 24 or 32 bits per pixel");
		return false;
	}

	uint32 flags = SDL_HWSURFACE | SDL_OPENGL | SDL_OPENGLBLIT;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

	if (setupSDLGL(width, height, bpp, flags))
		return true;

	// Could not initialize OpenGL, trying a different bpp value

	bpp = (bpp == 32) ? 24 : 32;

	if (setupSDLGL(width, height, bpp, flags))
		return true;

	// Still couldn't initialize OpenGL, erroring out
	warning("GraphicsManager::initSize(): Failed setting the video mode: %s", SDL_GetError());
	return false;
}

bool GraphicsManager::setupSDLGL(int width, int height, int bpp, uint32 flags) {
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE  , bpp);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

	_screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (!_screen)
		return false;

	return true;
}

void GraphicsManager::threadMethod() {
	while (!_killThread) {
		// Nothing yet
		SDL_Delay(100);
	}
}

} // End of namespace Graphics
