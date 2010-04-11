/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/cube.h
 *  A simple cube object, for testing.
 */

#ifndef GRAPHICS_CUBE_H
#define GRAPHICS_CUBE_H

#include "graphics/types.h"
#include "graphics/renderable.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class ImageDecoder;

class Cube : public Renderable {
public:
	Cube(Common::SeekableReadStream &tgaTexture);
	~Cube();

	void reloadTextures();

	void render();

private:
	uint32 _lastRotateTime;

	TextureID _texture;

	ImageDecoder *_textureImage;

	void initTexture(Common::SeekableReadStream &tgaTexture);

	void setRotate(float rotate);
	void doCubeSolid();
};

} // End of namespace Graphics

#endif // GRAPHICS_CUBE_H
