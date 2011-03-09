/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/moviescamp.h
 *  The campaign movies menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_MOVIESCAMP_H
#define ENGINES_NWN_GUI_MAIN_MOVIESCAMP_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN campaign movies menu. */
class MoviesCampMenu: public GUI {
public:
	MoviesCampMenu();
	~MoviesCampMenu();

protected:
	void callbackActive(Widget &widget);

private:
	GUI *_base;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_MOVIESCAMP_H
