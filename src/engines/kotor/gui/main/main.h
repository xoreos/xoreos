/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/main/main.h
 *  The KotOR main menu.
 */

#ifndef ENGINES_KOTOR_GUI_MAIN_MAIN_H
#define ENGINES_KOTOR_GUI_MAIN_MAIN_H

#include "engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

class MainMenu : public GUI {
public:
	MainMenu();
	~MainMenu();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_MAIN_MAIN_H
