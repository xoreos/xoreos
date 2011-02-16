/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/basemovies.h
 *  The base game movies menu.
 */

#ifndef ENGINES_NWN_MENU_BASEMOVIES_H
#define ENGINES_NWN_MENU_BASEMOVIES_H

#include "events/types.h"

#include "engines/nwn/menu/menu.h"

namespace Engines {

namespace NWN {

/** The NWN base game movies menu. */
class BaseMoviesMenu : public Menu {
public:
	BaseMoviesMenu();
	~BaseMoviesMenu();

	void show();

private:
	bool _close;

	void mouseUp(Events::Event &event);

	bool handleCallBack();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_BASEMOVIES_H
