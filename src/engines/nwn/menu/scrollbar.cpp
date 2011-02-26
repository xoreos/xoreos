/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/scrollbar.cpp
 *  The GUI scrollbar model.
 */

#include "common/util.h"
#include "common/maths.h"

#include "engines/nwn/menu/scrollbar.h"

#include "graphics/graphics.h"

namespace Engines {

namespace NWN {

Scrollbar::Scrollbar(Type type) : _type(type), _x(0.0), _y(0.0), _z(0.0) {
	_texture = TextureMan.get("gui_scrollbar");

	setLength(16.0);
}

Scrollbar::~Scrollbar() {
	removeFromQueue();
}

void Scrollbar::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x = x;
	_y = y;
	_z = z;

	GfxMan.unlockFrame();
}

bool Scrollbar::isIn(float x, float y) const {
	// TODO
	return false;
}

void Scrollbar::setLength(float length) {
	GfxMan.lockFrame();

	_length = MAX(length, 4.0f);

	if      (_type == kTypeVertical)
		createV();
	else if (_type == kTypeHorizontal)
		createH();

	GfxMan.unlockFrame();
}

void Scrollbar::show() {
	addToQueue();
}

void Scrollbar::hide() {
	removeFromQueue();
}

bool Scrollbar::isVisible() {
	return isInQueue();
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

void Scrollbar::newFrame() {
	_distance = -_z;
}

void Scrollbar::render() {
	TextureMan.set(_texture);

	glTranslatef(_x, _y, 0.0);

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
	int n = ceilf(_length / 16.0);

	float x = 0.0;
	float y = 0.0;

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
	int n = ceilf(_length / 16.0);

	float x = 0.0;
	float y = 0.0;

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

} // End of namespace NWN

} // End of namespace Engines
