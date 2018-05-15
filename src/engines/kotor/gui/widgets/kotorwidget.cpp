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

#include "src/graphics/font.h"
#include "src/graphics/windowman.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightabletext.h"
#include "src/graphics/aurora/highlightableguiquad.h"
#include "src/graphics/aurora/types.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

KotORWidget::Extend::Extend() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {
}


KotORWidget::Border::Border() : fillStyle(0), dimension(0), innerOffset(0), hasColor(false),
	r(0.0f), g(0.0f), b(0.0f), pulsing(false) {

}


KotORWidget::Text::Text() : strRef(Aurora::kStrRefInvalid), halign(0.0f), valign(0.0f),
	r(1.0f), g(1.0f), b(1.0f), pulsing(false) {

}

KotORWidget::Hilight::Hilight() : fill("") {

}

KotORWidget::KotORWidget(::Engines::GUI &gui, const Common::UString &tag) :
	Widget(gui, tag), _width(0.0f), _height(0.0f), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f),
	_unselectedR(1.0f), _unselectedG(1.0f), _unselectedB(1.0f), _unselectedA(1.0f),
	_wrapped(false), _subScene(NULL), _highlighted(false) {

}

KotORWidget::~KotORWidget() {
}

void KotORWidget::show() {
	if (isInvisible())
		return;

	Widget::show();

	if (_quad)
		_quad->show();
	if (_text)
		_text->show();
	if (_border)
		_border->show();
	if (_subScene)
		_subScene->show();
}

void KotORWidget::hide() {
	if (isInvisible())
		return;

	if (_subScene)
		_subScene->hide();
	if (_border)
		_border->hide();
	if (_highlight)
		_highlight->hide();
	if (_quad)
		_quad->hide();
	if (_text)
		_text->hide();

	Widget::hide();
}

void KotORWidget::setWrapped(bool wrapped) {
	_wrapped = wrapped;
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

	if (_highlight) {
		float hX, hY, hZ;
		_highlight->getPosition(hX, hY, hZ);

		_highlight->setPosition(hX - oX + x, hY - oY + y, hZ - oZ + z);
	}

	if (_border) {
		_border->setPosition(x, y, z);
	}
}

void KotORWidget::setSubScene(Graphics::Aurora::SubSceneQuad *subscene) {
	if (!subscene) {
		_subScene = NULL;
		return;
	}

	_subScene = subscene;
	_subScene->setSize(_width, _height);

	float wWidth, wHeight;
	wWidth = WindowMan.getWindowWidth();
	wHeight = WindowMan.getWindowHeight();

	float x, y, z;
	getPosition(x, y, z);
	_subScene->setPosition(x + wWidth/2, y + wHeight/2);
}

void KotORWidget::setScissor(int x, int y, int width, int height) {
	_quad->setScissor(true);
	_quad->setScissor(x, y, width, height);
}

void KotORWidget::setHighlight(bool highlight) {
	float r, g, b, a;

	if (_highlighted == highlight)
		return;

	if (highlight) {
		if (_highlight) {
			_quad->hide();
			_highlight->show();
		} else {
			if (getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
				_text->getColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
				_text->getHighlightedLowerBound(r, g, b, a);
				_text->setColor(r, g, b, a);
				_text->setHighlighted(true);
			}

			if (getQuadHighlightableComponent() && getQuadHighlightableComponent()->isHighlightable()) {
				_quad->getColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
				getQuadHighlightableComponent()->getHighlightedLowerBound(r, g, b, a);
				_quad->setColor(r, g, b, a);
				getQuadHighlightableComponent()->setHighlighted(true);
			}
		}
	} else {
		if (_highlight) {
			_quad->show();
			_highlight->hide();
		} else {
			if (getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
				_text->setHighlighted(false);
				_text->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
			}
			if (getQuadHighlightableComponent() && getQuadHighlightableComponent()->isHighlightable()) {
				getQuadHighlightableComponent()->setHighlighted(false);
				_quad->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
			}
		}
	}

	_highlighted = highlight;
}

bool KotORWidget::isHighlight() {
	return _highlighted;
}

void KotORWidget::setWidth(float width) {
	if (_quad)
		_quad->setWidth(width);
}

void KotORWidget::setHeight(float height) {
	if (_quad)
		_quad->setHeight(height);
}

float KotORWidget::getWidth() const {
	return _width;
}

float KotORWidget::getHeight() const {
	return _height;
}

void KotORWidget::setFont(const Common::UString &fnt) {
	_text->setFont(fnt);
}

void KotORWidget::setFill(const Common::UString &fill) {
	if (fill.empty()) {
		_quad->hide();
		_quad.release();
		return;
	}

	if (!_quad) {
		float x, y, z;
		getPosition(x, y, z);

		_quad.reset(new Graphics::Aurora::GUIQuad("", 0.0f, 0.0f, _width, _height));
		_quad->setPosition(x, y, z);
		_quad->setTag(getTag());
		_quad->setClickable(true);

		if (isVisible())
			_quad->show();
	}

	_quad->setTexture(fill);
	_quad->setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void KotORWidget::setClickable(bool clickable) {
	if (_quad)
		_quad->setClickable(clickable);
	if (_text)
		_text->setClickable(clickable);
	if (_highlight)
		_highlight->setClickable(clickable);
}

void KotORWidget::load(const Aurora::GFF3Struct &gff) {
	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);

	Extend extend = createExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0f);

	Border border = createBorder(gff);
	Hilight hilight = createHilight(gff);

	if (!hilight.fill.empty()) {
		_highlight.reset(new Graphics::Aurora::GUIQuad(hilight.fill, 0, 0, extend.w, extend.h));
		_highlight->setPosition(extend.x, extend.y, 0.0f);
		_highlight->setTag(getTag());
		_highlight->setClickable(true);
	}

	if (!border.edge.empty() && !border.corner.empty()) {
		_border.reset(new Graphics::Aurora::BorderQuad(border.edge, border.corner,
		                                               extend.x, extend.y, extend.w, extend.h,
		                                               border.dimension));

		_border->setColor(border.r, border.g, border.b);
		if (!border.fill.empty()) {
			_quad.reset(new Graphics::Aurora::HighlightableGUIQuad(border.fill, 0.0f, 0.0f,
			                                                       extend.w - 2*border.dimension,
			                                                       extend.h - 2*border.dimension));
		} else {
			_quad.reset(new Graphics::Aurora::GUIQuad(border.fill, 0.0f, 0.0f,
			                                          extend.w - 2*border.dimension,
			                                          extend.h - 2*border.dimension));
		}
		_quad->setPosition(extend.x + border.dimension, extend.y + border.dimension, 0.0f);
	} else {
		if (!border.fill.empty()) {
			_quad.reset(new Graphics::Aurora::HighlightableGUIQuad(border.fill, 0.0f, 0.0f, extend.w, extend.h));
		} else {
			_quad.reset(new Graphics::Aurora::GUIQuad(border.fill, 0.0f, 0.0f, extend.w, extend.h));
		}
		_quad->setPosition(extend.x, extend.y, 0.0f);
	}

	_quad->setTag(getTag());
	_quad->setClickable(true);

	if (border.fill.empty())
		_quad->setColor(0.0f, 0.0f, 0.0f, 0.0f);
	else if (!border.hasColor)
		_quad->setColor(1.0f, 1.0f, 1.0f, 1.0f);
	else
		_quad->setColor(border.r, border.g, border.b, 1.0f);

	Text text = createText(gff);

	if (!text.font.empty()) {
		// Buttons in Jade are too small for their text
		// If widget height is less than the font needs
		// increase to font height and reposition
		float fontHeight = FontMan.get(text.font).getFont().getHeight();
		float tY = extend.y;
		float tH = extend.h;
		if (extend.h < fontHeight) {
			tH = fontHeight;
			tY = extend.y - (fontHeight - extend.h);
		}
		_text.reset(new Graphics::Aurora::HighlightableText(FontMan.get(text.font), extend.w, tH,
		            text.text, text.r, text.g, text.b, 1.0f, text.halign, text.valign));

		_text->setPosition(extend.x, tY, -1.0f);
		_text->setTag(getTag());
		_text->setClickable(true);
	}
}

void KotORWidget::setColor(float r, float g, float b, float a) {
		_quad->setColor(r, g, b, a);
}

void KotORWidget::setText(const Common::UString &text) {
	_text->setText(text);
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

		border.hasColor = b.hasField("COLOR");
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
		if (alignment == 10) {
			text.halign = Graphics::Aurora::kHAlignCenter;
			text.valign = Graphics::Aurora::kVAlignTop;
		} else if (alignment == 18) {
			text.halign = Graphics::Aurora::kHAlignCenter;
			text.valign = Graphics::Aurora::kVAlignMiddle;
		}
	}

	return text;
}

KotORWidget::Hilight KotORWidget::createHilight(const Aurora::GFF3Struct &gff) {
	Hilight hilight;

	if (gff.hasField("HILIGHT")) {
		const Aurora::GFF3Struct &h = gff.getStruct("HILIGHT");

		hilight.fill   = h.getString("FILL");
	}

	return hilight;
}

Graphics::Aurora::Highlightable* KotORWidget::getTextHighlightableComponent() const {
	return static_cast<Graphics::Aurora::Highlightable*>(_text.get());
}

Graphics::Aurora::Highlightable* KotORWidget::getQuadHighlightableComponent() const {
	return dynamic_cast<Graphics::Aurora::Highlightable*>(_quad.get());
}

void KotORWidget::setInvisible(bool invisible) {
	Widget::setInvisible(invisible);

	setClickable(!invisible);
}

} // End of namespace KotOR

} // End of namespace Engines
