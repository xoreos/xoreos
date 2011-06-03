/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/main/charpremade.h
 *  The premade character chooser.
 */

#ifndef ENGINES_NWN_GUI_MAIN_CHARPREMADE_H
#define ENGINES_NWN_GUI_MAIN_CHARPREMADE_H

#include "aurora/types.h"

#include "engines/nwn/types.h"

#include "engines/nwn/gui/widgets/listbox.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;
class Portrait;

class WidgetListItemCharacter : public WidgetListItem {
public:
	WidgetListItemCharacter(::Engines::GUI &gui, const Common::UString &font,
	                        const Common::UString &name    , const Common::UString &classes,
	                        const Common::UString &portrait, float spacing = 0.0);
	~WidgetListItemCharacter();

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
	Graphics::Aurora::Text  *_textName;
	Graphics::Aurora::Text  *_textClass;

	Portrait *_portrait;

	float _spacing;
};

/** The NWN character creator. */
class CharPremadeMenu : public GUI {
public:
	CharPremadeMenu(Module &module);
	~CharPremadeMenu();

	void show();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);

	void callbackActive(Widget &widget);

private:
	struct Character {
		Common::UString file;

		Common::UString name;
		Common::UString classes;
		Common::UString portrait;

		Common::UString displayName;

		uint32 number;

		bool operator<(const Character &c) const;
	};

	Module *_module;

	GUI *_charGen;

	std::vector<Character> _characters;

	const Common::UString &getSelectedCharacter();

	void initCharacterList();
	void playCharacter();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_CHARPREMADE_H
