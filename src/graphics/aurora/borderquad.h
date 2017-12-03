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

/**
 * @file
 * A quad for generating borders.
 */

#ifndef GRAPHICS_AURORA_BORDERQUAD_H
#define GRAPHICS_AURORA_BORDERQUAD_H

#include "src/graphics/renderable.h"

#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/texturehandle.h"

namespace Graphics {

namespace Aurora {

class BorderQuad : public Renderable {
public:
	BorderQuad(const Common::UString &edge, const Common::UString &corner, float x, float y, float w, float h,
	           int dimension = 0);

	void setPosition(float x, float y, float z);
	void getPosition(float &x, float &y, float &z);
	void setSize(float w, float h);

	virtual void calculateDistance();

	void render(RenderPass pass);

private:
	TextureHandle _edge, _corner;

	int _edgeWidth, _edgeHeight;
	int _cornerWidth, _cornerHeight;

	float _x;
	float _y;
	float _w;
	float _h;
};

} // End of namespace Aurora

} // End of namespace Engines

#endif // GRAPHICS_AURORA_BORDERQUAD_H
