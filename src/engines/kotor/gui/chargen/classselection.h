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

#include "src/engines/kotor/module.h"

#include "src/engines/kotor/gui/gui.h"

#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/label.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

class ClassSelectionMenu : public GUI {
public:
	ClassSelectionMenu(Module *module, ::Engines::Console *console = 0);

private:
	void callbackRun();
	void callbackActive(Widget &widget);

	WidgetLabel *_labelDesc;
	WidgetLabel *_labelTitle;

	WidgetButton *_maleSoldierButton;
	WidgetButton *_maleScoutButton;
	WidgetButton *_maleScoundrelButton;
	WidgetButton *_femaleSoldierButton;
	WidgetButton *_femaleScoutButton;
	WidgetButton *_femaleScoundrelButton;

	WidgetButton *_hoveredButton;

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
};

} // End of namespace KotOR

} // End of namespace Engines


#endif // ENGINES_KOTOR_GUI_CHARGEN_CLASSSELECTION_H
