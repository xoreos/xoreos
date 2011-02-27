/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newmodule.h
 *  The new module menu.
 */

#ifndef ENGINES_NWN_MENU_NEWMODULE_H
#define ENGINES_NWN_MENU_NEWMODULE_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN new module menu. */
class NewModuleMenu : public GUI {
public:
	NewModuleMenu();
	~NewModuleMenu();

	void show();

protected:
	void callbackActive(Widget &widget);

private:
	std::vector<Common::UString> _modules;

	Common::UString getSelectedModule();

	void initModuleList();
	void selectedModule();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_NEWMODULE_H
