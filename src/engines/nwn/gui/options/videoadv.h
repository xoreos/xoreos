/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/videoadv.h
 *  The NWN advanced video options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_VIDEOADV_H
#define ENGINES_NWN_GUI_OPTIONS_VIDEOADV_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN advanced video options menu. */
class OptionsVideoAdvancedMenu: public GUI {
public:
	OptionsVideoAdvancedMenu(bool isMain = false);
	~OptionsVideoAdvancedMenu();

	void show();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);

	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	int _oldFSAA;

	void updateFSAALabel(int n);

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_OPTIONS_VIDEOADV_H
