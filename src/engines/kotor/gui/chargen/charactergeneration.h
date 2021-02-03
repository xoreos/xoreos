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
 *  The primary character generation menu.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"
#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotORBase {
	class CharacterGenerationInfo;
}

namespace KotOR {

class CharacterGenerationMenu : public KotORBase::GUI {
public:
	CharacterGenerationMenu(KotORBase::Module *module,
	                        CharacterGenerationInfo *pc,
	                        ::Engines::Console *console = 0);

	~CharacterGenerationMenu();

	void showQuickOrCustom();
	void showQuick();
	void showCustom();

	void showPortrait();
	void showName();

	/**
	 * @brief Show the menu for ability selection
	 */
	void showAbilities();

	/**
	 * @brief Show the menu for skill selection
	 */
	void showSkills();

	/**
	 * @brief Show the menu for feat selection
	 */
	void showFeats();

	int getStep();
	void decStep();

	void start();

private:
	KotORBase::Module *_module;
	CharacterGenerationInfo *_pc;
	std::unique_ptr<Graphics::Aurora::Model> _pcModel;

	int _step;

	std::unique_ptr<GUI> _quickOrCustom;
	std::unique_ptr<GUI> _quickChar;
	std::unique_ptr<GUI> _customChar;

	std::unique_ptr<CharacterGenerationBaseMenu> _charGenMenu;

	std::unique_ptr<Graphics::Aurora::SubSceneQuad> _charSubScene;

	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
