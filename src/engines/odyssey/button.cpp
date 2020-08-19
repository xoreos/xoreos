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
 *  Button widget for the Odyssey engine.
 */

#include "src/graphics/aurora/textureman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/odyssey/button.h"

namespace Engines {

namespace Odyssey {

WidgetButton::WidgetButton(GUI &gui, const Common::UString &tag) :
		Widget(gui, tag),
		_permanentHighlight(false),
		_disableHighlight(false),
		_disableHoverSound(false),
		_hovered(false) {
}

void WidgetButton::load(const Aurora::GFF3Struct &gff) {
	Widget::load(gff);

	Graphics::Aurora::Highlightable *highlightable = getTextHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);

	highlightable = getQuadHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);
}

void WidgetButton::show() {
	if (isInvisible())
		return;

	Widget::show();

	if (_iconQuad)
		_iconQuad->show();
}

void WidgetButton::hide() {
	if (!isVisible())
		return;

	Widget::hide();

	if (_iconQuad)
		_iconQuad->hide();
}

void WidgetButton::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	float dx = x - oX;
	float dy = y - oY;
	float dz = z - oZ;

	Widget::setPosition(x, y, z);

	if (_iconQuad) {
		_iconQuad->getPosition(x, y, z);
		_iconQuad->setPosition(x + dx, y + dy, z + dz);
	}
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
	} else if (_hovered) {
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

	// The button is being hovered over
	_hovered = true;
}

void WidgetButton::leave() {
	if (!_disableHoverSound)
		SoundMan.stopChannel(_sound);

	if (!_permanentHighlight && !_disableHighlight)
		setHighlight(false);

	// The button is not being hovered over anymore
	_hovered = false;
}

void WidgetButton::mouseUp(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	playSound("gui_actuse", Sound::kSoundTypeSFX);
	setActive(true);
}

const Common::UString &WidgetButton::getIcon() const {
	return _icon;
}

void WidgetButton::setIcon(const Common::UString &icon) {
	if (_icon == icon)
		return;

	_icon = icon;

	if (icon.empty()) {
		if (_iconQuad) {
			_iconQuad->hide();
			_iconQuad.reset();
		}
		return;
	}

	Graphics::Aurora::TextureHandle textureHandle = TextureMan.get(icon);
	Graphics::Aurora::Texture &texture = textureHandle.getTexture();

	_iconQuad = std::make_unique<Graphics::Aurora::GUIQuad>(textureHandle, 0.0f, 0.0f, texture.getWidth(), texture.getHeight());

	float x, y, z;
	getPosition(x, y, z);

	_iconQuad->setPosition(
		x + getWidth() / 2.0f - texture.getWidth() / 2.0f,
		y + getHeight() / 2.0f - texture.getHeight() / 2.0f,
		z - 1.0f
	);

	if (isVisible())
		_iconQuad->show();
}

void WidgetButton::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0.0f, 0.0f, 0.0f, 0.05f);
	highlightable->setHighlightLowerBound(1.0f, 1.0f, 0.0f, 0.2f);
	highlightable->setHighlightUpperBound(1.0f, 1.0f, 0.0f, 1.0f);
}

} // End of namespace Odyssey

} // End of namespace Engines
