/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	_x1 (x1) , _y1 (y1) , _x2 (x2) , _y2 (y2) ,
	_tX1(tX1), _tY1(tY1), _tX2(tX2), _tY2(tY2),
	_r(1.0), _g(1.0), _b(1.0), _a(1.0) {

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
	removeFromQueue();
}

void GUIQuad::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x2 = _x2 - _x1 + x;
	_y2 = _y2 - _y1 + y;

	_x1 = x;
	_y1 = y;

	_distance = z;
	GfxMan.resortObjects();

	GfxMan.unlockFrame();
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

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

} // End of namespace Aurora

} // End of namespace Graphics
