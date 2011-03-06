/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newxp1.h
 *  The new game menu, expansion 1.
 */

#ifndef ENGINES_NWN_MENU_NEWXP1_H
#define ENGINES_NWN_MENU_NEWXP1_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new game menu, expansion 1. */
class NewXP1Menu : public GUI {
public:
	NewXP1Menu(Module &module, GUI &charType);
	~NewXP1Menu();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	Module *_module;

	GUI *_charType;

	void loadModule(const Common::UString &module);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_NEWXP1_H
