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

/** @file engines/nwn/gui/widgets/scrollbar.cpp
 *  The GUI scrollbar model.
 */

#include "common/util.h"
#include "common/maths.h"

#include "graphics/graphics.h"

#include "engines/nwn/gui/widgets/scrollbar.h"

namespace Engines {

namespace NWN {

Scrollbar::Scrollbar(Type type) : _type(type), _x(0.0), _y(0.0), _z(0.0) {
	_texture = TextureMan.get("gui_scrollbar");

	setLength(16.0);
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
		return 10.0;
	else if (_type == kTypeHorizontal)
		return _length;

	return 0.0;
}

float Scrollbar::getHeight() const {
	if      (_type == kTypeVertical)
		return _length;
	else if (_type == kTypeHorizontal)
		return 10.0;

	return 0.0;
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

void Scrollbar::createH() {
	// Number of 16 pixel wide quads
	int n = ceilf(_length / 16.0);

	float x = 0.0;
	float y = 0.0;

	// Quads + caps
	_quads.resize(n + 2);

	// Left cap
	_quads[0].tX[0] =  3.0 / 16.0; _quads[0].tY[0] = 0.0;
	_quads[0].tX[1] =  3.0 / 16.0; _quads[0].tY[1] = 2.0 / 16.0;
	_quads[0].tX[2] = 13.0 / 16.0; _quads[0].tY[2] = 2.0 / 16.0;
	_quads[0].tX[3] = 13.0 / 16.0; _quads[0].tY[3] = 0.0;

	_quads[0].vX[0] = x      ; _quads[0].vY[0] = y;
	_quads[0].vX[1] = x + 2.0; _quads[0].vY[1] = y;
	_quads[0].vX[2] = x + 2.0; _quads[0].vY[2] = y + 10.0;
	_quads[0].vX[3] = x      ; _quads[0].vY[3] = y + 10.0;

	x += 2.0;

	float length = _length - 4.0;

	// Bar
	for (int i = 0; i < n; i++) {
		float pLength = MIN(length, 16.0f);
		float tLength = pLength / 16.0;

		_quads[i + 1].tX[0] =     0.0; _quads[i + 1].tY[0] =  3.0 / 16.0;
		_quads[i + 1].tX[1] = tLength; _quads[i + 1].tY[1] =  3.0 / 16.0;
		_quads[i + 1].tX[2] = tLength; _quads[i + 1].tY[2] = 13.0 / 16.0;
		_quads[i + 1].tX[3] =     0.0; _quads[i + 1].tY[3] = 13.0 / 16.0;

		_quads[i + 1].vX[0] = x          ; _quads[i + 1].vY[0] = y;
		_quads[i + 1].vX[1] = x + pLength; _quads[i + 1].vY[1] = y;
		_quads[i + 1].vX[2] = x + pLength; _quads[i + 1].vY[2] = y + 10.0;
		_quads[i + 1].vX[3] = x          ; _quads[i + 1].vY[3] = y + 10.0;

		x      += pLength;
		length -= pLength;
	}

	// Right cap
	_quads[n + 1].tX[0] =  3.0 / 16.0; _quads[n + 1].tY[0] = 14.0 / 16.0;
	_quads[n + 1].tX[1] =  3.0 / 16.0; _quads[n + 1].tY[1] = 16.0 / 16.0;
	_quads[n + 1].tX[2] = 13.0 / 16.0; _quads[n + 1].tY[2] = 16.0 / 16.0;
	_quads[n + 1].tX[3] = 13.0 / 16.0; _quads[n + 1].tY[3] = 14.0 / 16.0;

	_quads[n + 1].vX[0] = x      ; _quads[n + 1].vY[0] = y;
	_quads[n + 1].vX[1] = x + 2.0; _quads[n + 1].vY[1] = y;
	_quads[n + 1].vX[2] = x + 2.0; _quads[n + 1].vY[2] = y + 10.0;
	_quads[n + 1].vX[3] = x      ; _quads[n + 1].vY[3] = y + 10.0;
}

void Scrollbar::createV() {
	// Number of 16 pixel wide quads
	int n = ceilf(_length / 16.0);

	float x = 0.0;
	float y = 0.0;

	// Quads + caps
	_quads.resize(n + 2);

	// Bottom cap
	_quads[0].tX[0] =  3.0 / 16.0; _quads[0].tY[0] = 0.0;
	_quads[0].tX[1] =  3.0 / 16.0; _quads[0].tY[1] = 2.0 / 16.0;
	_quads[0].tX[2] = 13.0 / 16.0; _quads[0].tY[2] = 2.0 / 16.0;
	_quads[0].tX[3] = 13.0 / 16.0; _quads[0].tY[3] = 0.0;

	_quads[0].vX[0] = x       ; _quads[0].vY[0] = y;
	_quads[0].vX[1] = x + 10.0; _quads[0].vY[1] = y;
	_quads[0].vX[2] = x + 10.0; _quads[0].vY[2] = y + 2.0;
	_quads[0].vX[3] = x       ; _quads[0].vY[3] = y + 2.0;

	y += 2.0;

	float length = _length - 4.0;

	// Bar
	for (int i = 0; i < n; i++) {
		float pLength = MIN(length, 16.0f);
		float tLength = pLength / 16.0;

		_quads[i + 1].tX[0] =     0.0; _quads[i + 1].tY[0] =  3.0 / 16.0;
		_quads[i + 1].tX[1] =     0.0; _quads[i + 1].tY[1] = 13.0 / 16.0;
		_quads[i + 1].tX[2] = tLength; _quads[i + 1].tY[2] = 13.0 / 16.0;
		_quads[i + 1].tX[3] = tLength; _quads[i + 1].tY[3] =  3.0 / 16.0;

		_quads[i + 1].vX[0] = x       ; _quads[i + 1].vY[0] = y;
		_quads[i + 1].vX[1] = x + 10.0; _quads[i + 1].vY[1] = y;
		_quads[i + 1].vX[2] = x + 10.0; _quads[i + 1].vY[2] = y + pLength;
		_quads[i + 1].vX[3] = x       ; _quads[i + 1].vY[3] = y + pLength;

		y      += pLength;
		length -= pLength;
	}

	// Top cap
	_quads[n + 1].tX[0] =  3.0 / 16.0; _quads[n + 1].tY[0] = 14.0 / 16.0;
	_quads[n + 1].tX[1] =  3.0 / 16.0; _quads[n + 1].tY[1] = 16.0 / 16.0;
	_quads[n + 1].tX[2] = 13.0 / 16.0; _quads[n + 1].tY[2] = 16.0 / 16.0;
	_quads[n + 1].tX[3] = 13.0 / 16.0; _quads[n + 1].tY[3] = 14.0 / 16.0;

	_quads[n + 1].vX[0] = x       ; _quads[n + 1].vY[0] = y;
	_quads[n + 1].vX[1] = x + 10.0; _quads[n + 1].vY[1] = y;
	_quads[n + 1].vX[2] = x + 10.0; _quads[n + 1].vY[2] = y + 2.0;
	_quads[n + 1].vX[3] = x       ; _quads[n + 1].vY[3] = y + 2.0;
}


WidgetScrollbar::WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
                                 Scrollbar::Type type, float range) :
		NWNWidget(gui, tag), _type(type), _range(range), _state(0.0), _scrollbar(type) {

	_scrollbar.setTag(tag);
	_scrollbar.setClickable(true);

	setLength(1.0);
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
	_full = percent >= 1.0;

	// Calculate the actual length, at 2 pixel intervals
	_length = ceilf(MAX(_range * CLIP(percent, 0.0f, 1.0f), 10.0f));
	if ((((int) _length) % 2) == 1)
		_length += 1.0;

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

	return 0.0;
}

void WidgetScrollbar::mouseDown(uint8 state, float x, float y) {
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

void WidgetScrollbar::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	float steps = 1.0 / (_range - _length);

	if      (_type == Scrollbar::kTypeVertical)
		setState(_dragState + ((_dragY - y) * steps));
	else if (_type == Scrollbar::kTypeHorizontal)
		setState(_dragState + ((x - _dragX) * steps));

	setActive(true);
}

void WidgetScrollbar::mouseWheel(uint8 state, int x, int y) {
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
