/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/renderable.cpp
 *  An object that can be displayed by the graphics manager.
 */

#include "graphics/renderable.h"

namespace Graphics {

Renderable::Renderable() {
	_inRenderQueue    = false;
	_justAddedToQueue = false;
}

Renderable::~Renderable() {
	removeFromRenderQueue();
}

void Renderable::kickedOutOfRenderQueue() {
	_inRenderQueue    = false;
	_justAddedToQueue = false;
}

void Renderable::addToRenderQueue() {
	if (_inRenderQueue)
		// Already in the render queue
		return;

	_renderQueueRef = GfxMan.addToRenderQueue(*this);

	_inRenderQueue    = true;
	_justAddedToQueue = true;
}

void Renderable::removeFromRenderQueue() {
	if (!_inRenderQueue)
		// Not in the render queue anyway
		return;

	GfxMan.removeFromRenderQueue(_renderQueueRef);

	_inRenderQueue    = false;
	_justAddedToQueue = false;
}

} // End of namespace Graphics
