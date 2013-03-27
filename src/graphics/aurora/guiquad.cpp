/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/guiquad.cpp
 *  A textured quad for a GUI element.
 */

#include "common/util.h"
#include "common/ustring.h"

#include "graphics/graphics.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

GUIQuad::GUIQuad(const Common::UString &texture,
                 float x1 , float y1 , float x2 , float y2,
                 float tX1, float tY1, float tX2, float tY2) :
	_r(1.0), _g(1.0), _b(1.0), _a(1.0),
	_x1 (x1) , _y1 (y1) , _x2 (x2) , _y2 (y2) ,
	_tX1(tX1), _tY1(tY1), _tX2(tX2), _tY2(tY2),
	_xor(false) {

	try {

		if (!texture.empty())
			_texture = TextureMan.get(texture);

	} catch (...) {
		_texture.clear();

		_r = _g = _b = _a = 0.0;
	}

	_distance = -FLT_MAX;
}

GUIQuad::~GUIQuad() {
	hide();
}

void GUIQuad::getPosition(float &x, float &y, float &z) const {
	x = MIN(_x1, _x2);
	y = MIN(_y1, _y2);
	z = _distance;
}

void GUIQuad::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x2 = _x2 - _x1 + x;
	_y2 = _y2 - _y1 + y;

	_x1 = x;
	_y1 = y;

	_distance = z;
	resort();

	GfxMan.unlockFrame();
}


void GUIQuad::getColor(float& r, float& g, float& b, float& a) const {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}


void GUIQuad::setColor(float r, float g, float b, float a) {
	GfxMan.lockFrame();

	_r = r;
	_g = g;
	_b = b;
	_a = a;

	GfxMan.unlockFrame();
}

void GUIQuad::setTexture(const Common::UString &texture) {
	GfxMan.lockFrame();

	try {

		if (texture.empty())
			_texture.clear();
		else
			_texture = TextureMan.get(texture);

	} catch (...) {
		_texture.clear();

		_r = _g = _b = _a = 0.0;
	}

	GfxMan.unlockFrame();
}

float GUIQuad::getWidth() const {
	return ABS(_x2 - _x1);
}

float GUIQuad::getHeight() const {
	return ABS(_y2 - _y1);
}

void GUIQuad::setWidth(float w) {
	GfxMan.lockFrame();

	_x2 = _x1 + w;

	GfxMan.unlockFrame();
}

void GUIQuad::setHeight(float h) {
	GfxMan.lockFrame();

	_y2 = _y1 + h;

	GfxMan.unlockFrame();
}

void GUIQuad::setXOR(bool enabled) {
	GfxMan.lockFrame();

	_xor = enabled;

	GfxMan.unlockFrame();
}

bool GUIQuad::isIn(float x, float y) const {
	if ((x < _x1) || (x > _x2))
		return false;
	if ((y < _y1) || (y > _y2))
		return false;

	return true;
}

void GUIQuad::calculateDistance() {
}

void GUIQuad::render(RenderPass pass) {
	bool isTransparent = (_a < 1.0) || (!_texture.empty() && _texture.getTexture().hasAlpha());
	if (((pass == kRenderPassOpaque)      &&  isTransparent) ||
			((pass == kRenderPassTransparent) && !isTransparent))
		return;

	TextureMan.set(_texture);

	glColor4f(_r, _g, _b, _a);

	if (_xor) {
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
	}

	glBegin(GL_QUADS);
		glTexCoord2f(_tX1, _tY1);
		glVertex2f(_x1, _y1);
		glTexCoord2f(_tX2, _tY1);
		glVertex2f(_x2, _y1);
		glTexCoord2f(_tX2, _tY2);
		glVertex2f(_x2, _y2);
		glTexCoord2f(_tX1, _tY2);
		glVertex2f(_x1, _y2);
	glEnd();

	if (_xor)
		glDisable(GL_COLOR_LOGIC_OP);

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

} // End of namespace Aurora

} // End of namespace Graphics
