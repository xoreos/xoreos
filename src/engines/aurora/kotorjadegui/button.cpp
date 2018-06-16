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
 *  A button widget for Star Wars: Knights of the Old Republic and Jade Empire.
 */

#include "src/common/system.h"

#include "src/aurora/gff3file.h"

#include "src/sound/sound.h"

#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightabletext.h"

#include "src/engines/aurora/util.h"

#include "src/engines/aurora/kotorjadegui/button.h"

namespace Engines {

WidgetButton::WidgetButton(GUI &gui, const Common::UString &tag)
		: KotORJadeWidget(gui, tag),
		  _permanentHighlight(false),
		  _disableHighlight(false),
		  _disableHoverSound(false),
		  _hovered(false) {
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::load(const Aurora::GFF3Struct &gff) {
	KotORJadeWidget::load(gff);

	Graphics::Aurora::Highlightable *highlightable = getTextHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);

	highlightable = getQuadHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);
}

void WidgetButton::setPermanentHighlight(bool permanentHighlight) {
	_permanentHighlight = permanentHighlight;
	setHighlight(_permanentHighlight);
}

void WidgetButton::setStaticHighlight() {
	Graphics::Aurora::Highlightable *highlightable = getQuadHighlightableComponent();
	if (highlightable)
		highlightable->setHighlightLowerBound(1.0f, 1.0f, 0.0f, 1.0f);
}

void WidgetButton::setDisableHighlight(bool disableHighlight) {
	_disableHighlight = disableHighlight;

	if (_disableHighlight) {
		setHighlight(false);
	} else {
		if (_hovered)
			setHighlight(true);
	}
}

void WidgetButton::setDisableHoverSound(bool disableHoverSound) {
	_disableHoverSound = disableHoverSound;
}

bool WidgetButton::isHovered() const {
	return _hovered;
}

void WidgetButton::enter() {
	if (!_disableHoverSound)
		_sound = playSound("gui_actscroll", Sound::kSoundTypeSFX);

	if (!_permanentHighlight && !_disableHighlight)
		setHighlight(true);

	// The button is hovered at the moment
	_hovered = true;
}

void WidgetButton::leave() {
	if (!_disableHoverSound)
		SoundMan.stopChannel(_sound);

	if (!_permanentHighlight && !_disableHighlight)
		setHighlight(false);

	// The button is not hovered anymore
	_hovered = false;
}

void WidgetButton::mouseUp(uint8 UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	playSound("gui_actuse", Sound::kSoundTypeSFX);
	setActive(true);
}

void WidgetButton::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0.0f, 0.0f, 0.0f, 0.05f);
	highlightable->setHighlightLowerBound(1.0f, 1.0f, 0.0f, 0.2f);
	highlightable->setHighlightUpperBound(1.0f, 1.0f, 0.0f, 1.0f);
}

} // End of namespace Engines
