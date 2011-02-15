/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/menu.cpp
 *  A NWN menu.
 */

#include "graphics/graphics.h"
#include "graphics/aurora/cursorman.h"

#include "events/events.h"

#include "engines/util.h"

#include "engines/nwn/menu/menu.h"

namespace Engines {

namespace NWN {

Menu::Menu(const ModelLoader &modelLoader, const Common::UString &gui) : _currentButton(0) {
	_gui = loadGUI(modelLoader, gui);
}

Menu::~Menu() {
	delete _gui;
}

void Menu::show() {
	_currentButton = 0;

	_gui->show();
}

void Menu::hide() {
	_gui->hide();
}

void Menu::showButtons() {
	for (ButtonArray::iterator button = _buttons.begin(); button != _buttons.end(); ++button)
		if (*button)
			(*button)->show();
}

void Menu::hideButtons() {
	for (ButtonArray::iterator button = _buttons.begin(); button != _buttons.end(); ++button)
		if (*button)
			(*button)->hide();
}

void Menu::addButton(const Common::UString &tag, uint id) {
	if (!id)
		return;

	if (_buttons.size() <= id)
		_buttons.resize(id + 1, 0);

	_buttons[id] = &_gui->getWidget(tag);
	_buttonMap[tag] = id;
}

uint Menu::getButtonAt(int x, int y) const {
	const Common::UString &tag = GfxMan.getObjectAt(x, y);

	ButtonMap::const_iterator button = _buttonMap.find(tag);
	if (button == _buttonMap.end())
		return 0;

	return button->second;
}

bool Menu::isButtonDisabled(uint id) const {
	if (!id)
		return false;

	GUI::Widget *button = _buttons[id];
	assert(button);

	return button->isDisabled();
}

void Menu::setButtonNormal(uint id) {
	if (!id)
		return;

	GUI::Widget *button = _buttons[id];
	assert(button);

	button->setNormal();
}

void Menu::setButtonHighlight(uint id) {
	if (!id)
		return;

	GUI::Widget *button = _buttons[id];
	assert(button);

	button->setHighlight();
}

void Menu::setButtonPressed(uint id) {
	if (!id)
		return;

	GUI::Widget *button = _buttons[id];
	assert(button);

	if (!button->isDisabled()) {
		playSound("gui_button", Sound::kSoundTypeSFX);
		button->setPressed();
	}
}

void Menu::disableButton(uint id) {
	if (!id)
		return;

	GUI::Widget *button = _buttons[id];
	assert(button);

	button->disable();
}

void Menu::enableButton(uint id) {
	if (!id)
		return;

	GUI::Widget *button = _buttons[id];
	assert(button);

	button->enable();
}

void Menu::handle() {
	while (!EventMan.quitRequested()) {

		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if      (event.type == Events::kEventMouseMove)
				mouseMove(event);
			else if (event.type == Events::kEventMouseDown)
				mouseDown(event);
			else if (event.type == Events::kEventMouseUp)
				mouseUp(event);
		}

		if (handleCallBack())
			break;

		if (!EventMan.quitRequested())
			EventMan.delay(10);
	}

	setButtonNormal(_currentButton);
	_currentButton = 0;
}

void Menu::updateMouse() {
	int x, y;
	CursorMan.getPosition(x, y);

	setButtonNormal(_currentButton);

	_currentButton = 0;

	mouseMove(x, y, 0);
}

void Menu::mouseMove(Events::Event &event) {
	if (event.motion.state != 0)
		// Ignore move events when a mouse button is pressed
		return;

	uint button = getButtonAt(event.motion.x, event.motion.y);
	if (button == _currentButton)
		// Nothing changed, nothing to do
		return;

	// Unhighlight the button we leave
	setButtonNormal(_currentButton);
	// Highlight the button we leave
	setButtonHighlight(button);

	_currentButton = button;
}

void Menu::mouseMove(int x, int y, uint8 state) {
	Events::Event event;

	event.motion.state = state;
	event.button.x     = x;
	event.button.y     = y;

	mouseMove(event);
}

void Menu::mouseDown(Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	uint button = getButtonAt(event.motion.x, event.motion.y);

	setButtonPressed(button);

	_currentButton = button;
}

void Menu::mouseUp(Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	uint button = getButtonAt(event.motion.x, event.motion.y);
	if (_currentButton != button) {
		// We pressed and released the mouse on differents object => Just signal a move
		mouseMove(event.button.x, event.button.y, 0);
		return;
	}

	if (isButtonDisabled(button))
		return;

	_currentButton = button;
}

bool Menu::handleCallBack() {
	return false;
}

void Menu::subMenu(Menu &menu) {
	hideButtons();

	menu.show();
	menu.handle();
	menu.hide();

	showButtons();
}

} // End of namespace NWN

} // End of namespace Engines
