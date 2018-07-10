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
 *  The graphics resolution menu.
 */

#include "src/common/configman.h"
#include "src/common/strutil.h"
#include "src/common/ustring.h"

#include "src/graphics/windowman.h"

#include "src/engines/kotor/gui/options/resolution.h"

#include "src/engines/aurora/kotorjadegui/panel.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"

namespace Engines {

namespace KotOR {

OptionsResolutionMenu::OptionsResolutionMenu(Console *console) : GUI(console), _newWidth(0), _newHeight(0) {
	load("optresolution");

	WidgetPanel *guiPanel = getPanel("TGuiPanel");
	guiPanel->setPosition(-guiPanel->getWidth()/2, -guiPanel->getHeight()/2, 0);

	int currentIndex = -1;
	std::vector<Graphics::DisplayMode> modes = WindowMan.getDisplayModes();

	// Filter every resolution smaller than 800x600
	for (size_t i = 0; i < modes.size(); ++i) {
		bool duplicate = false;
		for (size_t j = 0; j < _modes.size(); ++j) {
			if (_modes[j].w == modes[i].w && _modes[j].h == modes[i].h)
				duplicate = true;
		}

		if (modes[i].w >= 800 && modes[i].h >= 600 && !duplicate) {
			_modes.push_back(modes[i]);
		}
	}

	WidgetListBox *listBox = getListBox("LB_RESOLUTIONS");
	listBox->createItemWidgets(_modes.size());
	for (size_t i = 0; i < _modes.size(); ++i) {
		Graphics::DisplayMode mode = _modes[i];
		listBox->addItem(Common::composeString(mode.w) + " x " + Common::composeString(mode.h));
	}

	listBox->setAdjustHeight(true);
	listBox->setItemSelectionEnabled(true);
	listBox->refreshItemWidgets();
}

void OptionsResolutionMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_OK") {
		int index = getListBox("LB_RESOLUTIONS")->getSelectedIndex();
		if (index < static_cast<int>(_modes.size()) && index != -1) {
			WindowMan.setWindowSize(_modes[index].w, _modes[index].h);
			ConfigMan.setInt("width", _modes[index].w);
			ConfigMan.setInt("height", _modes[index].h);
		}

		_returnCode = kReturnCodeAbort;
		return;
	}

	if (widget.getTag() == "BTN_CANCEL") {
		_returnCode = kReturnCodeAbort;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
