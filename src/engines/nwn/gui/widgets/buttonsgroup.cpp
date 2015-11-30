/*
 * xoreos - A reimplementation of BioWare's Aurora engine
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
 *  Utility class that handles group of WidgetButton where only
 *  one button can be toggled.
 */

#include "src/common/util.h"

#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/button.h"

#include "src/engines/nwn/gui/widgets/buttonsgroup.h"

namespace Engines {

namespace NWN {

ButtonsGroup::ButtonsGroup(WidgetEditBox *helpBox) {
	_helpBox = helpBox;
}

ButtonsGroup::~ButtonsGroup() {
}

void ButtonsGroup::addButton(WidgetButton *button, const Common::UString &helpTitle, const Common::UString &helpText) {
	button->setMode(WidgetButton::kModeToggle);

	_buttonsList.push_back(button);
	_helpTexts.push_back(helpText);
	_helpTitles.push_back(helpTitle);
}

void ButtonsGroup::removeButton(WidgetButton *button) {
	for (std::vector<WidgetButton *>::iterator it = _buttonsList.begin(); it != _buttonsList.end(); ++it) {
		if (*it == button){
			_buttonsList.erase(it);
			break;
		}
	}
}

void ButtonsGroup::setActive(WidgetButton *button) {
	if (!button)
		return;

	for (std::vector<WidgetButton *>::iterator it = _buttonsList.begin();
	     it != _buttonsList.end(); ++it) {
		// First we check if the button belongs to this group.
		if (*it == button) {
			_helpBox->setText("fnt_galahad14", _helpTexts[std::distance(_buttonsList.begin(), it)], 1.0f);
			_helpBox->setTitle("fnt_galahad14", _helpTitles[std::distance(_buttonsList.begin(), it)]);

			if ((*it)->getMode() == WidgetButton::kModeUnchanged)
				break;

			(*it)->setPressed(true);

			for (std::vector<WidgetButton *>::iterator jt = _buttonsList.begin();
			     jt != _buttonsList.end(); ++jt) {

				if (jt == it)
					continue;

				(*jt)->setPressed(false);
			}

			break;
		}
	}
}

void ButtonsGroup::setActive(size_t button) {
	if (button >= _buttonsList.size())
		error("Wrong ID buttons");


	setActive(_buttonsList[button]);
}

void ButtonsGroup::setAllInactive() {
	for (std::vector<WidgetButton *>::iterator it = _buttonsList.begin(); it != _buttonsList.end(); ++it)
		(*it)->setPressed(false);
}

size_t ButtonsGroup::getChoice() const {
	for (std::vector<WidgetButton *>::const_iterator it = _buttonsList.begin(); it != _buttonsList.end(); ++it) {
		if ((*it)->isPressed()) {
			return std::distance(_buttonsList.begin(), it);
		}
	}

	warning("No button chosen, returning first button");
	return 0;
}

const std::vector< WidgetButton* > &ButtonsGroup::getButtonsList() const {
	return _buttonsList;
}

} // End of namespace NWN

} // End of namespace Engines
