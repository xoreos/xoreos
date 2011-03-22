/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/texture.h
 *  A texture as used in the Aurora engines.
 */

#ifndef GRAPHICS_AURORA_TEXTURE_H
#define GRAPHICS_AURORA_TEXTURE_H

#include "common/ustring.h"

#include "graphics/types.h"
#include "graphics/texture.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class ImageDecoder;
class TXI;

namespace Aurora {

/** A texture. */
class Texture : public Graphics::Texture {
public:
	Texture(const Common::UString &name);
	Texture(ImageDecoder *image, const TXI &txi);
	Texture(ImageDecoder *image);
	~Texture();

	const uint32 getWidth()  const;
	const uint32 getHeight() const;

	bool hasAlpha() const;

	/** Return the TXI. */
	const TXI &getTXI() const;

	/** Reload the texture. */
	bool reload(const Common::UString &name = "");

protected:
	void doRebuild();
	void doDestroy();

private:
	Common::UString _name;

	TextureID _textureID; ///< OpenGL texture ID.

	::Aurora::FileType _type; ///< The texture's image's file type.

	ImageDecoder *_image; ///< The actual image.
	TXI *_txi;            ///< The TXI.

	uint32 _width;
	uint32 _height;

	void load(const Common::UString &name);
	void load(ImageDecoder *image);

	TextureID getID() const;

	friend class TextureManager;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXTURE_H
