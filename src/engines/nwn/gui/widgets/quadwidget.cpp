/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/quadwidget.cpp
 *  A NWN quad widget.
 */

#include "common/util.h"
#include "common/ustring.h"

#include "graphics/aurora/guiquad.h"

#include "engines/nwn/gui/widgets/quadwidget.h"

namespace Engines {

namespace NWN {

QuadWidget::QuadWidget(::Engines::GUI &gui, const Common::UString &tag,
                       const Common::UString &texture,
                       float  x1, float  y1, float  x2, float  y2,
                       float tX1, float tY1, float tX2, float tY2) :
	NWNWidget(gui, tag) {

	_quad = new Graphics::Aurora::GUIQuad(texture, x1, y1, x2, y2, tX1, tY1, tX2, tY2);
	_quad->setTag(tag);
	_quad->setClickable(true);

	_width  = ABS(x2 - x1);
	_height = ABS(y2 - y1);
}

QuadWidget::~QuadWidget() {
	delete _quad;
}

void QuadWidget::show() {
	_quad->show();
}

void QuadWidget::hide() {
	_quad->hide();
}

void QuadWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_quad->setPosition(x, y, z);
}

void QuadWidget::setColor(float r, float g, float b, float a) {
	_quad->setColor(r, g, b, a);
}

void QuadWidget::setTexture(const Common::UString &texture) {
	_quad->setTexture(texture);
}

void QuadWidget::setWidth(float w) {
	_quad->setWidth(w);
}

void QuadWidget::setHeight(float h) {
	_quad->setHeight(h);
}

float QuadWidget::getWidth() const {
	return _width;
}

float QuadWidget::getHeight() const {
	return _height;
}

} // End of namespace NWN

} // End of namespace Engines
