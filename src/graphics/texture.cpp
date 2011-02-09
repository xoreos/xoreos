/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/texture.cpp
 *  Virtual baseclass of a texture.
 */

#include "common/threads.h"

#include "graphics/texture.h"
#include "graphics/graphics.h"

namespace Graphics {

Texture::Texture() : Queueable<Texture>(GfxMan.getTextureQueue()) {
}

Texture::~Texture() {
}

void Texture::enforceMainThread() {
	Common::enforceMainThread();
}

} // End of namespace Graphics
