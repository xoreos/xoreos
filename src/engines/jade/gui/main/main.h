/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/jade/gui/main/main.h
 *  The Jade Empire main menu.
 */

#ifndef ENGINES_JADE_GUI_MAIN_MAIN_H
#define ENGINES_JADE_GUI_MAIN_MAIN_H

#include "engines/kotor/gui/gui.h"

namespace Engines {

namespace Jade {

class MainMenu : public ::Engines::KotOR::GUI {
public:
	MainMenu();
	~MainMenu();

protected:
	void callbackActive(Widget &widget);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GUI_MAIN_MAIN_H
