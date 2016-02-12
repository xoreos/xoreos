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
 *  A portrait model and widget.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/ustring.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/engines/nwn/gui/widgets/portrait.h"

static const char * const kSuffix[] = {"h"   , "l"   , "m"   , "s"   , "t"   ,  "i"  };
static const float        kWidth [] = {256.0f, 128.0f,  64.0f,  32.0f,  16.0f,  32.0f};
static const float        kHeight[] = {400.0f, 200.0f, 100.0f,  50.0f,  25.0f,  32.0f};

namespace Engines {

namespace NWN {

Portrait::Portrait(const Common::UString &name, Size size,
		float border, float bR, float bG, float bB, float bA) :
	Graphics::GUIElement(Graphics::GUIElement::kGUIElementFront),
	_size(size), _border(border), _bR(bR), _bG(bG), _bB(bB), _bA(bA) {

	assert((_size >= kSizeHuge) && (_size < kSizeMAX));

	setSize();
	setPortrait(name);

	setPosition(0.0f, 0.0f, -FLT_MAX);
}

Portrait::~Portrait() {
}

void Portrait::createBorder() {
	if (_border <= 0.0f)
		return;

	_qBorder.clear();

	const float x = _qPortrait.vX[0];
	const float y = _qPortrait.vY[0];

	const float width  = ABS(_qPortrait.vX[0] - _qPortrait.vX[1]);
	const float height = ABS(_qPortrait.vY[0] - _qPortrait.vY[2]);

	_qBorder.resize(4);

	_qBorder[0].vX[0] = x         - _border; _qBorder[0].vY[0] = y - _border;
	_qBorder[0].vX[1] = x + width + _border; _qBorder[0].vY[1] = y - _border;
	_qBorder[0].vX[2] = x + width + _border; _qBorder[0].vY[2] = y          ;
	_qBorder[0].vX[3] = x         - _border; _qBorder[0].vY[3] = y          ;

	_qBorder[1].vX[0] = x         - _border; _qBorder[1].vY[0] = y + height;
	_qBorder[1].vX[1] = x + width + _border; _qBorder[1].vY[1] = y + height;
	_qBorder[1].vX[2] = x + width + _border; _qBorder[1].vY[2] = y + height + _border;
	_qBorder[1].vX[3] = x         - _border; _qBorder[1].vY[3] = y + height + _border;

	_qBorder[2].vX[0] = x - _border; _qBorder[2].vY[0] = y          - _border;
	_qBorder[2].vX[1] = x          ; _qBorder[2].vY[1] = y          - _border;
	_qBorder[2].vX[2] = x          ; _qBorder[2].vY[2] = y + height + _border;
	_qBorder[2].vX[3] = x - _border; _qBorder[2].vY[3] = y + height + _border;

	_qBorder[3].vX[0] = x + width          ; _qBorder[3].vY[0] = y          - _border;
	_qBorder[3].vX[1] = x + width + _border; _qBorder[3].vY[1] = y          - _border;
	_qBorder[3].vX[2] = x + width + _border; _qBorder[3].vY[2] = y + height + _border;
	_qBorder[3].vX[3] = x + width          ; _qBorder[3].vY[3] = y + height + _border;
}

float Portrait::getWidth() const {
	return ABS(_qPortrait.vX[0] - _qPortrait.vX[1]) + _border + _border;
}

float Portrait::getHeight() const {
	return ABS(_qPortrait.vY[0] - _qPortrait.vY[2]) + _border + _border;
}

void Portrait::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	float width  = ABS(_qPortrait.vX[0] - _qPortrait.vX[1]);
	float height = ABS(_qPortrait.vY[0] - _qPortrait.vY[2]);

	x += _border;
	y += _border;

	_qPortrait.vX[0] = x        ; _qPortrait.vY[0] = y         ;
	_qPortrait.vX[1] = x + width; _qPortrait.vY[1] = y         ;
	_qPortrait.vX[2] = x + width; _qPortrait.vY[2] = y + height;
	_qPortrait.vX[3] = x        ; _qPortrait.vY[3] = y + height;

	createBorder();

	_distance = z;
	resort();

	GfxMan.unlockFrame();
}

void Portrait::getPosition(float &x, float &y, float &z) const {
	x = _qPortrait.vX[0];
	y = _qPortrait.vY[0];
	z = _distance;
}

bool Portrait::isIn(float x, float y) const {
	float x1 = _qPortrait.vX[0];
	float x2 = _qPortrait.vX[0] + getWidth();
	float y1 = _qPortrait.vY[0];
	float y2 = _qPortrait.vY[0] + getHeight();

	if ((x < x1) || (x > x2))
		return false;
	if ((y < y1) || (y > y2))
		return false;

	return true;
}

void Portrait::calculateDistance() {
}

void Portrait::render(Graphics::RenderPass pass) {
	bool isTransparent = (_bA < 1.0f) ||
	                     (!_texture.empty() && _texture.getTexture().hasAlpha());
	if (((pass == Graphics::kRenderPassOpaque)      &&  isTransparent) ||
			((pass == Graphics::kRenderPassTransparent) && !isTransparent))
		return;


	// Border

	TextureMan.set();
	glColor4f(_bR, _bG, _bB, _bA);

	glBegin(GL_QUADS);
	for (std::vector<Quad>::const_iterator b = _qBorder.begin(); b != _qBorder.end(); ++b)
		for (int i = 0; i < 4; i++)
			glVertex2f(b->vX[i], b->vY[i]);
	glEnd();


	// Portrait

	TextureMan.set(_texture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(_qPortrait.tX[i], _qPortrait.tY[i]);
		glVertex2f  (_qPortrait.vX[i], _qPortrait.vY[i]);
	}
	glEnd();
}

void Portrait::setPortrait(const Common::UString &name) {
	Size curSize = _size;

	_texture.clear();
	while (_texture.empty() && (curSize < kSizeMAX)) {
		try {
			_texture = TextureMan.get(name + kSuffix[curSize]);
		} catch (...) {
			_texture.clear();
		}

		curSize = (Size) (curSize + 1);
	}

	if (_texture.empty()) {
		try {
			_texture = TextureMan.get(name);
		} catch (...) {
			_texture.clear();
		}
	}
}

void Portrait::setBorderColor(float bR, float bG, float bB, float bA) {
	GfxMan.lockFrame();
	_bR = bR;
	_bG = bG;
	_bB = bB;
	_bA = bA;
	GfxMan.unlockFrame();
}

void Portrait::setSize() {
	_qPortrait.vX[0] = 0.0f                ; _qPortrait.vY[0] = 0.0f                 ;
	_qPortrait.vX[1] = 0.0f + kWidth[_size]; _qPortrait.vY[1] = 0.0f                 ;
	_qPortrait.vX[2] = 0.0f + kWidth[_size]; _qPortrait.vY[2] = 0.0f + kHeight[_size];
	_qPortrait.vX[3] = 0.0f                ; _qPortrait.vY[3] = 0.0f + kHeight[_size];

	// Part of the texture(the bottom) is cut for portraits but not for icons.
	float portraitCutRatio;
	if (_size != kSizeIcon) {
		portraitCutRatio = 112.0f / 512.0f;
	} else {
		portraitCutRatio = 0.0f;
	}

	_qPortrait.tX[0] = 0.0f; _qPortrait.tY[0] = portraitCutRatio;
	_qPortrait.tX[1] = 1.0f; _qPortrait.tY[1] = portraitCutRatio;
	_qPortrait.tX[2] = 1.0f; _qPortrait.tY[2] = 1.0f;
	_qPortrait.tX[3] = 0.0f; _qPortrait.tY[3] = 1.0f;
}


PortraitWidget::PortraitWidget(::Engines::GUI &gui, const Common::UString &tag,
               const Common::UString &name, Portrait::Size size, float border,
               float bR, float bG, float bB, float bA) : NWNWidget(gui, tag),
	_portrait(name, size, border, bR, bG, bB, bA) {

	_portrait.setTag(tag);
	_portrait.setClickable(true);
}

PortraitWidget::~PortraitWidget() {
}

void PortraitWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_portrait.show();

	NWNWidget::show();
}

void PortraitWidget::hide() {
	if (!isVisible())
		return;

	_portrait.hide();
	NWNWidget::hide();
}

void PortraitWidget::mouseDown(uint8 state, float x, float y) {
	if (_owner)
		_owner->mouseDown(state, x, y);
}

void PortraitWidget::mouseWheel(uint8 state, int x, int y) {
	if (_owner)
		_owner->mouseWheel(state, x, y);
}

void PortraitWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_portrait.setPosition(x, y, z);
}

void PortraitWidget::setPortrait(const Common::UString &name) {
	_portrait.setPortrait(name);
}

void PortraitWidget::setBorderColor(float bR, float bG, float bB, float bA) {
	_portrait.setBorderColor(bR, bG, bB, bA);
}

float PortraitWidget::getWidth() const {
	return _portrait.getWidth();
}

float PortraitWidget::getHeight() const {
	return _portrait.getHeight();
}

void PortraitWidget::setTag(const Common::UString &tag) {
	NWNWidget::setTag(tag);

	_portrait.setTag(getTag());
}

} // End of namespace NWN

} // End of namespace Engines
