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
 *  A KotOR button widget.
 */

#include "src/common/system.h"
#include "src/aurora/gff3file.h"

#include "src/sound/sound.h"

#include "src/graphics/aurora/guiquad.h"

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightabletext.h"
#include "src/graphics/aurora/highlightableborder.h"
#include "src/graphics/aurora/textureman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/kotor/gui/widgets/button.h"

namespace Engines {

namespace KotOR {

WidgetButton::WidgetButton(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag), _permanentHighlight(false), _disableHighlight(false),
	_disableHoverSound(false),  _hovered(false), _highlighted(false) {
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::setPermanentHighlight(bool permanentHighlight) {
	_permanentHighlight = permanentHighlight;

	if (_permanentHighlight) {
		if (!_highlighted)
			startHighlight();
	} else {
		if (_highlighted)
			stopHighlight();
	}
}

void WidgetButton::setDisableHighlight(bool disableHighlight) {
	_disableHighlight = disableHighlight;

	if (_disableHighlight) {
		if (_highlighted)
			stopHighlight();
	} else {
		if (_hovered)
			startHighlight();
	}
}

void WidgetButton::setDisableHoverSound(bool disableHoverSound) {
	_disableHoverSound = disableHoverSound;
}

void WidgetButton::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);
	if (getTextHighlightableComponent() != 0) {
		  setDefaultHighlighting(getTextHighlightableComponent());
	}
	if (getQuadHighlightableComponent() != 0) {
		  setDefaultHighlighting(getQuadHighlightableComponent());
	}
    if (getBorderHighlightableComponent() != 0) {
        setDefaultHighlighting(getBorderHighlightableComponent());
    }
}

bool WidgetButton::isHovered() {
	return _hovered;
}

void WidgetButton::mouseUp(uint8 UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	playSound("gui_actuse", Sound::kSoundTypeSFX);
	setActive(true);
}

void WidgetButton::enter() {
	if (!_disableHoverSound)
		_sound = playSound("gui_actscroll", Sound::kSoundTypeSFX);

	if (!_permanentHighlight && !_disableHighlight) {
		startHighlight();
	}

	// The button is hovered at the moment
	_hovered = true;
}

void WidgetButton::leave() {
	if (!_disableHoverSound)
		SoundMan.stopChannel(_sound);

	if (!_permanentHighlight && !_disableHighlight) {
		stopHighlight();
	}

	// The buttone is not hovered anymore
	_hovered = false;
}

void WidgetButton::startHighlight() {
	float r, g, b, a;

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

    if (getBorderHighlightableComponent() && getBorderHighlightableComponent()->isHighlightable()) {
        getBorderHighlightableComponent()->setHighlighted(true);
    }

	_highlighted = true;
}

void WidgetButton::stopHighlight() {
	if (getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
		_text->setHighlighted(false);
		_text->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
	}
	if (getQuadHighlightableComponent() && getQuadHighlightableComponent()->isHighlightable()) {
		getQuadHighlightableComponent()->setHighlighted(false);
		_quad->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
	}

    if (getBorderHighlightableComponent() && getBorderHighlightableComponent()->isHighlightable()) {
        getBorderHighlightableComponent()->setHighlighted(false);
    }

	_highlighted = false;
}

void WidgetButton::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0, 0, 0, .05);
	highlightable->setHighlightLowerBound(1, 1, 0, .2);
	highlightable->setHighlightUpperBound(1, 1, 0, 1);
}

void WidgetButton::setHighlightEdgeTexture() {
    if(getBorderHighlightableComponent() != 0) {
        ((Graphics::Aurora::HighlightableBorder*) getBorderHighlightableComponent())->setHighlightEdgeTexture(TextureMan.get("boxline6"));
    }
}

void WidgetButton::setHighlightCornerTexture() {
    if(getBorderHighlightableComponent() != 0) {
        ((Graphics::Aurora::HighlightableBorder*) getBorderHighlightableComponent())->setHighlightCornerTexture(TextureMan.get("boxline5"));
    }
}

} // End of namespace KotOR

} // End of namespace Engines
