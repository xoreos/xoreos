/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/main.h
 *  The main menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_MAIN_H
#define ENGINES_NWN_GUI_MAIN_MAIN_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN main menu. */
class MainMenu : public GUI {
public:
	MainMenu(Module &module);
	~MainMenu();

	void show();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;

	bool _hasXP;

	GUI *_charType;

	GUI *_new;
	GUI *_movies;
	GUI *_options;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_MAIN_H
