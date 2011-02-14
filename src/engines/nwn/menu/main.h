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

#include "engines/nwn/util.h"

#include "events/types.h"

namespace Engines {

class ModelLoader;

namespace NWN {

class GUI;

/** The NWN main menu. */
class MainMenu {
public:
	MainMenu(const ModelLoader &modelLoader, bool xp1, bool xp2);
	~MainMenu();

	void show();
	void handle();

private:
	GUI *_gui;

	Graphics::Aurora::Model *_xp1;
	Graphics::Aurora::Model *_xp2;

	bool isButton(const Common::UString &tag);

	void mouseMove(int x, int y, uint8 state, Common::UString &cursorTag);

	void mouseMove(Events::Event &event, Common::UString &cursorTag);
	void mouseDown(Events::Event &event, Common::UString &cursorTag);
	void mouseUp  (Events::Event &event, Common::UString &cursorTag);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_MAIN_H
