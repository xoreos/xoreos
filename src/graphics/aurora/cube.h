/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/cube.h
 *  A simple cube object, for testing.
 */

#ifndef GRAPHICS_AURORA_CUBE_H
#define GRAPHICS_AURORA_CUBE_H

#include "graphics/types.h"
#include "graphics/glcontainer.h"
#include "graphics/object.h"

#include "graphics/aurora/textureman.h"

namespace Common {
	class UString;
	class SeekableReadStream;
	class TransformationMatrix;
}

namespace Graphics {

namespace Aurora {

class Texture;

class Cube;

/** A cube side. */
class CubeSide : public Object {
public:
	CubeSide(Cube &parent, int n);

	void calculateDistance();
	void render(RenderPass pass);

private:
	Cube *_parent;
	int _n;

	friend class Cube;
};

/** A simple, rotating cube. */
class Cube : public GLContainer {
public:
	Cube(const Common::UString &texture);
	~Cube();

protected:
	// GLContainer
	void doRebuild();
	void doDestroy();

private:
	CubeSide *_sides[6];

	bool _firstTime;

	uint32 _lastRotateTime;
	float  _rotation;

	ListID _list;

	TextureHandle _texture;

	void applyTransformation(int n);
	void applyTransformation(int n, Common::TransformationMatrix &m);
	void setTexture();
	void callList();

	void newFrame();
	void reloadTextures();

	friend class CubeSide;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_CUBE_H
