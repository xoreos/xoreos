/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newmodule.h
 *  The new module menu.
 */

#ifndef ENGINES_NWN_MENU_NEWMODULE_H
#define ENGINES_NWN_MENU_NEWMODULE_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

class WidgetListItemModule : public WidgetListItem {
public:
	WidgetListItemModule(::Engines::GUI &gui, const Common::UString &font,
	                     const Common::UString &text, float spacing = 0.0);
	~WidgetListItemModule();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

protected:
	bool activate();
	bool deactivate();

private:
	Graphics::Aurora::Model *_button;
	Graphics::Aurora::Text  *_text;

	float _spacing;
};

/** The NWN new module menu. */
class NewModuleMenu : public GUI {
public:
	NewModuleMenu();
	~NewModuleMenu();

	void show();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);

	void callbackActive(Widget &widget);

private:
	std::vector<Common::UString> _modules;

	Common::UString getSelectedModule();

	void initModuleList();
	void selectedModule();
	void loadModule();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_NEWMODULE_H
