/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/resolution.cpp
 *  The NWN resolution options menu.
 */

#include "common/configman.h"

#include "graphics/graphics.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/listbox.h"

#include "engines/nwn/gui/options/resolution.h"

namespace Engines {

namespace NWN {

OptionsResolutionMenu::Resolution::Resolution(int w, int h) : width(w), height(h) {
}


OptionsResolutionMenu::OptionsResolutionMenu(bool isMain) {
	load("options_vidmodes");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel(*this, "PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
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
	uint maxRes = 0;
	for (uint i = 0; i < _resolutions.size(); i++) {
		if ((_resolutions[i].width <= maxWidth) && (_resolutions[i].height <= maxHeight)) {
			maxRes = i;
			break;
		}
	}

	// Find the current resolution in the list
	uint currentResolution = 0xFFFFFFFF;
	for (uint i = maxRes; i < _resolutions.size(); i++) {
		if ((_resolutions[i].width == curWidth) && (_resolutions[i].height == curHeight)) {
			currentResolution = i - maxRes;
			break;
		}
	}

	// Doesn't exist, add it at the top
	if (currentResolution == 0xFFFFFFFF) {
		currentResolution = 0;
		_useableResolutions.push_back(Resolution(curWidth, curHeight));
	}

	// Put the rest of the useable resolutions into the list
	for (uint i = maxRes; i < _resolutions.size(); i++)
		_useableResolutions.push_back(_resolutions[i]);


	resList.lock();

	resList.clear();
	for (std::vector<Resolution>::const_iterator r = _useableResolutions.begin(); r != _useableResolutions.end(); ++r)
		resList.add(new WidgetListItemTextLine(*this, "fnt_dialog16x16",
					Common::UString::sprintf("%dx%d", r->width, r->height), 0.0));

	resList.unlock();

	resList.select(currentResolution);
}

void OptionsResolutionMenu::setResolution(uint n) {
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
