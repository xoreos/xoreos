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
 *  A NWN checkbox widget.
 */

#include "src/common/system.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/checkbox.h"

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
		node->move(0.0f, 0.0f, -10.0f);
	_model->setState("checkeddown");
	if ((node = _model->getNode("XPlane")))
		node->move(0.0f, 0.0f, -10.0f);
	_model->setState("checkedhilite");
	if ((node = _model->getNode("XPlane")))
		node->move(0.0f, 0.0f, -10.0f);

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

void WidgetCheckBox::mouseDown(uint8_t state, float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_down = true;
	updateModel(true);

	playSound("gui_check", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
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
