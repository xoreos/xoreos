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
 *  The GUI scrollbar model.
 */

#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/graphics/graphics.h"

#include "src/graphics/mesh/meshman.h"

#include "src/graphics/aurora/textureman.h"

#include "src/engines/nwn/gui/widgets/scrollbar.h"

namespace Engines {

namespace NWN {

Scrollbar::Scrollbar(Type type) : Graphics::GUIElement(Graphics::GUIElement::kGUIElementFront),
	_type(type), _x(0.0f), _y(0.0f), _z(0.0f) {
	_texture = TextureMan.get("gui_scrollbar");

	if (GfxMan.isRendererExperimental()) {
		_surface = std::make_unique<Graphics::Shader::ShaderSurface>(ShaderMan.getShaderObject("default/textureMatrix.vert", Graphics::Shader::SHADER_VERTEX), "portrait");
		_surface->setVariableExternal("_uv0Matrix", &_textureMatrix);

		_material = std::make_unique<Graphics::Shader::ShaderMaterial>(ShaderMan.getShaderObject("default/texture.frag", Graphics::Shader::SHADER_FRAGMENT), "portrait");
		Graphics::Shader::ShaderSampler *sampler;
		sampler = (Graphics::Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
		sampler->handle = _texture;

		_renderable = std::make_unique<Graphics::Shader::ShaderRenderable>(_surface.get(), _material.get(), MeshMan.getMesh("defaultMeshQuad"));
	}
	setLength(16.0f);
}

Scrollbar::~Scrollbar() {
	hide();
}

void Scrollbar::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x = x;
	_y = y;
	_z = z;

	calculateDistance();

	GfxMan.unlockFrame();
}

void Scrollbar::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
}

bool Scrollbar::isIn(float x, float y) const {
	if ((x < _x) || (y < _y))
		return false;
	if ((x > (_x + getWidth())) || (y > (_y + getHeight())))
		return false;

	return true;
}

void Scrollbar::setLength(float length) {
	GfxMan.lockFrame();

	// Need at least the space for the 2 caps
	_length = MAX(length, 4.0f);

	if      (_type == kTypeVertical)
		createV();
	else if (_type == kTypeHorizontal)
		createH();

	GfxMan.unlockFrame();
}

float Scrollbar::getWidth() const {
	if      (_type == kTypeVertical)
		return 10.0f;
	else if (_type == kTypeHorizontal)
		return _length;

	return 0.0f;
}

float Scrollbar::getHeight() const {
	if      (_type == kTypeVertical)
		return _length;
	else if (_type == kTypeHorizontal)
		return 10.0f;

	return 0.0f;
}

void Scrollbar::calculateDistance() {
	_distance = _z;
}

void Scrollbar::render(Graphics::RenderPass pass) {
	// The scrollbar model is opaque
	if (pass == Graphics::kRenderPassTransparent)
		return;

	TextureMan.set(_texture);

	glTranslatef(roundf(_x), roundf(_y), _z);

	glBegin(GL_QUADS);
	for (std::vector<Quad>::const_iterator q = _quads.begin(); q != _quads.end(); ++q) {
		for (int i = 0; i < 4; i++) {
			glTexCoord2f(q->tX[i], q->tY[i]);
			glVertex2f  (q->vX[i], q->vY[i]);
		}
	}
	glEnd();
}

void Scrollbar::renderImmediate(const glm::mat4 &parentTransform) {
	glm::mat4 tform;
	tform = glm::translate(glm::mat4(), glm::vec3(roundf(_x), roundf(_y), _z));

	// CapA (top or left, depending on scrollbar orientation).
	_textureMatrix = _textureMatrixCapA;
	_renderable->renderImmediate(parentTransform * tform * _scrollMatrixCapA);

	// Render the scrollbar itself.
	_textureMatrix = _textureMatrixBar;
	_renderable->renderImmediate(parentTransform * tform * _scrollMatrixBar);

	// CapB (bottom or right, depending on scrollbar orientation).
	_textureMatrix = _textureMatrixCapB;
	_renderable->renderImmediate(parentTransform * tform * _scrollMatrixCapB);
}

void Scrollbar::createH() {
	// Number of 16 pixel wide quads
	int n = ceilf(_length / 16.0f);

	_textureMatrixBar = glm::mat4();
	_textureMatrixBar = glm::scale(_textureMatrixBar, glm::vec3(10.0f/16.0f, (_length - 4.0f) / 16.0f, 1.0f));
	_textureMatrixBar = glm::translate(_textureMatrixBar, glm::vec3((3.0f/16.0f), 0.0f, 0.0f));

	_textureMatrixCapA = glm::mat4();
	_textureMatrixCapA = glm::translate(_textureMatrixCapA, glm::vec3(3.0f/16.0f, 14.0f/16.0f, 0.0f));
	_textureMatrixCapA = glm::scale(_textureMatrixCapA, glm::vec3(10.0f/16.0f, 2.0f/16.0f, 1.0f));

	_textureMatrixCapB = glm::mat4();
	_textureMatrixCapB = glm::translate(_textureMatrixCapB, glm::vec3(3.0f/16.0f, 0.0f, 0.0f));
	_textureMatrixCapB = glm::scale(_textureMatrixCapB, glm::vec3(10.0f/16.0f, 2.0f/16.0f, 1.0f));

	_scrollMatrixBar = glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 0.0f));
	_scrollMatrixBar = glm::scale(_scrollMatrixBar, glm::vec3(_length - 4.0f, 10.0f, 1.0f));

	_scrollMatrixCapA = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
	_scrollMatrixCapA = glm::scale(_scrollMatrixCapA, glm::vec3(2.0f, 10.0f, 1.0f));

	_scrollMatrixCapB = glm::translate(glm::mat4(), glm::vec3(_length - 2.0f, 0.0f, 0.0f));
	_scrollMatrixCapB = glm::scale(_scrollMatrixCapB, glm::vec3(2.0f, 10.0f, 1.0f));

	float x = 0.0f;
	float y = 0.0f;

	// Quads + caps
	_quads.resize(n + 2);

	// Left cap
	_quads[0].tX[0] =  3.0f / 16.0f; _quads[0].tY[0] = 0.0f;
	_quads[0].tX[1] =  3.0f / 16.0f; _quads[0].tY[1] = 2.0f / 16.0f;
	_quads[0].tX[2] = 13.0f / 16.0f; _quads[0].tY[2] = 2.0f / 16.0f;
	_quads[0].tX[3] = 13.0f / 16.0f; _quads[0].tY[3] = 0.0f;

	_quads[0].vX[0] = x       ; _quads[0].vY[0] = y;
	_quads[0].vX[1] = x + 2.0f; _quads[0].vY[1] = y;
	_quads[0].vX[2] = x + 2.0f; _quads[0].vY[2] = y + 10.0f;
	_quads[0].vX[3] = x       ; _quads[0].vY[3] = y + 10.0f;

	x += 2.0f;

	float length = _length - 4.0f;

	// Bar
	for (int i = 0; i < n; i++) {
		float pLength = MIN(length, 16.0f);
		float tLength = pLength / 16.0f;

		_quads[i + 1].tX[0] =    0.0f; _quads[i + 1].tY[0] =  3.0f / 16.0f;
		_quads[i + 1].tX[1] = tLength; _quads[i + 1].tY[1] =  3.0f / 16.0f;
		_quads[i + 1].tX[2] = tLength; _quads[i + 1].tY[2] = 13.0f / 16.0f;
		_quads[i + 1].tX[3] =    0.0f; _quads[i + 1].tY[3] = 13.0f / 16.0f;

		_quads[i + 1].vX[0] = x          ; _quads[i + 1].vY[0] = y;
		_quads[i + 1].vX[1] = x + pLength; _quads[i + 1].vY[1] = y;
		_quads[i + 1].vX[2] = x + pLength; _quads[i + 1].vY[2] = y + 10.0f;
		_quads[i + 1].vX[3] = x          ; _quads[i + 1].vY[3] = y + 10.0f;

		x      += pLength;
		length -= pLength;
	}

	// Right cap
	_quads[n + 1].tX[0] =  3.0f / 16.0f; _quads[n + 1].tY[0] = 14.0f / 16.0f;
	_quads[n + 1].tX[1] =  3.0f / 16.0f; _quads[n + 1].tY[1] = 16.0f / 16.0f;
	_quads[n + 1].tX[2] = 13.0f / 16.0f; _quads[n + 1].tY[2] = 16.0f / 16.0f;
	_quads[n + 1].tX[3] = 13.0f / 16.0f; _quads[n + 1].tY[3] = 14.0f / 16.0f;

	_quads[n + 1].vX[0] = x       ; _quads[n + 1].vY[0] = y;
	_quads[n + 1].vX[1] = x + 2.0f; _quads[n + 1].vY[1] = y;
	_quads[n + 1].vX[2] = x + 2.0f; _quads[n + 1].vY[2] = y + 10.0f;
	_quads[n + 1].vX[3] = x       ; _quads[n + 1].vY[3] = y + 10.0f;
}

void Scrollbar::createV() {
	// Number of 16 pixel wide quads
	int n = ceilf(_length / 16.0f);

	_textureMatrixBar = glm::rotate(glm::mat4(), Common::deg2rad(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_textureMatrixBar = glm::scale(_textureMatrixBar, glm::vec3(10.0f/16.0f, (_length - 4.0f) / 16.0f, 1.0f));
	_textureMatrixBar = glm::translate(_textureMatrixBar, glm::vec3(3.0f/16.0f, 0.0f, 0.0f));

	_textureMatrixCapA = glm::mat4();
	_textureMatrixCapA = glm::translate(_textureMatrixCapA, glm::vec3(3.0f/16.0f, 14.0f/16.0f, 0.0f));
	_textureMatrixCapA = glm::scale(_textureMatrixCapA, glm::vec3(10.0f/16.0f, 2.0f/16.0f, 1.0f));

	_textureMatrixCapB = glm::mat4();
	_textureMatrixCapB = glm::translate(_textureMatrixCapB, glm::vec3(3.0f/16.0f, 0.0f, 0.0f));
	_textureMatrixCapB = glm::scale(_textureMatrixCapB, glm::vec3(10.0f/16.0f, 2.0f/16.0f, 1.0f));

	_scrollMatrixBar = glm::translate(glm::mat4(), glm::vec3(0.0f, 2.0f, 0.0f));
	_scrollMatrixBar = glm::scale(_scrollMatrixBar, glm::vec3(10.0f, _length - 4.0f, 1.0f));

	_scrollMatrixCapA = glm::translate(glm::mat4(), glm::vec3(0.0f, _length - 2.0f, 0.0f));
	_scrollMatrixCapA = glm::scale(_scrollMatrixCapA, glm::vec3(10.0f, 2.0f, 1.0f));

	_scrollMatrixCapB = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
	_scrollMatrixCapB = glm::scale(_scrollMatrixCapB, glm::vec3(10.0f, 2.0f, 1.0f));

	float x = 0.0f;
	float y = 0.0f;

	// Quads + caps
	_quads.resize(n + 2);

	// Bottom cap
	_quads[0].tX[0] =  3.0f / 16.0f; _quads[0].tY[0] = 0.0f;
	_quads[0].tX[1] =  3.0f / 16.0f; _quads[0].tY[1] = 2.0f / 16.0f;
	_quads[0].tX[2] = 13.0f / 16.0f; _quads[0].tY[2] = 2.0f / 16.0f;
	_quads[0].tX[3] = 13.0f / 16.0f; _quads[0].tY[3] = 0.0f;

	_quads[0].vX[0] = x        ; _quads[0].vY[0] = y;
	_quads[0].vX[1] = x + 10.0f; _quads[0].vY[1] = y;
	_quads[0].vX[2] = x + 10.0f; _quads[0].vY[2] = y + 2.0f;
	_quads[0].vX[3] = x        ; _quads[0].vY[3] = y + 2.0f;

	y += 2.0f;

	float length = _length - 4.0f;

	// Bar
	for (int i = 0; i < n; i++) {
		float pLength = MIN(length, 16.0f);
		float tLength = pLength / 16.0f;

		_quads[i + 1].tX[0] =    0.0f; _quads[i + 1].tY[0] =  3.0f / 16.0f;
		_quads[i + 1].tX[1] =    0.0f; _quads[i + 1].tY[1] = 13.0f / 16.0f;
		_quads[i + 1].tX[2] = tLength; _quads[i + 1].tY[2] = 13.0f / 16.0f;
		_quads[i + 1].tX[3] = tLength; _quads[i + 1].tY[3] =  3.0f / 16.0f;

		_quads[i + 1].vX[0] = x        ; _quads[i + 1].vY[0] = y;
		_quads[i + 1].vX[1] = x + 10.0f; _quads[i + 1].vY[1] = y;
		_quads[i + 1].vX[2] = x + 10.0f; _quads[i + 1].vY[2] = y + pLength;
		_quads[i + 1].vX[3] = x        ; _quads[i + 1].vY[3] = y + pLength;

		y      += pLength;
		length -= pLength;
	}

	// Top cap
	_quads[n + 1].tX[0] =  3.0f / 16.0f; _quads[n + 1].tY[0] = 14.0f / 16.0f;
	_quads[n + 1].tX[1] =  3.0f / 16.0f; _quads[n + 1].tY[1] = 16.0f / 16.0f;
	_quads[n + 1].tX[2] = 13.0f / 16.0f; _quads[n + 1].tY[2] = 16.0f / 16.0f;
	_quads[n + 1].tX[3] = 13.0f / 16.0f; _quads[n + 1].tY[3] = 14.0f / 16.0f;

	_quads[n + 1].vX[0] = x        ; _quads[n + 1].vY[0] = y;
	_quads[n + 1].vX[1] = x + 10.0f; _quads[n + 1].vY[1] = y;
	_quads[n + 1].vX[2] = x + 10.0f; _quads[n + 1].vY[2] = y + 2.0f;
	_quads[n + 1].vX[3] = x        ; _quads[n + 1].vY[3] = y + 2.0f;
}


WidgetScrollbar::WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
                                 Scrollbar::Type type, float range) :
		NWNWidget(gui, tag), _type(type), _range(range), _state(0.0f), _scrollbar(type) {

	_scrollbar.setTag(tag);
	_scrollbar.setClickable(true);

	setLength(1.0f);
}

WidgetScrollbar::~WidgetScrollbar() {
}

void WidgetScrollbar::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_scrollbar.show();

	NWNWidget::show();
}

void WidgetScrollbar::hide() {
	if (!isVisible())
		return;

	_scrollbar.hide();
	NWNWidget::hide();
}

void WidgetScrollbar::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	setState(_state);
}

void WidgetScrollbar::setLength(float percent) {
	_full = percent >= 1.0f;

	// Calculate the actual length, at 2 pixel intervals
	_length = ceilf(MAX(_range * CLIP(percent, 0.0f, 1.0f), 10.0f));
	if ((((int) _length) % 2) == 1)
		_length += 1.0f;

	if (_length > _range)
		_length = _range;

	_scrollbar.setLength(_length);

	setState(_state);
}

float WidgetScrollbar::getState() const {
	return _state;
}

void WidgetScrollbar::setState(float state) {
	_state = CLIP(state, 0.0f, 1.0f);

	float span = _range - _length; // Space to scroll in
	float pos  = _state * span;    // Offset within that space

	float x, y, z;
	getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		y += span - pos;
	else if (_type == Scrollbar::kTypeHorizontal)
		x += pos;

	_scrollbar.setPosition(x, y, z);
}

float WidgetScrollbar::getWidth() const {
	return _scrollbar.getWidth();
}

float WidgetScrollbar::getHeight() const {
	return _scrollbar.getHeight();
}

float WidgetScrollbar::getBarPosition() const {
	float x, y, z;
	_scrollbar.getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		return y;
	else if (_type == Scrollbar::kTypeHorizontal)
		return x;

	return 0.0f;
}

void WidgetScrollbar::mouseDown(uint8_t state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	// We only care about the left mouse button, pass everything else to the owner
	if (state != SDL_BUTTON_LMASK) {
		if (_owner)
			_owner->mouseDown(state, x, y);
		return;
	}

	_dragX     = x;
	_dragY     = y;
	_dragState = _state;
}

void WidgetScrollbar::mouseMove(uint8_t state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	float steps = 1.0f / (_range - _length);

	if      (_type == Scrollbar::kTypeVertical)
		setState(_dragState + ((_dragY - y) * steps));
	else if (_type == Scrollbar::kTypeHorizontal)
		setState(_dragState + ((x - _dragX) * steps));

	setActive(true);
}

void WidgetScrollbar::mouseWheel(uint8_t state, int x, int y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (_owner)
		_owner->mouseWheel(state, x, y);
}

} // End of namespace NWN

} // End of namespace Engines
