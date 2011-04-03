/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/widgets/kotorwidget.cpp
 *  A KotOR widget.
 */

#include "common/util.h"

#include "aurora/gfffile.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/text.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

KotORWidget::Extend::Extend() : x(0.0), y(0.0), w(0.0), h(0.0) {
}


KotORWidget::Border::Border() : fillStyle(0), dimension(0), innerOffset(0),
	r(0), g(0), b(0), pulsing(false) {

}


KotORWidget::KotORWidget(::Engines::GUI &gui, const Common::UString &tag) :
	Widget(gui, tag), _width(0.0), _height(0.0), _r(1.0), _g(1.0), _b(1.0), _a(1.0),
	_quad(0), _text(0) {

}

KotORWidget::~KotORWidget() {
	delete _text;
	delete _quad;
}

void KotORWidget::show() {
	if (isInvisible())
		return;

	Widget::show();

	if (_quad)
		_quad->show();
	if (_text)
		_text->show();
}

void KotORWidget::hide() {
	if (isInvisible())
		return;

	if (_quad)
		_quad->hide();
	if (_text)
		_text->hide();

	Widget::hide();
}

void KotORWidget::setTag(const Common::UString &tag) {
	Widget::setTag(tag);

	if (_quad)
		_quad->setTag(getTag());
	if (_text)
		_text->setTag(getTag());
}

void KotORWidget::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	Widget::setPosition(x, y, z);
	getPosition(x, y, z);

	if (_quad) {
		float qX, qY, qZ;
		_quad->getPosition(qX, qY, qZ);

		_quad->setPosition(qX - oX + x, qY - oY + y, qZ - oZ + z);
	}
}

float KotORWidget::getWidth() const {
	return _width;
}

float KotORWidget::getHeight() const {
	return _height;
}

void KotORWidget::setFill(const Common::UString &fill) {
	if (!_quad) {
		float x, y, z;
		getPosition(x, y, z);

		_quad = new Graphics::Aurora::GUIQuad("", 0.0, 0.0, _width, _height);
		_quad->setPosition(x, y, z);

		if (isVisible())
			_quad->show();
	}

	_quad->setTexture(fill);
	_quad->setColor(1.0, 1.0, 1.0, 1.0);
}

void KotORWidget::load(const Aurora::GFFStruct &gff) {
	Extend extend = getExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0);

	Border border = getBorder(gff);

	if (!border.fill.empty()) {
		_quad = new Graphics::Aurora::GUIQuad(border.fill, 0.0, 0.0, extend.w, extend.h);
		_quad->setPosition(extend.x, extend.y, 0.0);
	}

	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);
}

KotORWidget::Extend KotORWidget::getExtend(const Aurora::GFFStruct &gff) {
	Extend extend;

	if (gff.hasField("EXTENT")) {
		const Aurora::GFFStruct &e = gff.getStruct("EXTENT");

		extend.x = (float) e.getSint("LEFT");
		extend.y = (float) e.getSint("TOP");
		extend.w = (float) e.getSint("WIDTH");
		extend.h = (float) e.getSint("HEIGHT");
	}

	return extend;
}

KotORWidget::Border KotORWidget::getBorder(const Aurora::GFFStruct &gff) {
	Border border;

	if (gff.hasField("BORDER")) {
		const Aurora::GFFStruct &b = gff.getStruct("BORDER");

		border.corner = b.getString("CORNER");
		border.edge   = b.getString("EDGE");
		border.fill   = b.getString("FILL");

		border.fillStyle   = b.getUint("FILLSTYLE");
		border.dimension   = b.getUint("DIMENSION");
		border.innerOffset = b.getUint("INNEROFFSET");

		b.getVector("COLOR", border.r, border.g, border.b);

		border.pulsing = b.getBool("PULSING");
	}

	return border;
}

} // End of namespace KotOR

} // End of namespace Engines
