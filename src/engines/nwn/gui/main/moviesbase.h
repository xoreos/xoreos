/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/moviesbase.h
 *  The base game movies menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_MOVIESBASE_H
#define ENGINES_NWN_GUI_MAIN_MOVIESBASE_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN base game movies menu. */
class MoviesBaseMenu: public GUI {
public:
	MoviesBaseMenu();
	~MoviesBaseMenu();

protected:
	void callbackActive(Widget &widget);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_MOVIESBASE_H
