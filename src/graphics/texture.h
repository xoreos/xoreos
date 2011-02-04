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
#include "graphics/queueable.h"

namespace Graphics {

/** A texture. */
class Texture : public Queueable<Texture> {
public:
	Texture();
	virtual ~Texture();

	/** Return the ID of the texture for use with OpenGL. */
	virtual TextureID getID() const = 0;

	virtual const uint32 getWidth()  const = 0;
	virtual const uint32 getHeight() const = 0;


// To be called from the main/events/graphics thread
public:
	virtual void reload()  = 0;
	virtual void destroy() = 0;
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXTURE_H
