/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/controls.h
 *  The NWN controls options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_CONTROLS_H
#define ENGINES_NWN_GUI_OPTIONS_CONTROLS_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN controls options menu. */
class OptionsControlsMenu: public GUI {
public:
	OptionsControlsMenu(bool isMain = false);
	~OptionsControlsMenu();

protected:
	void callbackActive(Widget &widget);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_OPTIONS_CONTROLS_H
