/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsresolution.cpp
 *  The resolution options menu.
 */

#include "common/configman.h"

#include "graphics/graphics.h"

#include "engines/nwn/menu/optionsresolution.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsResolutionMenu::Resolution::Resolution(int w, int h) : width(w), height(h) {
}


OptionsResolutionMenu::OptionsResolutionMenu(bool isMain) {
	load("options_vidmodes");

	if (isMain) {
		WidgetPanel *backdrop = new WidgetPanel("PNL_MAINMENU", "pnl_mainmenu");
		backdrop->setPosition(0.0, 0.0, -10.0);
		addWidget(backdrop);
	}

	initResolutions();
}

OptionsResolutionMenu::~OptionsResolutionMenu() {
}

void OptionsResolutionMenu::show() {
	initResolutionsBox(*getEditBox("VideoModeList", true));

	_width  = GfxMan.getScreenWidth ();
	_height = GfxMan.getScreenHeight();

	GUI::show();
}

void OptionsResolutionMenu::initWidget(Widget &widget) {
	if (widget.getTag() == "VideoModeList") {
		dynamic_cast<WidgetEditBox &>(widget).setMode(WidgetEditBox::kModeSelectable);
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

		setResolution(getEditBox("VideoModeList", true)->getSelectedLine());
		adoptChanges();
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "ApplyButton") {
		setResolution(getEditBox("VideoModeList", true)->getSelectedLine());
		return;
	}
}

void OptionsResolutionMenu::initResolutions() {
	// Add all standard resolutions to the list
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

void OptionsResolutionMenu::initResolutionsBox(WidgetEditBox &resList) {
	resList.clear();

	int maxWidth  = GfxMan.getSystemWidth ();
	int maxHeight = GfxMan.getSystemHeight();
	int curWidth  = GfxMan.getScreenWidth ();
	int curHeight = GfxMan.getScreenHeight();

	std::vector<Resolution>::const_iterator maxRes = _resolutions.end();

	// Find the max allowed resolution in the list
	for (std::vector<Resolution>::const_iterator it = _resolutions.begin(); it != _resolutions.end(); ++it) {
		if ((it->width <= maxWidth) && (it->height <= maxHeight)) {
			maxRes = it;
			break;
		}
	}

	// Find the current resolution in the list
	uint currentResolution = 0xFFFFFFFF;
	if (maxRes != _resolutions.end()) {
		std::vector<Resolution>::const_iterator cur = maxRes;
		for (uint i = 0; cur != _resolutions.end(); i++, ++cur) {
			if ((cur->width == curWidth) && (cur->height == curHeight)) {
				currentResolution = i;
				break;
			}
		}
	}

	// Doesn't exist, add it at the top
	if (currentResolution == 0xFFFFFFFF) {
		currentResolution = 0;
		resList.addLine(Common::UString::sprintf("%dx%d", curWidth, curHeight));
	}

	// Add the standard resolutions to the box
	for (std::vector<Resolution>::const_iterator it = maxRes; it != _resolutions.end(); ++it)
		resList.addLine(Common::UString::sprintf("%dx%d", it->width, it->height));

	resList.selectLine(currentResolution);
}

void OptionsResolutionMenu::setResolution(const Common::UString &line) {
	int width, height;
	if (sscanf(line.c_str(), "%dx%d", &width, &height) != 2)
		return;

	GfxMan.setScreenSize(width, height);
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
