/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/texture.h
 *  A texture.
 */

#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <string>

#include "graphics/types.h"
#include "graphics/graphics.h"

#include "aurora/types.h"

namespace Graphics {

class ImageDecoder;

class Texture {
public:
	Texture(const std::string &name);
	~Texture();

	TextureID getID() const;

private:
	bool                        _inTextureList;
	GraphicsManager::TextureRef _textureRef;

	TextureID _textureID;

	Aurora::FileType _type;

	ImageDecoder *_plainImage;

	void load(const std::string &name);


// To be called from the main/events/graphics thread
public:
	void removedFromList();

	void reload();
	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXTURE_H
