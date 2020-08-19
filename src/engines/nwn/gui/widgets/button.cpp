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
 *  A NWN button widget.
 */

#include "src/common/system.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/button.h"

namespace Engines {

namespace NWN {

WidgetButton::WidgetButton(::Engines::GUI &gui, const Common::UString &tag,
                           const Common::UString &model, const Common::UString &sound) :
	ModelWidget(gui, tag, model) {

	_model->setClickable(true);
	_model->setState("up");

	_sound = sound;
	_mode = kModeNormal;
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::enter() {
	ModelWidget::enter();

	if (isDisabled())
		return;

	if (_mode == kModeNormal)
		_model->setState("hilite");
}

void WidgetButton::leave() {
	ModelWidget::leave();

	if (isDisabled())
		return;

	if (_mode == kModeNormal)
		_model->setState("up");
}

void WidgetButton::setPosition(float x, float y, float z) {
	ModelWidget::setPosition(x, y, z);

	// Don't write on the border
	if (_caption && getHorizontalAlign() == Graphics::Aurora::kHAlignLeft) {
		_caption->getPosition(x, y, z);
		_caption->setPosition(x + 7, y, z);
	}
}

void WidgetButton::setMode(WidgetButton::Mode mode) {
	_mode = mode;

	if (mode == kModeUnchanged)
		_model->setState("disabled");
}

WidgetButton::Mode WidgetButton::getMode() const {
	return _mode;
}

void WidgetButton::setPressed(bool pushed) {
	if (pushed)
		_model->setState("down");
	else
		_model->setState("up");
}

bool WidgetButton::isPressed() const {
	if (_model->getState() == "down")
		return true;

	return false;
}

void WidgetButton::setDisabled(bool disabled) {
	ModelWidget::setDisabled(disabled);

	if (isDisabled())
		_model->setState("disabled");
	else
		_model->setState("up");
}

void WidgetButton::mouseDown(uint8_t state, float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	if (_mode == kModeNormal) {
		_model->setState("down");
		playSound(_sound, Sound::kSoundTypeSFX);
	} else if (_mode == kModeUnchanged) {
		playSound(_sound, Sound::kSoundTypeSFX);
	}
}

void WidgetButton::mouseUp(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	setActive(true);

	if (_mode == kModeNormal) {
		_model->setState("hilite");
	} else if (_mode == kModeToggle) {
		_model->setState("down");
		playSound(_sound, Sound::kSoundTypeSFX);
	}
}

void WidgetButton::mouseWheel(uint8_t state, int x, int y) {
	if (_owner) {
		_owner->mouseWheel(state, x, y);
	}
}

} // End of namespace NWN

} // End of namespace Engines
