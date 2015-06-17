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
 *  A KotOR widget.
 */

#include "src/common/util.h"

#include "src/aurora/types.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightabletext.h"
#include "src/graphics/aurora/highlightableguiquad.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

KotORWidget::Extend::Extend() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {
}


KotORWidget::Border::Border() : fillStyle(0), dimension(0), innerOffset(0),
	r(0.0f), g(0.0f), b(0.0f), pulsing(false) {

}


KotORWidget::Text::Text() : strRef(Aurora::kStrRefInvalid), halign(0.0f), valign(0.0f),
	r(1.0f), g(1.0f), b(1.0f), pulsing(false) {

}


KotORWidget::KotORWidget(::Engines::GUI &gui, const Common::UString &tag) :
	Widget(gui, tag), _width(0.0f), _height(0.0f), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f),
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

		_quad = new Graphics::Aurora::GUIQuad("", 0.0f, 0.0f, _width, _height);
		_quad->setPosition(x, y, z);
		_quad->setTag(getTag());
		_quad->setClickable(true);

		if (isVisible())
			_quad->show();
	}

	_quad->setTexture(fill);
	_quad->setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void KotORWidget::load(const Aurora::GFF3Struct &gff) {
	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);

	Extend extend = createExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0f);

	Border border = createBorder(gff);

	if (!border.fill.empty()) {
		_quad = new Graphics::Aurora::HighlightableGUIQuad(border.fill, 0.0f, 0.0f, extend.w, extend.h);
	} else {
		_quad = new Graphics::Aurora::GUIQuad(border.fill, 0.0f, 0.0f, extend.w, extend.h);
	}

	_quad->setPosition(extend.x, extend.y, 0.0f);
	_quad->setTag(getTag());
	_quad->setClickable(true);

	if (border.fill.empty())
		_quad->setColor(0.0f, 0.0f, 0.0f, 0.0f);

	Text text = createText(gff);

	if (!text.text.empty() && !text.font.empty()) {
		_text = new Graphics::Aurora::HighlightableText(FontMan.get(text.font), text.text,
		                                   text.r, text.g, text.b, 1.0f);

		const float hspan = extend.w - _text->getWidth();
		const float vspan = extend.h - _text->getHeight();


		const float x = extend.x + text.halign * hspan;
		const float y = extend.y + text.valign * vspan;

		_text->setPosition(x, y, -1.0f);
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

		_text->setPosition(x, y, -1.0f);
}

KotORWidget::Extend KotORWidget::createExtend(const Aurora::GFF3Struct &gff) {
	Extend extend;

	if (gff.hasField("EXTENT")) {
		const Aurora::GFF3Struct &e = gff.getStruct("EXTENT");

		extend.x = (float) e.getSint("LEFT");
		extend.y = (float) e.getSint("TOP");
		extend.w = (float) e.getSint("WIDTH");
		extend.h = (float) e.getSint("HEIGHT");
	}

	return extend;
}

KotORWidget::Border KotORWidget::createBorder(const Aurora::GFF3Struct &gff) {
	Border border;

	if (gff.hasField("BORDER")) {
		const Aurora::GFF3Struct &b = gff.getStruct("BORDER");

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

KotORWidget::Text KotORWidget::createText(const Aurora::GFF3Struct &gff) {
	Text text;

	if (gff.hasField("TEXT")) {
		const Aurora::GFF3Struct &t = gff.getStruct("TEXT");

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
			text.halign = 0.5f;
			text.valign = 0.5f;
		}
	}

	return text;
}

Graphics::Aurora::Highlightable* KotORWidget::getTextHighlightableComponent() const {
	return static_cast<Graphics::Aurora::Highlightable*>(_text);
}

Graphics::Aurora::Highlightable* KotORWidget::getQuadHighlightableComponent() const {
	return dynamic_cast<Graphics::Aurora::Highlightable*>(_quad);
}

} // End of namespace KotOR

} // End of namespace Engines
