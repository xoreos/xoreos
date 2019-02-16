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
 *  The class selection menu.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CLASSSELECTION_H
#define ENGINES_KOTOR_GUI_CHARGEN_CLASSSELECTION_H

#include "glm/mat4x4.hpp"

#include "src/graphics/aurora/subscenequad.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"

#include "src/engines/kotorbase/gui/chargeninfo.h"

#include "src/engines/kotor/module.h"

#include "src/engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

class ClassSelectionMenu : public GUI {
public:
	ClassSelectionMenu(Module *module, ::Engines::Console *console = 0);
	virtual ~ClassSelectionMenu();

private:
	void createCharacterGeneration(CharacterGenerationInfo*);

	void callbackRun();
	void callbackActive(Widget &widget);
	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);

	void setupClassSubScene(const Common::UString &widgetName,
			Graphics::Aurora::SubSceneQuad &subScene, Graphics::Aurora::Model *model,
			glm::mat4 &projection, glm::mat4 &transformation);

	Common::ScopedPtr<GUI> _charGen;

	Graphics::Aurora::SubSceneQuad _maleSoldierSubScene;
	Graphics::Aurora::SubSceneQuad _maleScoutSubScene;
	Graphics::Aurora::SubSceneQuad _maleScoundrelSubScene;
	Graphics::Aurora::SubSceneQuad _femaleSoldierSubScene;
	Graphics::Aurora::SubSceneQuad _femaleScoutSubScene;
	Graphics::Aurora::SubSceneQuad _femaleScoundrelSubScene;

	Odyssey::WidgetLabel *_labelDesc;
	Odyssey::WidgetLabel *_labelTitle;

	Odyssey::WidgetButton *_maleSoldierButton;
	Odyssey::WidgetButton *_maleScoutButton;
	Odyssey::WidgetButton *_maleScoundrelButton;
	Odyssey::WidgetButton *_femaleSoldierButton;
	Odyssey::WidgetButton *_femaleScoutButton;
	Odyssey::WidgetButton *_femaleScoundrelButton;

	Odyssey::WidgetButton *_hoveredButton;

	Module *_module;

	Common::UString _soldierDesc;
	Common::UString _scoundrelDesc;
	Common::UString _scoutDesc;

	Common::UString _soldierMaleTitle;
	Common::UString _soldierFemaleTitle;
	Common::UString _scoundrelMaleTitle;
	Common::UString _scoundrelFemaleTitle;
	Common::UString _scoutMaleTitle;
	Common::UString _scoutFemaleTitle;

	CharacterGenerationInfo *_maleSoldier;
	CharacterGenerationInfo *_maleScout;
	CharacterGenerationInfo *_maleScoundrel;
	CharacterGenerationInfo *_femaleSoldier;
	CharacterGenerationInfo *_femaleScout;
	CharacterGenerationInfo *_femaleScoundrel;
};

} // End of namespace KotOR

} // End of namespace Engines


#endif // ENGINES_KOTOR_GUI_CHARGEN_CLASSSELECTION_H
