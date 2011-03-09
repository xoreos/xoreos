/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/newcamp.h
 *  The new campaign menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_NEWCAMP_H
#define ENGINES_NWN_GUI_MAIN_NEWCAMP_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new campaign menu. */
class NewCampMenu : public GUI {
public:
	NewCampMenu(Module &module, GUI &charType);
	~NewCampMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;

	GUI *_charType;

	GUI *_base;
	GUI *_xp1;
	GUI *_xp2;
	GUI *_modules;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_NEWCAMP_H
