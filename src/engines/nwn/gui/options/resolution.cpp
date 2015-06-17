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

#include "src/graphics/graphics.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/listbox.h"

#include "src/engines/nwn/gui/options/resolution.h"

namespace Engines {

namespace NWN {

OptionsResolutionMenu::Resolution::Resolution(int w, int h) : width(w), height(h) {
}


OptionsResolutionMenu::OptionsResolutionMenu(bool isMain, ::Engines::Console *console) : GUI(console) {
	load("options_vidmodes");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0f, 0.0f, 100.0f);
		addWidget(backdrop);
	}

	initResolutions();
}

OptionsResolutionMenu::~OptionsResolutionMenu() {
}

void OptionsResolutionMenu::show() {
	initResolutionsBox(*getListBox("VideoModeList", true));

	_width  = GfxMan.getScreenWidth ();
	_height = GfxMan.getScreenHeight();

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

void OptionsResolutionMenu::initResolutions() {
	// Add all standard resolutions to the list
	_resolutions.reserve(33);
	_resolutions.push_back(Resolution(7680, 4800));
	_resolutions.push_back(Resolution(7680, 4320));
	_resolutions.push_back(Resolution(6400, 4800));
	_resolutions.push_back(Resolution(6400, 4096));
	_resolutions.push_back(Resolution(5120, 4096));
	_resolutions.push_back(Resolution(5120, 3200));
	_resolutions.push_back(Resolution(4096, 3072));
	_resolutions.push_back(Resolution(4096, 1716));
	_resolutions.push_back(Resolution(3840, 2400));
	_resolutions.push_back(Resolution(3200, 2400));
	_resolutions.push_back(Resolution(3200, 2048));
	_resolutions.push_back(Resolution(2560, 2048));
	_resolutions.push_back(Resolution(2560, 1600));
	_resolutions.push_back(Resolution(2560, 1440));
	_resolutions.push_back(Resolution(2048, 1536));
	_resolutions.push_back(Resolution(2048, 1152));
	_resolutions.push_back(Resolution(2048, 1080));
	_resolutions.push_back(Resolution(1920, 1200));
	_resolutions.push_back(Resolution(1920, 1080));
	_resolutions.push_back(Resolution(1680, 1050));
	_resolutions.push_back(Resolution(1600, 1200));
	_resolutions.push_back(Resolution(1600,  900));
	_resolutions.push_back(Resolution(1440,  900));
	_resolutions.push_back(Resolution(1400, 1050));
	_resolutions.push_back(Resolution(1280, 1024));
	_resolutions.push_back(Resolution(1280,  800));
	_resolutions.push_back(Resolution(1280,  720));
	_resolutions.push_back(Resolution(1152,  864));
	_resolutions.push_back(Resolution(1024,  768));
	_resolutions.push_back(Resolution(800 ,  600));
	_resolutions.push_back(Resolution(640 ,  480));
	_resolutions.push_back(Resolution(320 ,  240));
	_resolutions.push_back(Resolution(320 ,  200));
}

void OptionsResolutionMenu::initResolutionsBox(WidgetListBox &resList) {
	_useableResolutions.clear();

	int maxWidth  = GfxMan.getSystemWidth ();
	int maxHeight = GfxMan.getSystemHeight();
	int curWidth  = GfxMan.getScreenWidth ();
	int curHeight = GfxMan.getScreenHeight();

	// Find the max allowed resolution in the list
	size_t maxRes = 0;
	for (size_t i = 0; i < _resolutions.size(); i++) {
		if ((_resolutions[i].width <= maxWidth) && (_resolutions[i].height <= maxHeight)) {
			maxRes = i;
			break;
		}
	}

	// Find the current resolution in the list
	size_t currentResolution = SIZE_MAX;
	for (size_t i = maxRes; i < _resolutions.size(); i++) {
		if ((_resolutions[i].width == curWidth) && (_resolutions[i].height == curHeight)) {
			currentResolution = i - maxRes;
			break;
		}
	}

	// Doesn't exist, add it at the top
	if (currentResolution == SIZE_MAX) {
		currentResolution = 0;
		_useableResolutions.push_back(Resolution(curWidth, curHeight));
	}

	// Put the rest of the useable resolutions into the list
	for (size_t i = maxRes; i < _resolutions.size(); i++)
		_useableResolutions.push_back(_resolutions[i]);


	resList.lock();

	resList.clear();
	for (std::vector<Resolution>::const_iterator r = _useableResolutions.begin(); r != _useableResolutions.end(); ++r)
		resList.add(new WidgetListItemTextLine(*this, "fnt_dialog16x16",
					Common::UString::format("%dx%d", r->width, r->height), 0.0f));

	resList.unlock();

	resList.select(currentResolution);
}

void OptionsResolutionMenu::setResolution(size_t n) {
	if (n >= _useableResolutions.size())
		return;

	GfxMan.setScreenSize(_useableResolutions[n].width, _useableResolutions[n].height);
}

void OptionsResolutionMenu::adoptChanges() {
	ConfigMan.setInt("width" , GfxMan.getScreenWidth (), true);
	ConfigMan.setInt("height", GfxMan.getScreenHeight(), true);
}

void OptionsResolutionMenu::revertChanges() {
	GfxMan.setScreenSize(_width, _height);
}

} // End of namespace NWN

} // End of namespace Engines
