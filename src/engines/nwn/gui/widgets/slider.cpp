/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/slider.cpp
 *  A NWN slider widget.
 */

#include "common/util.h"
#include "common/ustring.h"

#include "graphics/aurora/model.h"

#include "engines/nwn/gui/widgets/slider.h"

namespace Engines {

namespace NWN {

WidgetSlider::WidgetSlider(::Engines::GUI &gui, const Common::UString &tag,
                           const Common::UString &model) :
	ModelWidget(gui, tag, model), _position(0.0), _steps(0), _state(0) {

	_width = getWidth();

	changePosition(0.0);
}

WidgetSlider::~WidgetSlider() {
}

void WidgetSlider::setPosition(float x, float y, float z) {
	ModelWidget::setPosition(x, y, z);
}

void WidgetSlider::setSteps(int steps) {
	_steps = steps;
}

int WidgetSlider::getState() const {
	return _state;
}

void WidgetSlider::setState(int state) {
	_state = state;

	changePosition(CLIP(((float) _state) / _steps, 0.0f, 1.0f));
}

void WidgetSlider::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	changedValue(x, y);
}

void WidgetSlider::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	changedValue(x, y);
}

void WidgetSlider::changedValue(float x, float y) {
	float curX, curY, curZ;
	getPosition(curX, curY, curZ);

	float pX    = CLIP(x - curX, 0.0f, _width) / _width;
	int   state = roundf(pX * _steps);

	if (state == _state)
		// No change
		return;

	_state = state;

	if (_steps == 0) {
		changePosition(0.0);
		return;
	}

	changePosition(((float) _state) / _steps);

	setActive(true);
}

void WidgetSlider::changePosition(float value) {
	value = (value * _width) - (_model->getNodeWidth("thumb") / 2.0);

	_model->moveNode("thumb", -_position + value, 0.0, 0.0);

	_position = value;
}

} // End of namespace NWN

} // End of namespace Engines
