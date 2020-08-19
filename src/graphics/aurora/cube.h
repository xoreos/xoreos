/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A simple cube object, for testing.
 */

#ifndef GRAPHICS_AURORA_CUBE_H
#define GRAPHICS_AURORA_CUBE_H

#include "external/glm/mat4x4.hpp"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/object.h"

#include "src/graphics/aurora/texturehandle.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

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

	uint32_t _lastRotateTime;
	float  _rotation;

	ListID _list;

	TextureHandle _texture;

	void applyTransformation(int n);
	void applyTransformation(int n, glm::mat4 &m);
	void setTexture();
	void callList();

	void newFrame();
	void reloadTextures();

	friend class CubeSide;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_CUBE_H
