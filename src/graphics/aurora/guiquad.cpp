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
 *  A textured quad for a GUI element.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"

#include "src/graphics/images/txi.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

namespace Graphics {

namespace Aurora {

GUIQuad::GUIQuad(const Common::UString &texture,
                 float x1 , float y1 , float x2 , float y2,
                 float tX1, float tY1, float tX2, float tY2) :
	GUIElement(GUIElement::kGUIElementFront),
	_r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f),
	_x1 (x1) , _y1 (y1) , _x2 (x2) , _y2 (y2) ,
	_tX1(tX1), _tY1(tY1), _tX2(tX2), _tY2(tY2),
	_scissorX(0), _scissorY(0), _scissorWidth(0), _scissorHeight(0),
	_xor(false), _scissor(false), _additiveBlending(false) {

	try {

		if (!texture.empty())
			_texture = TextureMan.get(texture);

	} catch (...) {
		_texture.clear();

		_r = _g = _b = _a = 0.0f;
	}

	if (!_texture.empty()) {
		_additiveBlending = (_texture.getTexture().getTXI().getFeatures().blending == TXI::kBlendingAdditive);
	}

	_distance = -FLT_MAX;
}

GUIQuad::GUIQuad(Graphics::GUIElement::GUIElementType type, const Common::UString &texture,
                 float x1 , float y1 , float x2 , float y2,
                 float tX1, float tY1, float tX2, float tY2) :
	GUIElement(type),
	_r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f),
	_x1 (x1) , _y1 (y1) , _x2 (x2) , _y2 (y2) ,
	_tX1(tX1), _tY1(tY1), _tX2(tX2), _tY2(tY2),
	_scissorX(0), _scissorY(0), _scissorWidth(0), _scissorHeight(0),
	_xor(false), _scissor(false), _additiveBlending(false) {

	try {

		if (!texture.empty())
			_texture = TextureMan.get(texture);

	} catch (...) {
		_texture.clear();

		_r = _g = _b = _a = 0.0f;
	}

	if (!_texture.empty()) {
		_additiveBlending = (_texture.getTexture().getTXI().getFeatures().blending == TXI::kBlendingAdditive);
	}

	_distance = -FLT_MAX;
}

GUIQuad::GUIQuad(TextureHandle texture,
                 float x1 , float y1 , float x2 , float y2,
                 float tX1, float tY1, float tX2, float tY2) :
	GUIElement(GUIElement::kGUIElementFront),
	_texture(texture), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f),
	_x1 (x1) , _y1 (y1) , _x2 (x2) , _y2 (y2) ,
	_tX1(tX1), _tY1(tY1), _tX2(tX2), _tY2(tY2),
	_scissorX(0), _scissorY(0), _scissorWidth(0), _scissorHeight(0),
	_xor(false), _scissor(false), _additiveBlending(false) {

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
	lockFrameIfVisible();

	_x2 = _x2 - _x1 + x;
	_y2 = _y2 - _y1 + y;

	_x1 = x;
	_y1 = y;

	_distance = z;
	resort();

	unlockFrameIfVisible();
}


void GUIQuad::getColor(float& r, float& g, float& b, float& a) const {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}


void GUIQuad::setColor(float r, float g, float b, float a) {
	lockFrameIfVisible();

	_r = r;
	_g = g;
	_b = b;
	_a = a;

	unlockFrameIfVisible();
}

void GUIQuad::setTexture(const Common::UString &texture) {
	lockFrameIfVisible();

	try {

		if (texture.empty())
			_texture.clear();
		else
			_texture = TextureMan.get(texture);

	} catch (...) {
		_texture.clear();

		_r = _g = _b = _a = 0.0f;
	}

	unlockFrameIfVisible();
}

void GUIQuad::setTexture(TextureHandle texture) {
	lockFrameIfVisible();

	_texture = texture;

	unlockFrameIfVisible();
}

void GUIQuad::setScissor(int x, int y, int width, int height) {
	_scissorX = x;
	_scissorY = y;
	_scissorWidth = width;
	_scissorHeight = height;
}

float GUIQuad::getWidth() const {
	return ABS(_x2 - _x1);
}

float GUIQuad::getHeight() const {
	return ABS(_y2 - _y1);
}

void GUIQuad::setWidth(float w) {
	lockFrameIfVisible();

	_x2 = _x1 + w;

	unlockFrameIfVisible();
}

void GUIQuad::setHeight(float h) {
	lockFrameIfVisible();

	_y2 = _y1 + h;

	unlockFrameIfVisible();
}

void GUIQuad::setXOR(bool enabled) {
	lockFrameIfVisible();

	_xor = enabled;

	unlockFrameIfVisible();
}

void GUIQuad::setScissor(bool enabled) {
	lockFrameIfVisible();

	_scissor = enabled;

	unlockFrameIfVisible();
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
	bool isTransparent = (_a < 1.0f) || (!_texture.empty() && _texture.getTexture().hasAlpha());
	if (((pass == kRenderPassOpaque)      &&  isTransparent) ||
			((pass == kRenderPassTransparent) && !isTransparent))
		return;

	TextureMan.set(_texture);

	glColor4f(_r, _g, _b, _a);

	if (_additiveBlending) {
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	if (_xor) {
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
	}

	if (_scissor) {
		glEnable(GL_SCISSOR_TEST);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glScissor(viewport[2]/2 + _x1 + _scissorX,
		          viewport[3]/2 + _y1 + _scissorY,
		          _scissorWidth,
		          _scissorHeight);
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

	if (_scissor)
		glDisable(GL_SCISSOR_TEST);

	if (_xor)
		glDisable(GL_COLOR_LOGIC_OP);

	if (_additiveBlending) {
		glPopAttrib();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

} // End of namespace Aurora

} // End of namespace Graphics
