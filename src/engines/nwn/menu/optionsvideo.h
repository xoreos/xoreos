/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionsvideo.h
 *  The video options menu.
 */

#ifndef ENGINES_NWN_MENU_OPTIONSVIDEO_H
#define ENGINES_NWN_MENU_OPTIONSVIDEO_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN video options menu. */
class OptionsVideoMenu: public GUI {
public:
	OptionsVideoMenu(bool isMain = false);
	~OptionsVideoMenu();

	void show();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	GUI *_resolution;
	GUI *_advanced;

	float _gamma;

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_OPTIONSVIDEO_H
