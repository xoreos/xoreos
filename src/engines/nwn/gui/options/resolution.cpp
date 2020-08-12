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
 *  The NWN resolution options menu.
 */

#include "src/common/configman.h"
#include "src/common/string.h"
#include "src/common/util.h"

#include "src/graphics/graphics.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/listbox.h"

#include "src/engines/nwn/gui/options/resolution.h"

namespace Engines {

namespace NWN {


OptionsResolutionMenu::OptionsResolutionMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_vidmodes");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}
}

OptionsResolutionMenu::~OptionsResolutionMenu() {
}

void OptionsResolutionMenu::show() {
	initResolutionsBox(*getListBox("VideoModeList", true));

	_width  = WindowMan.getWindowWidth ();
	_height = WindowMan.getWindowHeight();

	GUI::show();
}

void OptionsResolutionMenu::fixWidgetType(const Common::UString &tag, WidgetType &type) {
	if (tag == "VideoModeList")
		type = kWidgetTypeListBox;
}

void OptionsResolutionMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "VideoModeList") {
		dynamic_cast<WidgetListBox &>(widget).setMode(WidgetListBox::kModeSelectable);
		return;
	}
}

void OptionsResolutionMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {

		revertChanges();
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "OkButton") {

		setResolution(getListBox("VideoModeList", true)->getSelected());
		adoptChanges();
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "ApplyButton") {
		setResolution(getListBox("VideoModeList", true)->getSelected());
		return;
	}
}

void OptionsResolutionMenu::initResolutionsBox(WidgetListBox &resList) {
	_useableResolutions.clear();

	int maxWidth  = WindowMan.getSystemWidth ();
	int maxHeight = WindowMan.getSystemHeight();
	int curWidth  = WindowMan.getWindowWidth ();
	int curHeight = WindowMan.getWindowHeight();

	// Find the max allowed resolution in the list
	size_t maxRes = 0;
	for (size_t i = 0; i < ARRAYSIZE(Graphics::kResolutions); i++) {
		if ((Graphics::kResolutions[i].width <= maxWidth) && (Graphics::kResolutions[i].height <= maxHeight)) {
			maxRes = i;
			break;
		}
	}

	// Find the current resolution in the list
	size_t currentResolution = SIZE_MAX;
	for (size_t i = maxRes; i < ARRAYSIZE(Graphics::kResolutions); i++) {
		if ((Graphics::kResolutions[i].width == curWidth) && (Graphics::kResolutions[i].height == curHeight)) {
			currentResolution = i - maxRes;
			break;
		}
	}

	// Doesn't exist, add it at the top
	if (currentResolution == SIZE_MAX) {
		currentResolution = 0;
		Graphics::Resolution curRes;
		curRes.height = curHeight;
		curRes.width = curWidth;
		_useableResolutions.push_back(curRes);
	}

	// Put the rest of the useable resolutions into the list
	for (size_t i = maxRes; i < ARRAYSIZE(Graphics::kResolutions); i++)
		_useableResolutions.push_back(Graphics::kResolutions[i]);


	resList.lock();

	resList.clear();
	for (std::vector<Graphics::Resolution>::const_iterator r = _useableResolutions.begin(); r != _useableResolutions.end(); ++r)
		resList.add(new WidgetListItemTextLine(*this, "fnt_dialog16x16",
					Common::String::format("%dx%d", r->width, r->height), 0.0f));

	resList.unlock();

	resList.select(currentResolution);
}

void OptionsResolutionMenu::setResolution(size_t n) {
	if (n >= _useableResolutions.size())
		return;

	WindowMan.setWindowSize(_useableResolutions[n].width, _useableResolutions[n].height);
}

void OptionsResolutionMenu::adoptChanges() {
	ConfigMan.setInt("width" , WindowMan.getWindowWidth (), true);
	ConfigMan.setInt("height", WindowMan.getWindowHeight(), true);
}

void OptionsResolutionMenu::revertChanges() {
	WindowMan.setWindowSize(_width, _height);
}

} // End of namespace NWN

} // End of namespace Engines
