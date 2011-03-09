/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/texture.h
 *  Virtual baseclass of a texture.
 */

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include "graphics/types.h"
#include "graphics/glcontainer.h"
#include "graphics/queueable.h"

namespace Graphics {

/** A texture. */
class Texture : public GLContainer, public Queueable<Texture> {
public:
	Texture();
	virtual ~Texture();
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXTURE_H
