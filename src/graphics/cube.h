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

class Texture;

class Cube;

class CubeSide : public Renderable {
public:
	CubeSide(Cube &parent, int n);

	void newFrame();

	void reloadTextures();

	void render();

private:
	Cube *_parent;
	int _n;

	friend class Cube;
};

class Cube {
public:
	Cube(const std::string &texture);
	~Cube();

private:
	CubeSide *_sides[6];

	uint32 _lastRotateTime;
	float  _rotation;

	Texture *_texture;

	void setRotate(float rotate);

	void applyTransformation(int n);
	void setTexture();

	void newFrame();
	void reloadTextures();

	friend class CubeSide;
};

} // End of namespace Graphics

#endif // GRAPHICS_CUBE_H
