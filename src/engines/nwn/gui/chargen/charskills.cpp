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

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/chargen/charskills.h"

namespace Engines {

namespace NWN {

WidgetListItemSkill::WidgetListItemSkill(::Engines::GUI &gui, const SkillItem &skill) :
	WidgetListItemButton(gui, "ctl_cg_btn_skill", skill.name, skill.icon),
	_upButton(0), _downButton(0), _skillPointPanel(0), _skill(skill) {

	setTag("Item#" + skill.name);

	// Set maximum width for text.
	_text->set(skill.name, 230.f);

	// Up and down buttons.
	_upButton = new WidgetButton(gui, _tag + "#Up", "ctl_cg_btn_skup");
	addChild(*_upButton);
	addSub(*_upButton);

	_downButton = new WidgetButton(gui, _tag + "#Down", "ctl_cg_btn_skdn");
	addChild(*_downButton);
	addSub(*_downButton);

	_skillPointPanel = new WidgetPanel(gui, _tag + "#SkillPointPanel", "ctl_cg_numbox3");
	_skillPointPanel->initCaption("fnt_galahad14", "0", 1.0f, 1.0f, 1.0f, 1.0f,
	                              Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignMiddle);
	addChild(*_skillPointPanel);
}

WidgetListItemSkill::~WidgetListItemSkill() {
	_skillPointPanel->remove();
}

void WidgetListItemSkill::setPosition(float x, float y, float z) {
	WidgetListItemButton::setPosition(x, y, z);

	float pX, pY, pZ;
	_button->getNode("skillup")->getPosition(pX, pY, pZ);
	_upButton->setPosition(pX + x, pY + y, z - 5.f);
	_button->getNode("skilldown")->getPosition(pX, pY, pZ);
	_downButton->setPosition(pX + x, pY + y, z - 5.f);

	_button->getNode("skillrank")->getPosition(pX, pY, pZ);
	_skillPointPanel->setPosition(pX + x, pY + y, z - pZ);
}

void WidgetListItemSkill::reset() {
	_skill.rank = _skill.minRank;
	_skillPointPanel->setText(Common::composeString<uint8_t>(_skill.rank));
}

void WidgetListItemSkill::mouseDown(uint8_t state, float x, float y) {
	WidgetListItem::mouseDown(state, x, y);

	uint8_t cost = (_skill.isClassSkill) ? 1 : 2;
	dynamic_cast<CharSkills &>(*_gui).setSkillCost(cost);
}

void WidgetListItemSkill::subActive(Widget &widget) {
	select();

	uint8_t cost = (_skill.isClassSkill) ? 1 : 2;
	dynamic_cast<CharSkills &>(*_gui).setSkillCost(cost);
	dynamic_cast<CharSkills &>(*_gui).setHelpText(_skill.name, _skill.help);

	if (widget.getTag().endsWith("#Up"))
		changeRank(true);

	if (widget.getTag().endsWith("#Down"))
		changeRank(false);
}

bool WidgetListItemSkill::changeRank(bool isIncreasing) {
	uint8_t classRankFactor = (_skill.isClassSkill) ? 1 : 2;

	if (isIncreasing) {
		if (_skill.maxRank == _skill.rank)
			return false;
		if (!dynamic_cast<CharSkills &>(*_gui).changeAvailableSkillRank(-1 * classRankFactor))
			return false;

		++_skill.rank;
	} else {
		if (_skill.rank == _skill.minRank)
			return false;
		dynamic_cast<CharSkills &>(*_gui).changeAvailableSkillRank(1 * classRankFactor);
		--_skill.rank;
	}

	_skillPointPanel->setText(Common::composeString<uint8_t>(_skill.rank));
	return true;
}

CharSkills::CharSkills(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_skills");

	_helpTextBox = getEditBox("HelpBox", true);

	// Adjust panel position.
	WidgetLabel *costBox = getLabel("CostBox", true);
	costBox->movePosition(costBox->getWidth(), 0.f, 0.f);

	createSkillsList();

	getLabel("PtsRemainingBox", true)->setText(Common::composeString<uint8_t>(_availableSkillRank));

	// Default help texts.
	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(58250));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(58251));
}

CharSkills::~CharSkills() {
}

void CharSkills::reset() {
	WidgetListBox *skillBox = getListBox("SkillsButtonBox", true);
	skillBox->deselect();

	uint8_t spentPoint = 0;
	for (std::vector<WidgetListItem *>::iterator it = skillBox->begin();
	     it != skillBox->end(); ++it) {

		WidgetListItemSkill &item = dynamic_cast<WidgetListItemSkill &>(**it);
		spentPoint += (item._skill.isClassSkill) ? item._skill.rank : item._skill.rank * 2;
		_choices->setSkill(item._skill.skillID, 0);
		item.reset();
	}
	changeAvailableSkillRank(spentPoint);

	// Default help texts.
	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(58250));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(58251));

	_choices->setNotUsedSkills(0);
	for (size_t sk = 0; sk < 28; ++sk)
		_choices->setSkill(sk, 0);
}

void CharSkills::hide() {
	CharGenBase::hide();

	if (_returnCode == 1)
		reset();
}

bool CharSkills::changeAvailableSkillRank(int changeRank) {
	if ((int) _availableSkillRank + changeRank < 0)
		return false;

	_availableSkillRank += changeRank;
	getLabel("PtsRemainingBox", true)->setText(Common::composeString<uint8_t>(_availableSkillRank));
	return true;
}

void CharSkills::setSkillCost(uint8_t cost) {
	getLabel("CostBox")->setText(Common::composeString<uint8_t>(cost));
}

void CharSkills::callbackActive(Widget &widget) {
	if (widget.getTag() == "SkillsButtonBox") {
		WidgetListBox       *skillBox = getListBox("SkillsButtonBox", true);
		WidgetListItemSkill *item     = dynamic_cast<WidgetListItemSkill *>(skillBox->getSelectedItem());

		if (item)
			setHelpText(item->_skill.name, item->_skill.help);
	}

	if (widget.getTag() == "OkButton") {
		// Retrieve all skills rank.
		WidgetListBox *skillBox = getListBox("SkillsButtonBox", true);
		for (std::vector<WidgetListItem *>::iterator it = skillBox->begin();
		     it != skillBox->end(); ++it) {
			WidgetListItemSkill &item = dynamic_cast<WidgetListItemSkill &>(**it);
			size_t skillIndex         = item._skill.skillID;
			uint8_t skillRank           = item._skill.rank;

			_choices->setSkill(skillIndex, skillRank);
		}

		_choices->setNotUsedSkills(_availableSkillRank);

		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		GfxMan.lockFrame();
		setRecommendedSkills();
		GfxMan.unlockFrame();
		return;
	}
}

void CharSkills::setHelpText(const Common::UString &title, const Common::UString &text) {
	_helpTextBox->setTitle("fnt_galahad14", title);
	_helpTextBox->setText("fnt_galahad14", text);
}

void CharSkills::createSkillsList() {
	_availableSkillRank = _choices->computeAvailSkillRank();

	std::vector<SkillItem> skills;
	_choices->getSkillItems(skills);

	WidgetListBox *skillListBox = getListBox("SkillsButtonBox");
	skillListBox->setMode(WidgetListBox::kModeSelectable);
	skillListBox->lock();

	for (std::vector<SkillItem>::iterator s = skills.begin(); s != skills.end(); ++s)
		skillListBox->add(new WidgetListItemSkill(*this, *s), true);

	skillListBox->sortByTag();
	skillListBox->unlock();
}

void CharSkills::setRecommendedSkills() {
	reset();

	std::vector<uint8_t> prefSkills;
	_choices->getPrefSkills(prefSkills);

	WidgetListBox *listBox = getListBox("SkillsButtonBox");

	// Fill preferred skills
	for (std::vector<uint8_t>::iterator pS = prefSkills.begin(); pS != prefSkills.end(); ++pS) {
		if (_availableSkillRank == 0)
			break;

		for (std::vector<WidgetListItem *>::iterator i = listBox->begin(); i != listBox->end(); ++i) {
			WidgetListItemSkill &item = dynamic_cast<WidgetListItemSkill &>(**i);

			if (item._skill.skillID != *pS)
				continue;

			// Increase skill rank to the maximum.
			bool skillFilled = false;
			while (!skillFilled) {
				if (!item.changeRank(true))
					skillFilled = true;
			}

			// Continue to the next preferred skill.
			break;
		}
	}
}

}   // End of namespace NWN

} // End of namespace Engines
