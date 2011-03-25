/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/textwidget.cpp
 *  A NWN text widget.
 */

#include "common/ustring.h"

#include "graphics/font.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/fontman.h"

#include "engines/nwn/gui/widgets/textwidget.h"

namespace Engines {

namespace NWN {

TextWidget::TextWidget(::Engines::GUI &gui, const Common::UString &tag,
                       const Common::UString &font, const Common::UString &text) :
	NWNWidget(gui, tag), _r(1.0), _g(1.0), _b(1.0), _a(1.0) {

	_text = new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a, 0.5);
	_text->setTag(tag);
}

TextWidget::~TextWidget() {
	delete _text;
}

void TextWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_text->show();

	NWNWidget::show();
}

void TextWidget::hide() {
	if (!isVisible())
		return;

	_text->hide();
	NWNWidget::hide();
}

void TextWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y, z);
}

void TextWidget::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_text->setColor(_r, _g, _b, _a);
}

void TextWidget::setText(const Common::UString &text) {
	_text->set(text);
}

float TextWidget::getWidth() const {
	return _text->getWidth();
}

float TextWidget::getHeight() const {
	return _text->getHeight();
}

void TextWidget::setDisabled(bool disabled) {
	if (isDisabled())
		return;

	_a = disabled ? (_a * 0.6) : (_a / 0.6);

	_text->setColor(_r, _g, _b, _a);

	NWNWidget::setDisabled(disabled);
}

} // End of namespace NWN

} // End of namespace Engines
