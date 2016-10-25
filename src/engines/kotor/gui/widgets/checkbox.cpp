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
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/engines/aurora/util.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"

namespace Engines {

namespace KotOR {

WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag), _state(false),
	_unselectedR(1.0f), _unselectedG(1.0f), _unselectedB(1.0f), _unselectedA(1.0f) {

}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);

	Border border = createBorder(gff);
	Graphics::Aurora::TextureHandle texture = TextureMan.get(border.fill);

	float squareLength = _quad->getHeight();
	float _quadPosCorrect = (squareLength - (texture.getTexture().getHeight() * 0.625f)) / 2;
	float x, y, z;
	if (_quad) {
		_quad->getPosition(x, y, z);
		_quad->setPosition(x + _quadPosCorrect, y + _quadPosCorrect, z);
		_quad->setHeight(texture.getTexture().getHeight() * 0.625f);
		_quad->setWidth(texture.getTexture().getWidth() * 0.625f);
	}
	if (_text) {
		_text->getPosition(x, y, z);
		_text->setPosition(x + squareLength, y, z);
		_text->setSize(_text->getWidth() - squareLength, _text->getHeight());
	}

	if (getTextHighlightableComponent() != 0) {
		setTextHighlighting(getTextHighlightableComponent());
	}
	if (getQuadHighlightableComponent() != 0) {
		setQuadHighlighting(getQuadHighlightableComponent());
	}
}

bool WidgetCheckBox::getState() const {
	return _state;
}

void WidgetCheckBox::setState(bool state) {
	_state = state;

	// Preserve the current color after replacing the texture
	float textR, textG, textB, textA;
	float quadR, quadG, quadB, quadA;
	_text->getColor(textR, textG, textB, textA);
	_quad->getColor(quadR, quadG, quadB, quadA);

	setFill(_state ? "i_checkbox02" : "i_checkbox01");

	_text->setColor(textR, textG, textB, textA);
	_quad->setColor(quadR, quadG, quadB, quadA);

	setActive(true);
}

void WidgetCheckBox::mouseUp(uint8 UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	setState(!_state);
	playSound("gui_check", Sound::kSoundTypeSFX);
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

void WidgetCheckBox::setTextHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0, 0, 0, .05);
	highlightable->setHighlightLowerBound(1, 1, 0, .2);
	highlightable->setHighlightUpperBound(1, 1, 0, 1);
}

void WidgetCheckBox::setQuadHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightLowerBound(1, 1, 0, 1);
}

} // End of namespace KotOR

} // End of namespace Engines
