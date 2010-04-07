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

#include <SDL.h>

#include "common/util.h"

#include "graphics/graphics.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

GraphicsManager::GraphicsManager() {
	_ready = false;
}

bool GraphicsManager::init() {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
		warning("GraphicsManager::init(): Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	_ready = true;
	return true;
}

void GraphicsManager::deinit() {
	if (!_ready)
		return;

	SDL_Quit();

	_ready = false;
}

bool GraphicsManager::ready() const {
	return _ready;
}

} // End of namespace Graphics
