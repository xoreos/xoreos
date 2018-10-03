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

#include <cfloat>

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/borderquad.h"

namespace Graphics {

namespace Aurora {

BorderQuad::BorderQuad(const Common::UString &edge, const Common::UString &corner,
                       float x, float y, float w, float h, int dimension) :
	Renderable(kRenderableTypeGUIFront), _verticalCut(false),
	_edgeWidth(0), _edgeHeight(0), _cornerWidth(0), _cornerHeight(0),
	_r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f), _x(x), _y(y), _w(w), _h(h) {

	_edge = TextureMan.get(edge);
	_corner = TextureMan.get(corner);

	if (dimension == 0) {
		_cornerWidth = _corner.getTexture().getWidth();
		_cornerHeight = _corner.getTexture().getHeight();
		_edgeWidth = _edge.getTexture().getWidth();
		_edgeHeight = _edge.getTexture().getHeight();
	} else {
		/*
		 * because we do not have a proper factor for modifying the corners, we simply calculate the
		 * factor with the difference of the edge to the dimension
		 */
		float widthFactor = static_cast<float>(dimension) / static_cast<float>(_edge.getTexture().getWidth());
		float heightFactor = static_cast<float>(dimension) / static_cast<float>(_edge.getTexture().getHeight());
		_cornerWidth = widthFactor * _corner.getTexture().getWidth();
		_cornerHeight = heightFactor * _corner.getTexture().getHeight();
		_edgeWidth = dimension;
		_edgeHeight = dimension;
	}

	if (_h < 2 * _edgeHeight) {
		_verticalCut = true;
	}

	_distance = -FLT_MAX;

	assert(!_corner.empty());
	assert(!_edge.empty());
}

void BorderQuad::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;
}

void BorderQuad::setPosition(float x, float y, float z) {
	lockFrameIfVisible();

	// Because of some graphic glitches with non-integer floats, i have to floor the values
	_x = std::floor(x);
	_y = std::floor(y);
	_distance = z;

	unlockFrameIfVisible();
}

void BorderQuad::getPosition(float &x, float &y, float &z) {
	x = _x;
	y = _y;
	z = _distance;
}

void BorderQuad::setSize(float w, float h) {
	_w = std::floor(w);
	_h = std::floor(h);

	if (_h < 2 * _edgeHeight) {
		_verticalCut = true;
	} else {
		_verticalCut = false;
	}
}

void BorderQuad::getSize(float &w, float &h) const {
	w = _w;
	h = _h;
}

void BorderQuad::calculateDistance() {
}

void BorderQuad::render(RenderPass pass) {
	bool isTransparent = (!_corner.empty() && _corner.getTexture().hasAlpha()) ||
	                     (!_edge.empty() && _edge.getTexture().hasAlpha());
	if (((pass == kRenderPassOpaque)      &&  isTransparent) ||
			((pass == kRenderPassTransparent) && !isTransparent))
		return;

	TextureMan.set(_corner);

	glColor4f(_r, _g, _b, _a);

	// Upper left corner
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(_x, _y + _h - _cornerHeight);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _cornerWidth, _y + _h - _cornerHeight);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _cornerWidth, _y + _h);
	glTexCoord2f(0, 1);
	glVertex2f(_x, _y + _h);
	glEnd();

	// Upper right corner
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _w - _cornerWidth, _y + _h - _cornerHeight);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _w, _y + _h - _cornerHeight);
	glTexCoord2f(0, 1);
	glVertex2f(_x + _w, _y + _h);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _w - _cornerWidth, _y + _h);
	glEnd();

	if (_verticalCut) {
		glEnable(GL_SCISSOR_TEST);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glScissor(
				viewport[2]/2 + _x, viewport[3]/2 + _y,
				_w, _h - _edgeHeight
		);
	}

	// Lower left corner
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(_x, _y);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _cornerWidth, _y);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _cornerWidth, _y + _cornerHeight);
	glTexCoord2f(0, 0);
	glVertex2f(_x, _y + _cornerHeight);
	glEnd();

	// Lower right corner
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _w - _cornerWidth, _y);
	glTexCoord2f(0, 1);
	glVertex2f(_x + _w, _y);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _w, _y + _cornerHeight);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _w - _cornerWidth, _y + _cornerHeight);
	glEnd();

	TextureMan.set(_edge);

	// Lower edge
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(_x + _cornerWidth, _y);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _w - _cornerWidth, _y);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _w - _cornerWidth, _y + _edgeHeight);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _cornerWidth, _y + _edgeHeight);
	glEnd();

	if (_verticalCut) {
		glDisable(GL_SCISSOR_TEST);
	}

	// Left edge
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _w - _edgeWidth, _y + _cornerHeight);
	glTexCoord2f(0, 1);
	glVertex2f(_x + _w, _y + _cornerHeight);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _w, _y + _h - _cornerHeight);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _w - _edgeWidth, _y + _h - _cornerHeight);
	glEnd();

	// Right edge
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(_x, _y + _cornerHeight);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _cornerWidth, _y + _cornerHeight);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _cornerWidth, _y + _h - _cornerHeight);
	glTexCoord2f(1, 1);
	glVertex2f(_x, _y + _h - _cornerHeight);
	glEnd();

	// Upper Edge
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex2f(_x + _cornerWidth, _y + _h - _edgeHeight);
	glTexCoord2f(0, 0);
	glVertex2f(_x + _w - _cornerWidth, _y + _h - _edgeHeight);
	glTexCoord2f(0, 1);
	glVertex2f(_x + _w - _cornerWidth, _y + _h);
	glTexCoord2f(1, 1);
	glVertex2f(_x + _cornerWidth, _y + _h);
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

} // End of namespace Aurora

} // End of namespace Graphics
