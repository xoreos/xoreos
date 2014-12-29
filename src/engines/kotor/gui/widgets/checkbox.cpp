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

/** @file engines/kotor/gui/widgets/checkbox.cpp
 *  A KotOR checkbox widget.
 */

#include "common/system.h"

#include "aurora/gfffile.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/highlightabletext.h"
#include "graphics/aurora/highlightableguiquad.h"

#include "engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::load(const Aurora::GFFStruct &gff) {
	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);

	Extend extend = createExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0);

	Border border = createBorder(gff);

	if (!border.fill.empty()) {
		_quad = new Graphics::Aurora::HighlightableGUIQuad(border.fill, 0.0, 0.0, extend.h * .62, extend.h * .62);
	} else {
		_quad = new Graphics::Aurora::GUIQuad(border.fill, 0.0, 0.0, extend.h * .62, extend.h * .62);
	}

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

} // End of namespace KotOR

} // End of namespace Engines
