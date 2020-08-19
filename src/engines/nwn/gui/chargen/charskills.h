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
 *  The skills selection GUI.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARSKILLS_H
#define ENGINES_NWN_GUI_CHARGEN_CHARSKILLS_H

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class WidgetButton;
class WidgetPanel;
class WidgetEditBox;

class WidgetListItemSkill : public WidgetListItemButton {
public:
	WidgetListItemSkill(::Engines::GUI &gui, const SkillItem &skill);
	~WidgetListItemSkill();

	void setPosition(float x, float y, float z);

	void reset();
	void mouseDown(uint8_t state, float x, float y);

private:
	void subActive(Widget &widget);
	bool changeRank(bool isIncreasing);

	WidgetButton *_upButton;
	WidgetButton *_downButton;
	WidgetPanel  *_skillPointPanel;

	SkillItem _skill;

	friend class CharSkills;
};

class CharSkills : public CharGenBase {
public:
	CharSkills(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharSkills();

	void reset();
	void hide();

	bool changeAvailableSkillRank(int rankChange);
	void setSkillCost(uint8_t cost);
	void setHelpText(const Common::UString &title, const Common::UString &text);

private:
	void callbackActive(Widget &widget);
	void createSkillsList();
	void computeAvailablePoints(uint8_t pointBase);
	void setRecommendedSkills();

	size_t _availableSkillRank;
	WidgetEditBox *_helpTextBox;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARSKILLS_H
