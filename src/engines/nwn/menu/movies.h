/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/movies.h
 *  The movies menu.
 */

#ifndef ENGINES_NWN_MENU_MOVIES_H
#define ENGINES_NWN_MENU_MOVIES_H

#include "events/types.h"

#include "engines/nwn/menu/menu.h"

namespace Engines {

namespace NWN {

class BaseMoviesMenu;

/** The NWN movies menu. */
class MoviesMenu : public Menu {
public:
	MoviesMenu(bool xp1, bool xp2, bool xp3);
	~MoviesMenu();

	void show();

private:
	bool _close;

	BaseMoviesMenu *_baseMovies;

	void mouseUp(Events::Event &event);

	bool handleCallBack();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_MOVIES_H
