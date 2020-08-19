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
 *  A NWN slider widget.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/nwn/gui/widgets/slider.h"

namespace Engines {

namespace NWN {

WidgetSlider::WidgetSlider(::Engines::GUI &gui, const Common::UString &tag,
                           const Common::UString &model) :
	ModelWidget(gui, tag, model), _position(0.0f), _steps(0), _state(0) {

	_model->setClickable(true);

	_width = getWidth();

	_thumb = _model->getNode("thumb");

	assert(_thumb);

	changePosition(0.0f);
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

void WidgetSlider::mouseMove(uint8_t state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	changedValue(x, y);
}

void WidgetSlider::mouseDown(uint8_t state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	changedValue(x, y);
}

void WidgetSlider::changedValue(float x, float UNUSED(y)) {
	float curX, curY, curZ;
	getPosition(curX, curY, curZ);

	float pX    = CLIP(x - curX, 0.0f, _width) / _width;
	int   state = roundf(pX * _steps);

	if (state == _state)
		// No change
		return;

	_state = state;

	if (_steps == 0) {
		changePosition(0.0f);
		return;
	}

	changePosition(((float) _state) / _steps);

	setActive(true);
}

void WidgetSlider::changePosition(float value) {
	value = (value * _width) - (_thumb->getWidth() / 2.0f);

	_thumb->move(-_position + value, 0.0f, 0.0f);

	_position = value;
}

} // End of namespace NWN

} // End of namespace Engines
