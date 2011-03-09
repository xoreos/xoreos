/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/checkbox.cpp
 *  A NWN checkbox widget.
 */

#include "common/ustring.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn/gui/widgets/checkbox.h"

namespace Engines {

namespace NWN {

WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model) :
	ModelWidget(gui, tag, model) {

	_state = false;
	updateModel(false);
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::updateModel(bool highlight) {
	if (highlight) {
		if (_state)
			_model->setState("");
		else
			_model->setState("hilite");
	} else {
		if (_state)
			_model->setState("");
		else
			_model->setState("uncheckedup");
	}
}

bool WidgetCheckBox::getState() const {
	return _state;
}

void WidgetCheckBox::setState(bool state) {
	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (!state)
			// We can't just uncheck a radio button without checking another one
			return;

		_state = true;
		updateModel(false);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !!state;
		updateModel(false);
		setActive(true);
	}
}

void WidgetCheckBox::enter() {
	if (isDisabled())
		return;

	updateModel(true);
}

void WidgetCheckBox::leave() {
	if (isDisabled())
		return;

	updateModel(false);
}

void WidgetCheckBox::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	playSound("gui_check", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (_state)
			// We are already active
			return;

		_state = true;
		updateModel(true);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !_state;
		updateModel(true);
		setActive(true);
	}

}

void WidgetCheckBox::signalGroupMemberActive() {
	Widget::signalGroupMemberActive();

	_state = false;
	updateModel(false);
}

} // End of namespace NWN

} // End of namespace Engines
