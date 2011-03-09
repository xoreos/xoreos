/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/chartype.h
 *  The new/premade character selection menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_CHARTYPE_H
#define ENGINES_NWN_GUI_MAIN_CHARTYPE_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new/premade character selection menu. */
class CharTypeMenu : public GUI {
public:
	CharTypeMenu(Module &module);
	~CharTypeMenu();

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;

	GUI *_charGen;
	GUI *_charPremade;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_CHARTYPE_H
