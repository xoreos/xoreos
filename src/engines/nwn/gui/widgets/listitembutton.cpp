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
#include "src/engines/nwn/gui/widgets/listitembutton.h"

namespace Engines {

namespace NWN {

WidgetListItemBaseButton::WidgetListItemBaseButton(::Engines::GUI &gui,
                                                   const Common::UString &button, float spacing,
                                                   const Common::UString &soundClick) :
                                                   WidgetListItem(gui), _spacing(spacing),
                                                   _sound(soundClick) {

	_button = loadModelGUI(button);
	assert(_button);

	_button->setClickable(true);
}

WidgetListItemBaseButton::~WidgetListItemBaseButton() {
	delete _button;
}

void WidgetListItemBaseButton::show() {
	_button->show();
}

void WidgetListItemBaseButton::hide() {
	_button->hide();
}

void WidgetListItemBaseButton::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_button->setPosition(x, y, z);
}

void WidgetListItemBaseButton::mouseDown(uint8 state, float x, float y) {
	Engines::Widget::mouseDown(state, x, y);

	playSound(_sound, Sound::kSoundTypeSFX);
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

	return true;
}

WidgetListItemButton::WidgetListItemButton(::Engines::GUI &gui, const Common::UString &button,
                                           const Common::UString &text, const Common::UString &icon,
                                           const Common::UString &soundClick) :
                                           WidgetListItemBaseButton(gui, button, 1.0f, soundClick) {

	_text = new Graphics::Aurora::Text(FontMan.get("fnt_galahad14"), text);

	if (icon == "") {
		_icon = 0;
	} else {
		_icon = new Portrait(icon, Portrait::kSizeIcon);
	}
}

WidgetListItemButton::~WidgetListItemButton() {
	delete _text;

	if (_icon)
		delete _icon;
}

void WidgetListItemButton::show() {
	WidgetListItemBaseButton::show();

	_text->show();

	if (_icon)
		_icon->show();
}

void WidgetListItemButton::hide() {
	WidgetListItemBaseButton::hide();

	_text->hide();

	if (_icon)
		_icon->hide();
}

void WidgetListItemButton::setPosition(float x, float y, float z) {
	WidgetListItemBaseButton::setPosition(x, y, z);

	float pX, pY, pZ;
	if (_button->getNode("text0")) {
		_button->getNode("text0")->getPosition(pX, pY, pZ);
		pY -= _text->getHeight() * 1.5f;
	} else if (_button->getNode("text")) {
		_button->getNode("text")->getPosition(pX, pY, pZ);
		pY -= _text->getHeight() / 2;
	} else {
		pX = 0.0f;
		pY = -_text->getHeight() / 2;
		pZ = 5.0f;
	}

	_text->setPosition(x + pX, y + pY, z - pZ);

	if (!_icon)
		return;

	_button->getNode("icon")->getPosition(pX, pY, pZ);
	_icon->setPosition(x + pX - _icon->getWidth() / 2, y + pY - _icon->getHeight() / 2, z - 5);
}

void WidgetListItemButton::setTextColor(float r, float g, float b, float a) {
	_text->setColor(r, g, b, a);
}

} // End of namespace NWN

} // End of namespace Engines
