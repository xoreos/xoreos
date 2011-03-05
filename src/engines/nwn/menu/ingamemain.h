/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/ingamemain.h
 *  The ingame main menu.
 */

#ifndef ENGINES_NWN_MENU_INGAMEMAIN_H
#define ENGINES_NWN_MENU_INGAMEMAIN_H

#include "engines/nwn/menu/gui.h"

#include "graphics/aurora/types.h"

namespace Engines {

namespace NWN {

/** The NWN ingame main menu. */
class InGameMainMenu : public GUI {
public:
	InGameMainMenu();
	~InGameMainMenu();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	GUI *_game;
	GUI *_video;
	GUI *_sound;
	GUI *_controls;

	GUI *_quitPrompt;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_INGAMEMAIN_H
