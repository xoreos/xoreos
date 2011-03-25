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
#include "graphics/aurora/modelnode.h"

#include "engines/aurora/util.h"

#include "engines/nwn/gui/widgets/checkbox.h"

namespace Engines {

namespace NWN {

WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model) :
	ModelWidget(gui, tag, model) {

	_model->setClickable(true);

	Graphics::Aurora::ModelNode *node = 0;

	_model->setState("uncheckeddown");
	if ((node = _model->getNode("XPlane")))
		node->setInvisible(true);

	_model->setState("checkedup");
	if ((node = _model->getNode("XPlane")))
		node->move(0.0, 0.0, -10.0);
	_model->setState("checkeddown");
	if ((node = _model->getNode("XPlane")))
		node->move(0.0, 0.0, -10.0);
	_model->setState("checkedhilite");
	if ((node = _model->getNode("XPlane")))
		node->move(0.0, 0.0, -10.0);

	_state = false;
	_down  = false;
	updateModel(false);
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::updateModel(bool highlight) {
	if (highlight) {
		if (_state) {
			if (_down)
				_model->setState("checkeddown");
			else
				_model->setState("checkedhilite");
		} else {
			if (_down)
				_model->setState("uncheckeddown");
			else
				_model->setState("hilite");
		}
	} else {
		if (_state) {
			if (_down)
				_model->setState("checkeddown");
			else
				_model->setState("checkedup");
		} else {
			if (_down)
				_model->setState("uncheckeddown");
			else
				_model->setState("uncheckedup");
		}
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
	ModelWidget::enter();

	if (isDisabled())
		return;

	_down = false;
	updateModel(true);
}

void WidgetCheckBox::leave() {
	ModelWidget::leave();

	if (isDisabled())
		return;

	_down = false;
	updateModel(false);
}

void WidgetCheckBox::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_down = true;
	updateModel(true);

	playSound("gui_check", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		bool oldState = _state;

		_down  = false;
		_state = true;
		updateModel(true);

		if (oldState != _state)
			setActive(true);

	} else {
		// No group members, we are a check box

		_state = !_state;
		_down  = false;
		updateModel(true);
		setActive(true);
	}

}

void WidgetCheckBox::signalGroupMemberActive() {
	NWNWidget::signalGroupMemberActive();

	_state = false;
	_down  = false;
	updateModel(false);
}

} // End of namespace NWN

} // End of namespace Engines
