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
 *  The KotOR 2 class selection.
 */

#ifndef ENGINES_KOTOR2_GUI_CHARGEN_CLASSSELECTION_H
#define ENGINES_KOTOR2_GUI_CHARGEN_CLASSSELECTION_H

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/button.h"

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor2/gui/chargen/charactergeneration.h"
#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

class ClassSelection : public KotORBase::GUI {
public:
	ClassSelection(KotORBase::Module *module, Engines::Console *console = 0);
	~ClassSelection();

protected:
	void callbackRun();
	void callbackActive(Widget &widget);

private:
	KotORBase::Module *_module;

	Common::ScopedPtr<CharacterGeneration> _chargen;

	CharacterGenerationInfo *_consularMaleInfo;
	CharacterGenerationInfo *_sentinelMaleInfo;
	CharacterGenerationInfo *_guardianMaleInfo;
	CharacterGenerationInfo *_consularFemaleInfo;
	CharacterGenerationInfo *_sentinelFemaleInfo;
	CharacterGenerationInfo *_guardianFemaleInfo;

	Odyssey::WidgetButton *_consularMaleButton;
	Odyssey::WidgetButton *_sentinelMaleButton;
	Odyssey::WidgetButton *_guardianMaleButton;
	Odyssey::WidgetButton *_guardianFemaleButton;
	Odyssey::WidgetButton *_sentinelFemaleButton;
	Odyssey::WidgetButton *_consularFemaleButton;
	Odyssey::WidgetButton *_hoveredButton;

	Odyssey::WidgetLabel *_labelDesc;
	Odyssey::WidgetLabel *_labelTitle;

	Common::UString _guardianMaleTitle;
	Common::UString _guardianFemaleTitle;
	Common::UString _consularMaleTitle;
	Common::UString _consularFemaleTitle;
	Common::UString _sentinelMaleTitle;
	Common::UString _sentinelFemaleTitle;

	Common::UString _guardianDescription;
	Common::UString _consularDescription;
	Common::UString _sentinelDescription;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_GUI_CHARGEN_CLASSSELECTION_H
