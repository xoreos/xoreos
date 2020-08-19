/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The premade character chooser.
 */

#ifndef ENGINES_NWN_GUI_MAIN_CHARPREMADE_H
#define ENGINES_NWN_GUI_MAIN_CHARPREMADE_H

#include <vector>
#include <memory>

#include "src/aurora/types.h"

#include "src/engines/nwn/types.h"

#include "src/engines/nwn/gui/widgets/listitembutton.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;
class Portrait;

class WidgetListItemCharacter : public WidgetListItemBaseButton {
public:
	WidgetListItemCharacter(::Engines::GUI &gui, const Common::UString &font,
	                        const Common::UString &name    , const Common::UString &classes,
	                        const Common::UString &portrait, float spacing = 0.0f);
	~WidgetListItemCharacter();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

private:
	std::unique_ptr<Graphics::Aurora::Text> _textName;
	std::unique_ptr<Graphics::Aurora::Text> _textClass;
	std::unique_ptr<Portrait> _portrait;
};

/** The NWN character creator. */
class CharPremadeMenu : public GUI {
public:
	CharPremadeMenu(Module &module, ::Engines::Console *console = 0);
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

		uint32_t number;

		bool operator<(const Character &c) const;
	};

	Module *_module;

	std::unique_ptr<GUI> _charGen;

	std::vector<Character> _characters;

	const Common::UString &getSelectedCharacter();

	void initCharacterList();
	void playCharacter();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_CHARPREMADE_H
