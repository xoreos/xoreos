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
 *  Button items used in WidgetListBox.
 */

#include <cassert>

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/aurora/model.h"
#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/portrait.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/listitembutton.h"

namespace Engines {

namespace NWN {

WidgetListItemBaseButton::WidgetListItemBaseButton(::Engines::GUI &gui,
                                                   const Common::UString &button, float spacing,
                                                   const Common::UString &soundClick) :
                                                   WidgetListItem(gui), _spacing(spacing),
                                                   _sound(soundClick) {

	_button.reset(loadModelGUI(button));
	assert(_button);

	_button->setClickable(true);
	_channelHandle = Sound::ChannelHandle();
}

WidgetListItemBaseButton::~WidgetListItemBaseButton() {
}

void WidgetListItemBaseButton::show() {
	WidgetListItem::show();

	_button->show();
}

void WidgetListItemBaseButton::hide() {
	WidgetListItem::hide();

	_button->hide();
}

void WidgetListItemBaseButton::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);
}

void WidgetListItemBaseButton::mouseDown(uint8_t state, float x, float y) {
	Engines::Widget::mouseDown(state, x, y);

	if (SoundMan.isValidChannel(_channelHandle))
		SoundMan.stopChannel(_channelHandle);

	_channelHandle = playSound(_sound, Sound::kSoundTypeSFX);
}

float WidgetListItemBaseButton::getWidth() const {
	return _button->getWidth();
}

float WidgetListItemBaseButton::getHeight() const {
	return _button->getHeight() + _spacing;
}

void WidgetListItemBaseButton::setTag(const Common::UString &tag) {
	WidgetListItem::setTag(tag);

	_button->setTag(tag);
}

void WidgetListItemBaseButton::setSound(const Common::UString &sound) {
	_sound = sound;
}

bool WidgetListItemBaseButton::activate() {
	if (!WidgetListItem::activate())
		return false;

	_button->setState("down");

	return true;
}

bool WidgetListItemBaseButton::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_button->setState("");
	if (SoundMan.isPlaying(_channelHandle))
		SoundMan.stopChannel(_channelHandle);

	return true;
}

WidgetListItemButton::WidgetListItemButton(::Engines::GUI &gui, const Common::UString &button,
                                           const Common::UString &text, const Common::UString &icon,
                                           uint32_t otherButtons, const Common::UString &soundClick) :
	WidgetListItemBaseButton(gui, button, 1.0f, soundClick),
	_isRight(true), _isMovable(false), _moveButtonRight(0), _moveButtonLeft(0) {

	_text = std::make_unique<Graphics::Aurora::Text>(FontMan.get("fnt_galahad14"), text);

	if (!icon.empty())
		_icon = std::make_unique<Portrait>(icon, Portrait::kSizeIcon);

	if (otherButtons & kHelpButton)
		_helpButton.reset(loadModelGUI("ctl_cg_btn_help"));

	if (otherButtons & kMoveButton) {
		_isMovable = true;

		_moveButtonRight = new WidgetButton(gui, "Item#" + text + "#MoveButtonRight", "ctl_cg_btn_right");
		addSub(*_moveButtonRight);

		_moveButtonLeft = new WidgetButton(gui, "Item#" + text + "#MoveButtonLeft", "ctl_cg_btn_left");
		addSub(*_moveButtonLeft);
	}
}

WidgetListItemButton::~WidgetListItemButton() {
}

void WidgetListItemButton::show() {
	WidgetListItemBaseButton::show();

	_text->show();

	if (_icon)
		_icon->show();

	if (_moveButtonRight) {
		if (_isRight) {
			_moveButtonRight->show();
			if (_moveButtonLeft->isVisible())
				_moveButtonLeft->hide();
		} else {
			_moveButtonLeft->show();
			if (_moveButtonRight->isVisible())
				_moveButtonRight->hide();
		}
	}
}

void WidgetListItemButton::hide() {
	WidgetListItemBaseButton::hide();

	_text->hide();

	if (_icon)
		_icon->hide();

	if (_moveButtonRight) {
		if (_isRight) {
			_moveButtonRight->hide();
		} else {
			_moveButtonLeft->hide();
		}
	}
}

void WidgetListItemButton::setPosition(float x, float y, float z) {
	WidgetListItemBaseButton::setPosition(x, y, z);

	float pX, pY, pZ;
	if (_button->getNode("text0")) {
		_button->getNode("text0")->getPosition(pX, pY, pZ);
		pY -= (4 - _text->getLineCount()) * _text->getHeight() * 0.5f;
	} else if (_button->getNode("text")) {
		_button->getNode("text")->getPosition(pX, pY, pZ);
		pY -= _text->getHeight() / 2;
	} else {
		pX = 0.0f;
		pY = -_text->getHeight() / 2;
		pZ = 5.0f;
	}

	_text->setPosition(x + pX, y + pY, z - pZ);

	if (_moveButtonRight) {
		_button->getNode("addremovebutton")->getPosition(pX, pY, pZ);
		_moveButtonRight->setPosition(x + pX, y + pY, z - pZ - 100.f);
		_moveButtonLeft->setPosition(x + pX, y + pY, z - pZ - 100.f);
	}

	if (_helpButton) {
		_button->getNode("helpbutton")->getPosition(pX, pY, pZ);
		_helpButton->setPosition(x + pX, y + pY, z - pZ - 100.f);
	}

	if (!_icon)
		return;

	_button->getNode("icon")->getPosition(pX, pY, pZ);
	_icon->setPosition(x + pX - _icon->getWidth() / 2, y + pY - _icon->getHeight() / 2, z - 100.f);
}

void WidgetListItemButton::mouseDown(uint8_t state, float x, float y) {
	Engines::NWN::WidgetListItemBaseButton::mouseDown(state, x, y);

	if (_helpButton) {
		if (_helpButton->isIn(x, y)) {
			_helpButton->setState("down");
		}
	}
}

void WidgetListItemButton::mouseUp(uint8_t state, float x, float y) {
	Engines::NWN::WidgetListItemBaseButton::mouseUp(state, x, y);

	if (_helpButton) {
		_helpButton->setState("up");
		if (_helpButton->isIn(x, y)) {
			callbackHelp();
		}
	}
}

void WidgetListItemButton::enter() {
	if (_helpButton)
		_helpButton->show();
}

void WidgetListItemButton::leave() {
	if (_helpButton)
		_helpButton->hide();
}

bool WidgetListItemButton::movable() const {
	return _isMovable;
}

void WidgetListItemButton::setUnmovable() {
	_isMovable = false;
	_text->setColor(0.5f, 0.5f, 0.5f, 1.f);
}

void WidgetListItemButton::setTextColor(float r, float g, float b, float a) {
	_text->setColor(r, g, b, a);
}

void WidgetListItemButton::changeArrowDirection() {
	if (!_moveButtonRight)
		return;

	_isRight = !_isRight;

	if (isVisible()) {
		hide();
		show();
	}
}

void WidgetListItemButton::callbackHelp() {
}

} // End of namespace NWN

} // End of namespace Engines
