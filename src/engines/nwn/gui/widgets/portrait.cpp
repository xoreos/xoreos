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

#include "glm/gtc/matrix_transform.hpp"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/shader/materialman.h"
#include "src/graphics/mesh/meshman.h"

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

	_surface = new Graphics::Shader::ShaderSurface(ShaderMan.getShaderObject("default/textureMatrix.vert", Graphics::Shader::SHADER_FRAGMENT), "portrait");
	_material = new Graphics::Shader::ShaderMaterial(ShaderMan.getShaderObject("default/texture.frag", Graphics::Shader::SHADER_FRAGMENT), "portrait");
	// Sampler should be changed when the portrait texture is changed.
	_renderable = new Graphics::Shader::ShaderRenderable(_surface,
	                                                     _material,
	                                                     MeshMan.getMesh("defaultMeshQuad"));

	_borderRenderable = new Graphics::Shader::ShaderRenderable(SurfaceMan.getSurface("defaultSurface"),
	                                                           MaterialMan.getMaterial("defaultBlack"),
	                                                           MeshMan.getMesh("defaultMeshQuad"));
	setSize();
	setPortrait(name);

	setPosition(0.0f, 0.0f, -FLT_MAX);
}

Portrait::~Portrait() {
	delete _renderable;
	delete _surface;
	delete _material;

	delete _borderRenderable;
}

float Portrait::getWidth() const {
	return _width + _border + _border;
}

float Portrait::getHeight() const {
	return _height + _border + _border;
}

void Portrait::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();
	_x = x + _border;
	_y = y + _border;
	_distance = z;
	resort();

	GfxMan.unlockFrame();
}

void Portrait::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _distance;
}

bool Portrait::isIn(float x, float y) const {
	float x1 = _x;
	float x2 = _x + getWidth();
	float y1 = _y;
	float y2 = _y + getHeight();

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

	/**
	 * The "border" in this case is rendered first. The portrait itself is overlaid on
	 * top afterwards - because depth testing is disabled it would seem. This is unfortunate,
	 * because there's a few pixels rendered there for absolutely no reason. Could try to sort
	 * by depth - but then some widgets might actually have to rely on this. So for now, just
	 * deal with the inefficiency.
	 * It's not going to have that great of an impact probably, as borders don't seem to be
	 * used in many places other than the menu system.
	 */
	if (_border > 0.0f) {
		glm::mat4 bform = glm::translate(glm::mat4(), glm::vec3(_x - _border, _y - _border, -10.0f));
		bform *= glm::scale(glm::mat4(), glm::vec3(_width + 2.0f * _border,
		                                           _height + 2.0f * _border,
		                                           1.0f));
		_borderRenderable->renderImmediate(bform);
	}

	glm::mat4 tform = glm::translate(glm::mat4(), glm::vec3(_x, _y, -10.0f));
	tform *= glm::scale(glm::mat4(), glm::vec3(_width, _height, 1.0f));
	_renderable->renderImmediate(tform);
}

void Portrait::renderImmediate(const glm::mat4 &parentTransform) {
	/**
	 * The "border" in this case is rendered first. The portrait itself is overlaid on
	 * top afterwards - because depth testing is disabled it would seem. This is unfortunate,
	 * because there's a few pixels rendered there for absolutely no reason. Could try to sort
	 * by depth - but then some widgets might actually have to rely on this. So for now, just
	 * deal with the inefficiency.
	 * It's not going to have that great of an impact probably, as borders don't seem to be
	 * used in many places other than the menu system.
	 */
	if (_border > 0.0f) {
		glm::mat4 bform = glm::translate(glm::mat4(), glm::vec3(_x - _border, _y - _border, -10.0f));
		bform *= glm::scale(glm::mat4(), glm::vec3(_width + 2.0f * _border,
		                                           _height + 2.0f * _border,
		                                           1.0f));
		_borderRenderable->renderImmediate(parentTransform * bform);
	}

	glm::mat4 tform = glm::translate(glm::mat4(), glm::vec3(_x, _y, -10.0f));
	tform *= glm::scale(glm::mat4(), glm::vec3(_width, _height, 1.0f));
	_renderable->renderImmediate(tform);
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

	Graphics::Shader::ShaderSampler *sampler;
	sampler = (Graphics::Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	sampler->handle = _texture;
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
	_width = kWidth[_size];
	_height = kHeight[_size];
	// Part of the texture(the bottom) is cut for portraits but not for icons.
	float portraitCutRatio;
	if (_size != kSizeIcon) {
		portraitCutRatio = 112.0f / 512.0f;
	} else {
		portraitCutRatio = 0.0f;
	}

	glm::mat4 tmatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, portraitCutRatio, 0.0f));
	tmatrix *= glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f - portraitCutRatio, 1.0f));
	memcpy(_surface->getVariableData("_uv0Matrix"), &tmatrix[0], 16 * sizeof(float));
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
