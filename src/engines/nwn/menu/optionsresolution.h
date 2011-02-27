/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsresolution.h
 *  The resolution options menu.
 */

#ifndef ENGINES_NWN_MENU_OPTIONSRESOLUTION_H
#define ENGINES_NWN_MENU_OPTIONSRESOLUTION_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN resolution options menu. */
class OptionsResolutionMenu: public GUI {
public:
	OptionsResolutionMenu(bool isMain = false);
	~OptionsResolutionMenu();

	void show();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	struct Resolution {
		int width;
		int height;

		Resolution(int w, int h);
	};

	std::vector<Resolution> _resolutions;

	int _width;
	int _height;

	void initResolutions();
	void initResolutionsBox(WidgetEditBox &resList);

	void setResolution(const Common::UString &line);

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_OPTIONSRESOLUTION_H
