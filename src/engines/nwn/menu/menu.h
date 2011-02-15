/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/menu.h
 *  A NWN menu.
 */

#ifndef ENGINES_NWN_MENU_MENU_H
#define ENGINES_NWN_MENU_MENU_H

#include <vector>
#include <map>

#include "common/ustring.h"

#include "events/types.h"

#include "engines/nwn/menu/gui.h"

namespace Engines {

class ModelLoader;

namespace NWN {

/** A menu in NWN. */
class Menu {
public:
	Menu(const ModelLoader &modelLoader, const Common::UString &gui);
	~Menu();

	virtual void show();
	virtual void hide();

	virtual void showButtons();
	virtual void hideButtons();

	virtual void handle();

protected:
	uint _currentButton;

	void addButton(const Common::UString &tag, uint id);

	uint getButtonAt(int x, int y) const;

	bool isButtonDisabled(uint id) const;

	void setButtonNormal(uint id);
	void setButtonHighlight(uint id);
	void setButtonPressed(uint id);

	void disableButton(uint id);
	void enableButton(uint id);

	virtual void updateMouse();

	virtual void mouseMove(int x, int y, uint8 state);

	virtual void mouseMove(Events::Event &event);
	virtual void mouseDown(Events::Event &event);
	virtual void mouseUp  (Events::Event &event);

	virtual bool handleCallBack();

	virtual void subMenu(Menu &menu);

private:
	typedef std::map<Common::UString, uint> ButtonMap;
	typedef std::vector<GUI::Widget *> ButtonArray;

	GUI *_gui;

	ButtonArray _buttons;
	ButtonMap _buttonMap;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_MENU_H
