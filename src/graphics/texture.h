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
#include "graphics/queueable.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class ImageDecoder;
class TXI;

/** A texture. */
class Texture : public Queueable<Texture> {
public:
	Texture(const std::string &name);
	~Texture();

	/** Return the ID of the texture for use with OpenGL. */
	TextureID getID() const;

	const uint32 getWidth()  const;
	const uint32 getHeight() const;

	/** Return the TXI. */
	const TXI &getTXI() const;

private:
	TextureID _textureID; ///< OpenGL texture ID.

	Aurora::FileType _type; ///< The texture's image's file type.

	ImageDecoder *_image; ///< The actual image.
	TXI *_txi;            ///< The TXI.

	uint32 _width;
	uint32 _height;

	void load(const std::string &name);


// To be called from the main/events/graphics thread
public:
	void reload();
	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXTURE_H
