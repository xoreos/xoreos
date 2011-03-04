/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/main.h
 *  The main menu.
 */

#ifndef ENGINES_NWN_MENU_MAIN_H
#define ENGINES_NWN_MENU_MAIN_H

#include "engines/nwn/menu/gui.h"

#include "graphics/aurora/types.h"

namespace Engines {

namespace NWN {

struct ModuleContext;

/** The NWN main menu. */
class MainMenu : public GUI {
public:
	MainMenu(ModuleContext &moduleContext);
	~MainMenu();

protected:
	void callbackRun();
	void callbackActive(Widget &widget);

private:
	ModuleContext *_moduleContext;

	bool _hasXP;

	GUI *_new;
	GUI *_movies;
	GUI *_options;

	void callNew(int startCode = 0);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_MAIN_H
