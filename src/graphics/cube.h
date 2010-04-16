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
#include "graphics/listcontainer.h"
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

	void render();

private:
	Cube *_parent;
	int _n;

	friend class Cube;
};

class Cube : public ListContainer {
public:
	Cube(const std::string &texture);
	~Cube();

// ListContainer
public:
	void rebuild();
	void destroy();

private:
	CubeSide *_sides[6];

	bool _firstTime;

	uint32 _lastRotateTime;
	float  _rotation;

	ListID _list;

	Texture *_texture;

	void setRotate(float rotate);

	void applyTransformation(int n);
	void setTexture();
	void callList();

	void newFrame();
	void reloadTextures();

	friend class CubeSide;
};

} // End of namespace Graphics

#endif // GRAPHICS_CUBE_H
