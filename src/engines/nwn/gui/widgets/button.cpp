/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/button.cpp
 *  A NWN button widget.
 */

#include "graphics/aurora/model_nwn.h"

#include "engines/aurora/util.h"

#include "engines/nwn/gui/widgets/button.h"

namespace Engines {

namespace NWN {

WidgetButton::WidgetButton(::Engines::GUI &gui, const Common::UString &tag,
                           const Common::UString &model, const Common::UString &sound) :
	ModelWidget(gui, tag, model) {

	_model->setSelectable(true);
	_model->setState("");
	updateSize();

	_sound = sound;
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::enter() {
	ModelWidget::enter();

	if (isDisabled())
		return;

	_model->setState("hilite");

	updateSize();
}

void WidgetButton::leave() {
	ModelWidget::leave();

	if (isDisabled())
		return;

	_model->setState("");

	updateSize();
}

void WidgetButton::setDisabled(bool disabled) {
	NWNWidget::setDisabled(disabled);

	if (isDisabled())
		_model->setState("disabled");
	else
		_model->setState("");

	updateSize();
}

void WidgetButton::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_model->setState("down");

	updateSize();

	playSound(_sound, Sound::kSoundTypeSFX);
}

void WidgetButton::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (!_model->setState("hilite"))
		_model->setState("");

	updateSize();

	setActive(true);
}

} // End of namespace NWN

} // End of namespace Engines
