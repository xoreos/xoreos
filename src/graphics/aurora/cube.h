/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
	~CubeSide();

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
