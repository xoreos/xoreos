/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newxp2.h
 *  The new game menu, expansion 2.
 */

#ifndef ENGINES_NWN_MENU_NEWXP2_H
#define ENGINES_NWN_MENU_NEWXP2_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

class Module;

/** The NWN new game menu, expansion 2. */
class NewXP2Menu : public GUI {
public:
	NewXP2Menu(Module &module, GUI &charType);
	~NewXP2Menu();

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

#endif // ENGINES_NWN_MENU_NEWXP2_H
