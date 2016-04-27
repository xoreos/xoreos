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

WidgetListItemSkill::WidgetListItemSkill(::Engines::GUI &gui,
                                         const Common::UString &text, const Common::UString &helpText,
                                         const Common::UString &icon, size_t skillIndex,
                                         bool isClassSkill, uint8 maxRank) :
                                         WidgetListItemButton(gui, "ctl_cg_btn_skill", text, icon),
                                         _name(text), _helpText(helpText), _skillIndex(skillIndex),
                                         _isClassSkill(isClassSkill), _skillRank(0),
                                         _maxRank(maxRank) {

	// Set maximum width for text.
	_text->set(text, 230.f);

	// Up and down buttons.
	_downButton = new WidgetButton(gui, "", "ctl_cg_btn_skdn");
	_upButton   = new WidgetButton(gui, "", "ctl_cg_btn_skup");

	addChild(*_upButton);
	addChild(*_downButton);

	_skillPointPanel = new WidgetPanel(gui, _tag + "#SkillPointPanel", "ctl_cg_numbox3");
	addChild(*_skillPointPanel);

	_skillPoint = new WidgetLabel(gui, _tag + "#SkillPointLabel", "fnt_galahad14", "0");
	addChild(*_skillPoint);

	// Set min rank as the initial rank.
	_minRank = _skillRank;
}

WidgetListItemSkill::~WidgetListItemSkill() {
	_skillPoint->remove();
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
	_skillPoint->setPosition(pX + x + _skillPointPanel->getWidth() / 2.f,
	                         pY + y + _skillPointPanel->getHeight() / 2.f - 7.f,
	                         z - pZ - 1.f);
}

void WidgetListItemSkill::setTag(const Common::UString &tag) {
	WidgetListItemButton::setTag(tag);

	_downButton->setTag(tag + "#Down");
	_upButton->setTag(tag + "#Up");

	// As the WidgetListBox changes the tag and subwidgets are stored by tags,
	// only add sub after the tags are set.
	addSub(*_upButton);
	addSub(*_downButton);
}

void WidgetListItemSkill::reset() {
	_skillRank = _minRank;
	_skillPoint->setText(Common::composeString<uint8>(_skillRank));
}

void WidgetListItemSkill::mouseDown(uint8 state, float x, float y) {
	WidgetListItem::mouseDown(state, x, y);

	uint8 cost = (_isClassSkill) ? 1 : 2;
	dynamic_cast<CharSkills &>(*_gui).setSkillCost(cost);

}
void WidgetListItemSkill::subActive(Widget &widget) {
	select();

	uint8 cost = (_isClassSkill) ? 1 : 2;
	dynamic_cast<CharSkills &>(*_gui).setSkillCost(cost);
	dynamic_cast<CharSkills &>(*_gui).setHelpText(_name, _helpText);

	if (widget.getTag().endsWith("#Up"))
		changeRank(true);

	if (widget.getTag().endsWith("#Down"))
		changeRank(false);
}

void WidgetListItemSkill::changeRank(bool isIncreasing) {
	uint8 classRankFactor = (_isClassSkill) ? 1 : 2;

	if (isIncreasing) {
		if (_maxRank == _skillRank)
			return;
		if (!dynamic_cast<CharSkills &>(*_gui).changeAvailableSkillRank(-1 * classRankFactor))
			return;

		++_skillRank;
	} else {
		if (_skillRank == _minRank)
			return;
		dynamic_cast<CharSkills &>(*_gui).changeAvailableSkillRank(1 * classRankFactor);
		--_skillRank;
	}

	_skillPoint->setText(Common::composeString<uint8>(_skillRank));
}

CharSkills::CharSkills(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_skills");

	_helpTextBox = getEditBox("HelpBox", true);

	// Adjust panel position.
	WidgetLabel *costBox = getLabel("CostBox", true);
	costBox->movePosition(costBox->getWidth(), 0.f, 0.f);

	createSkillsList();

	getLabel("PtsRemainingBox", true)->setText(Common::composeString<uint8>(_availableSkillRank));

	// Default help texts.
	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(58250));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(58251));
}

CharSkills::~CharSkills() {
}

void CharSkills::reset() {
	WidgetListBox *skillBox = getListBox("SkillsButtonBox", true);
	skillBox->deselect();

	uint8 spentPoint = 0;
	for (std::vector<WidgetListItem *>::iterator it = skillBox->begin();
	     it != skillBox->end(); ++it) {

		WidgetListItemSkill *item = dynamic_cast<WidgetListItemSkill *>(*it);
		spentPoint += (item->_isClassSkill) ? item->_skillRank : item->_skillRank * 2;
		_choices->setSkill(item->_skillIndex, 0);
		item->reset();
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
	getLabel("PtsRemainingBox", true)->setText(Common::composeString<uint8>(_availableSkillRank));
	return true;
}

void CharSkills::setSkillCost(uint8 cost) {
	getLabel("CostBox")->setText(Common::composeString<uint8>(cost));
}

void CharSkills::callbackActive(Widget &widget) {
	if (widget.getTag() == "SkillsButtonBox") {
		WidgetListBox       *skillBox = getListBox("SkillsButtonBox", true);
		WidgetListItemSkill *item     = dynamic_cast<WidgetListItemSkill *>(skillBox->getSelectedItem());
		setHelpText(item->_name, item->_helpText);
	}

	if (widget.getTag() == "OkButton") {
		// Retrieve all skills rank.
		WidgetListBox *skillBox = getListBox("SkillsButtonBox", true);
		for (std::vector<WidgetListItem *>::iterator it = skillBox->begin();
		     it != skillBox->end(); ++it) {
			WidgetListItemSkill *item = dynamic_cast<WidgetListItemSkill *>(*it);
			size_t skillIndex         = item->_skillIndex;
			uint8 skillRank = item->_skillRank;

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
}

void CharSkills::setHelpText(const Common::UString &title, const Common::UString &text) {
	_helpTextBox->setTitle("fnt_galahad14", title);
	_helpTextBox->setText("fnt_galahad14", text);
}

void CharSkills::createSkillsList() {
	const Aurora::TwoDAFile &twodaClasses     = TwoDAReg.get2DA("classes");
	const Aurora::TwoDAFile &twodaSkills      = TwoDAReg.get2DA("skills");
	const Aurora::TwoDARow  &rowClasses       = twodaClasses.getRow(_choices->getClass());
	const Common::UString   skillsClassFile   = rowClasses.getString("SkillsTable");
	const Aurora::TwoDAFile &twoDaSkillsClass = TwoDAReg.get2DA(skillsClassFile);

	computeAvailablePoints(rowClasses.getInt("SkillPointBase"));

	getListBox("SkillsButtonBox")->setMode(WidgetListBox::kModeSelectable);
	getListBox("SkillsButtonBox")->lock();
	for (size_t s = 0; s < twoDaSkillsClass.getRowCount(); ++s) {
		const Aurora::TwoDARow &skillsClassRow = twoDaSkillsClass.getRow(s);
		size_t skillIndex = skillsClassRow.getInt("SkillIndex");
		const Aurora::TwoDARow &skillRow = twodaSkills.getRow(skillIndex);

		Common::UString skillName = TalkMan.getString(skillRow.getInt("Name"));
		Common::UString helpText  = TalkMan.getString(skillRow.getInt("Description"));

		bool classSkill = (bool) skillsClassRow.getInt("ClassSkill");


		uint8 maxRank = 4 + _choices->getCharacter().getHitDice();

		if (classSkill) {
			// Add information about wether it is a skill class.
			skillName += " " + TalkMan.getString(52951);
		} else
			maxRank = (maxRank - maxRank % 2) / 2;

		getListBox("SkillsButtonBox")->add(new WidgetListItemSkill(*this, skillName, helpText,
		                                                           skillRow.getString("Icon"),
		                                                           skillIndex, classSkill, maxRank));
	}
	getListBox("SkillsButtonBox")->unlock();
}

void CharSkills::computeAvailablePoints(uint8 pointBase) {
	uint8 abilityScore = _choices->getAbility(kAbilityIntelligence);
	int8    modifier    = (abilityScore - abilityScore % 2) / 2 - 5;
	_availableSkillRank = modifier + pointBase;

	// Human race can have more skills
	if (_choices->getRace() == 6)
		++_availableSkillRank;
	// More points are available for the first level.
	if (_choices->getCharacter().getHitDice() == 0)
		_availableSkillRank *= 4;
}

}   // End of namespace NWN

} // End of namespace Engines
