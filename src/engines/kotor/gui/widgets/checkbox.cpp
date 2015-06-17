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
 *  A KotOR checkbox widget.
 */

#include "src/common/system.h"

#include "src/aurora/gff3file.h"

#include "src/sound/sound.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightabletext.h"
#include "src/graphics/aurora/highlightableguiquad.h"

#include "src/engines/aurora/util.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::load(const Aurora::GFF3Struct &gff) {
	gff.getVector("COLOR", _r, _g, _b);
	_a = gff.getDouble("ALPHA", 1.0);

	Extend extend = createExtend(gff);

	_width  = extend.w;
	_height = extend.h;

	Widget::setPosition(extend.x, extend.y, 0.0f);

	Border border = createBorder(gff);

	if (!border.fill.empty()) {
		_quad = new Graphics::Aurora::HighlightableGUIQuad(border.fill, 0.0f, 0.0f, extend.h * .62, extend.h * .62);
	} else {
		_quad = new Graphics::Aurora::GUIQuad(border.fill, 0.0f, 0.0f, extend.h * .62, extend.h * .62);
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

	if (getTextHighlightableComponent() != 0) {
		  setDefaultHighlighting(getTextHighlightableComponent());
	}
	if (getQuadHighlightableComponent() != 0) {
		  setDefaultHighlighting(getQuadHighlightableComponent());
	}
}

void WidgetCheckBox::mouseUp(uint8 UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	playSound("gui_actuse", Sound::kSoundTypeSFX);
	setActive(true);
}

void WidgetCheckBox::enter() {
	float r, g, b, a;
	_sound = playSound("gui_actscroll", Sound::kSoundTypeSFX);
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

void WidgetCheckBox::leave() {
	SoundMan.stopChannel(_sound);
	if (getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
		_text->setHighlighted(false);
		_text->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
	}
	if (getQuadHighlightableComponent() && getQuadHighlightableComponent()->isHighlightable()) {
		getQuadHighlightableComponent()->setHighlighted(false);
		_quad->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
	}
}

void WidgetCheckBox::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0, 0, 0, .05);
	highlightable->setHighlightLowerBound(1, 1, 0, .2);
	highlightable->setHighlightUpperBound(1, 1, 0, 1);
}

} // End of namespace KotOR

} // End of namespace Engines
