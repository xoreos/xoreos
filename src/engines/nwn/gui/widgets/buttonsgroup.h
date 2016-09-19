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

#ifndef ENGINES_NWN_GUI_WIDGETS_BUTTONSGROUP_H
#define ENGINES_NWN_GUI_WIDGETS_BUTTONSGROUP_H

#include <vector>

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class WidgetButton;
class WidgetEditBox;

class ButtonsGroup {
public:
	ButtonsGroup(WidgetEditBox *helpBox);
	~ButtonsGroup();

	void addButton(WidgetButton *button, const Common::UString &helpTitle, const Common::UString &helpText);
	void removeButton(WidgetButton *button);

	void setActive(WidgetButton *button);
	void setActive(size_t button);

	void setAllInactive();

	size_t getChoice() const;
	const std::vector<WidgetButton *> &getButtonsList() const;

private:
	std::vector<WidgetButton *> _buttonsList;

	WidgetEditBox *_helpBox;
	std::vector<Common::UString> _helpTitles;
	std::vector<Common::UString> _helpTexts;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_BUTTONSGROUP_H
