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
#include "common/ustring.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/text.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

KotORWidget::KotORWidget(::Engines::GUI &gui, const Common::UString &tag) :
	Widget(gui, tag), _quad(0), _text(0) {

}

KotORWidget::~KotORWidget() {
	delete _text;
	delete _quad;
}

void KotORWidget::show() {
	Widget::show();

	if (_quad)
		_quad->show();
	if (_text)
		_text->show();
}

void KotORWidget::hide() {
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
	Widget::setPosition(x, y, z);
	getPosition(x, y, z);

	_quad->setPosition(x, y, z);
}

float KotORWidget::getWidth() const {
	float x, y, z;
	getPosition(x, y, z);

	float qX = x, qY = y, qZ = z, qW = 0.0;
	if (_quad) {
		_quad->getPosition(qX, qY, qZ);
		qW = _quad->getWidth();
	}

	float tX = x, tY = y, tZ = z, tW = 0.0;
	if (_text) {
		_text->getPosition(tX, tY, tZ);
		tW = _text->getWidth();
	}

	return MAX((qX - x) + qW, (tX - x) + tW);
}

float KotORWidget::getHeight() const {
	float x, y, z;
	getPosition(x, y, z);

	float qX = x, qY = y, qZ = z, qH = 0.0;
	if (_quad) {
		_quad->getPosition(qX, qY, qZ);
		qH = _quad->getHeight();
	}

	float tX = x, tY = y, tZ = z, tH = 0.0;
	if (_text) {
		_text->getPosition(tX, tY, tZ);
		tH = _text->getHeight();
	}

	return MAX((qY - y) + qH, (tY - y) + tH);
}

} // End of namespace KotOR

} // End of namespace Engines
