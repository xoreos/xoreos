/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/main.cpp
 *  The main menu.
 */

#include "common/util.h"

#include "engines/nwn/menu/main.h"
#include "engines/nwn/menu/gui.h"

#include "events/events.h"

#include "graphics/graphics.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

MainMenu::MainMenu(const ModelLoader &modelLoader, bool xp1, bool xp2) : _gui(0), _xp1(0), _xp2(0) {
	_gui = loadGUI(modelLoader, "pre_main");

	if (xp1) {
		_xp1 = modelLoader.loadGUI("ctl_xp1_text");
		_xp1->setPosition(1.24, 0.00, 0.50);
	}

	if (xp2) {
		_xp2 = modelLoader.loadGUI("ctl_xp2_text");
		_xp2->setPosition(1.24, -1.47, 0.50);
	}
}

MainMenu::~MainMenu() {
	delete _gui;

	ModelLoader::free(_xp1);
	ModelLoader::free(_xp2);
}

void MainMenu::show() {
	_gui->show();

	if (_xp1)
		_xp1->show();
	if (_xp2)
		_xp2->show();
}

void MainMenu::handle() {
	Common::UString cursorTag;

	while (!EventMan.quitRequested()) {

		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if      (event.type == Events::kEventMouseMove)
				mouseMove(event, cursorTag);
			else if (event.type == Events::kEventMouseDown)
				mouseDown(event, cursorTag);
			else if (event.type == Events::kEventMouseUp)
				mouseUp(event, cursorTag);
		}

		if (!EventMan.quitRequested())
			EventMan.delay(10);
	}

}

bool MainMenu::isButton(const Common::UString &tag) {
	if (tag == "NewButton")
		return true;
	if (tag == "LoadButton")
		return true;
	if (tag == "MultiButton")
		return true;
	if (tag == "MoviesButton")
		return true;
	if (tag == "OptionsButton")
		return true;
	if (tag == "ExitButton")
		return true;

	return false;
}

void MainMenu::mouseMove(Events::Event &event, Common::UString &cursorTag) {
	if (event.motion.state != 0)
		// Ignore move events when a mouse button is pressed
		return;

	const Common::UString &tag = GfxMan.getObjectAt(event.motion.x, event.motion.y);
	if (tag == cursorTag)
		// Nothing changed, nothing to do
		return;

	// Unhighlight the button we leave
	if (isButton(cursorTag))
		_gui->setWidgetState(cursorTag, "");

	// Highlight the button we leave
	if (isButton(tag))
		_gui->setWidgetState(tag, "hilite");

	cursorTag = tag;
}

void MainMenu::mouseMove(int x, int y, uint8 state, Common::UString &cursorTag) {
	Events::Event event;

	event.motion.state = state;
	event.button.x     = x;
	event.button.y     = y;

	mouseMove(event, cursorTag);
}

void MainMenu::mouseDown(Events::Event &event, Common::UString &cursorTag) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	const Common::UString &tag = GfxMan.getObjectAt(event.button.x, event.button.y);

	if (isButton(tag)) {
		playSound("gui_button", Sound::kSoundTypeSFX);

		_gui->setWidgetState(tag, "down");
	}

	cursorTag = tag;
}

void MainMenu::mouseUp(Events::Event &event, Common::UString &cursorTag) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	const Common::UString &tag = GfxMan.getObjectAt(event.button.x, event.button.y);
	if (cursorTag != tag) {
		// We pressed and released the mouse on differents object => Just signal a move
		mouseMove(event.button.x, event.button.y, 0, cursorTag);
		return;
	}

	if        (tag == "NewButton") {
	} else if (tag == "LoadButton") {
	} else if (tag == "MultiButton") {
	} else if (tag == "MoviesButton") {
	} else if (tag == "OptionsButton") {
	} else if (tag == "ExitButton") {
		EventMan.requestQuit();
		return;
	}

	if (isButton(tag))
		_gui->setWidgetState(tag, "hilite");

	cursorTag = tag;
}

} // End of namespace NWN

} // End of namespace Engines
