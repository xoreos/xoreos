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
#include "src/graphics/resolution.h"

#include "src/engines/odyssey/panel.h"
#include "src/engines/odyssey/listbox.h"

#include "src/engines/kotor/gui/options/resolution.h"

static bool operator<(const Graphics::DisplayMode &d1, const Graphics::DisplayMode &d2) {
	if (d1.w == d2.w)
		return d1.h < d2.h;
	else
		return d1.w < d2.w;
}

namespace Engines {

namespace KotOR {

OptionsResolutionMenu::OptionsResolutionMenu(Console *console) : KotORBase::GUI(console), _newWidth(0), _newHeight(0) {
	load("optresolution");

	Odyssey::WidgetPanel *guiPanel = getPanel("TGuiPanel");
	guiPanel->setPosition(-guiPanel->getWidth()/2, -guiPanel->getHeight()/2, 0);

	int currentIndex = -1;
	std::vector<Graphics::DisplayMode> modes = WindowMan.getDisplayModes();

	// sort and then get the highest resolution.
	std::sort(modes.begin(), modes.end());
	Graphics::DisplayMode maxMode = modes.back();

	// If we have no fullscreen, we add some other common resolutions.
	if (!WindowMan.isFullScreen()) {
		for (size_t i = 0; i < ARRAYSIZE(Graphics::kResolutions); ++i) {
			Graphics::DisplayMode mode;
			mode.w = Graphics::kResolutions[i].width;
			mode.h = Graphics::kResolutions[i].height;
			modes.push_back(mode);
		}

		std::sort(modes.begin(), modes.end());
	}

	// Filter every resolution smaller than 800x600
	for (size_t i = 0; i < modes.size(); ++i) {
		bool duplicate = false;
		for (size_t j = 0; j < _modes.size(); ++j) {
			if (_modes[j].w == modes[i].w && _modes[j].h == modes[i].h)
				duplicate = true;
		}

		if (modes[i].w >= 800 && modes[i].h >= 600 &&
			modes[i].w <= maxMode.w && modes[i].h <= maxMode.h && !duplicate) {
			_modes.push_back(modes[i]);

			if (modes[i].w == WindowMan.getWindowWidth() && modes[i].h == WindowMan.getWindowHeight())
				currentIndex = _modes.size() - 1;
		}
	}

	Odyssey::WidgetListBox *listBox = getListBox("LB_RESOLUTIONS");
	listBox->createItemWidgets(_modes.size());
	for (size_t i = 0; i < _modes.size(); ++i) {
		Graphics::DisplayMode mode = _modes[i];
		listBox->addItem(Common::composeString(mode.w) + " x " + Common::composeString(mode.h));
	}

	listBox->setAdjustHeight(true);
	listBox->setItemSelectionEnabled(true);
	if (currentIndex != -1)
		listBox->selectItemByIndex(currentIndex);
	listBox->refreshItemWidgets();
}

void OptionsResolutionMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_OK") {
		const int index = getListBox("LB_RESOLUTIONS")->getSelectedIndex();

		if (index >= 0 && static_cast<size_t>(index) < _modes.size()) {
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
