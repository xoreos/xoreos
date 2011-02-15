/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/main.h
 *  The main menu.
 */

#ifndef ENGINES_NWN_MENU_MAIN_H
#define ENGINES_NWN_MENU_MAIN_H

#include "events/types.h"

#include "engines/nwn/menu/menu.h"

namespace Engines {

namespace NWN {

class MoviesMenu;

/** The NWN main menu. */
class MainMenu : public Menu {
public:
	MainMenu(const ModelLoader &modelLoader, bool xp1, bool xp2, bool xp3);
	~MainMenu();

	void show();
	void hide();

private:
	Graphics::Aurora::Model *_xp1;
	Graphics::Aurora::Model *_xp2;

	MoviesMenu *_movies;

	void showModels();
	void hideModels();

	void mouseUp(Events::Event &event);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_MAIN_H
