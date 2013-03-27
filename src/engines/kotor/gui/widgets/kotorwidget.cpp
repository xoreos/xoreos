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

/** @file engines/kotor/gui/widgets/kotorwidget.cpp
 *  A KotOR widget.
 */

#include "common/util.h"

#include "aurora/types.h"
#include "aurora/gfffile.h"
#include "aurora/talkman.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/highlightabletext.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

KotORWidget::Extend::Extend() : x(0.0), y(0.0), w(0.0), h(0.0) {
}


KotORWidget::Border::Border() : fillStyle(0), dimension(0), innerOffset(0),
	r(0.0), g(0.0), b(0.0), pulsing(false) {

}


KotORWidget::Text::Text() : strRef(Aurora::kStrRefInvalid), halign(0.0), valign(0.0),
	r(1.0), g(1.0), b(1.0), pulsing(false) {

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

	if (_text) {
		float tX, tY, tZ;
		_text->getPosition(tX, tY, tZ);

		_text->setPosition(tX - oX + x, tY - oY + y, tZ - oZ + z);
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
		_quad->setTag(getTag());
		_quad->setClickable(true);

		if (isVisible())
			_quad->show();
	}

	_quad->setTexture(fill);
	_quad->setColor(1.0, 1.0, 1.0, 1.0);
}

void KotORWidget::load(const Aurora::GFFStruct &gff) {
	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);

	Extend extend = createExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0);

	Border border = createBorder(gff);

	_quad = new Graphics::Aurora::GUIQuad(border.fill, 0.0, 0.0, extend.w, extend.h);
	_quad->setPosition(extend.x, extend.y, 0.0);
	_quad->setTag(getTag());
	_quad->setClickable(true);

	if (border.fill.empty())
		_quad->setColor(0.0, 0.0, 0.0, 0.0);

	Text text = createText(gff);

	if (!text.text.empty() && !text.font.empty()) {
		_text = new Graphics::Aurora::HighlightableText(FontMan.get(text.font), text.text,
		                                   text.r, text.g, text.b, 1.0);

		const float hspan = extend.w - _text->getWidth();
		const float vspan = extend.h - _text->getHeight();


		const float x = extend.x + text.halign * hspan;
		const float y = extend.y + text.valign * vspan;

		_text->setPosition(x, y, -1.0);
		_text->setTag(getTag());
		_text->setClickable(true);
	}
}

void KotORWidget::setColor(float r, float g, float b, float a) {
		_quad->setColor(r, g, b, a);
}

void KotORWidget::setText(const Common::UString &text) {
		float extendX, extendY, extendZ, textX, textY, textZ;
		Widget::getPosition(extendX, extendY, extendZ);
		_text->getPosition(textX, textY, textZ);

		const float halign = (textX - extendX) / (_width - _text->getWidth());
		const float valign = (textY - extendY) / (_height - _text->getHeight());
		 _text->set(text);

		const float hspan = _width - _text->getWidth();
		const float vspan = _height - _text->getHeight();

		const float x = extendX + halign * hspan;
		const float y = extendY + valign * vspan;

		_text->setPosition(x, y, -1.0);
}

KotORWidget::Extend KotORWidget::createExtend(const Aurora::GFFStruct &gff) {
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

KotORWidget::Border KotORWidget::createBorder(const Aurora::GFFStruct &gff) {
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

KotORWidget::Text KotORWidget::createText(const Aurora::GFFStruct &gff) {
	Text text;

	if (gff.hasField("TEXT")) {
		const Aurora::GFFStruct &t = gff.getStruct("TEXT");

		text.font   = t.getString("FONT");
		text.text   = t.getString("TEXT");
		text.strRef = t.getUint("STRREF", Aurora::kStrRefInvalid);

		const uint32 alignment = t.getUint("ALIGNMENT");

		t.getVector("COLOR", text.r, text.g, text.b);

		text.pulsing = t.getBool("PULSING");


		if (text.text == "(Unitialized)")
			text.text.clear();

		if (text.strRef != Aurora::kStrRefInvalid)
			text.text = TalkMan.getString(text.strRef);

		// TODO: KotORWidget::getText(): Alignment
		if (alignment == 18) {
			text.halign = 0.5;
			text.valign = 0.5;
		}
	}

	return text;
}

Graphics::Aurora::Highlightable* KotORWidget::getTextHighlightableComponent() const {
	return static_cast<Graphics::Aurora::Highlightable*>(_text);
}

} // End of namespace KotOR

} // End of namespace Engines
