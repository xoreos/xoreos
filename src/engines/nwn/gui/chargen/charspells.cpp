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
 *  The spells selection GUI in CharGen.
 */

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/modelnode.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/chargen/charhelp.h"
#include "src/engines/nwn/gui/chargen/charspells.h"

namespace Engines {

namespace NWN {

WidgetListItemSpell::WidgetListItemSpell(::Engines::GUI &gui, Spell spell, bool isRight) :
    WidgetListItemButton(gui, "ctl_cg_btn_feat", spell.name, spell.icon, kMoveButton | kHelpButton) {
	// Set maximum width for text.
	_text->set(spell.name, 230.f);

	if (!isRight)
		changeArrowDirection();

	setTag("Item#" + spell.name);

	_spell = spell;
}

WidgetListItemSpell::~WidgetListItemSpell() {
}

void WidgetListItemSpell::subActive(Widget &widget) {
	if ((widget.getTag().endsWith("#MoveButtonLeft") || widget.getTag().endsWith("#MoveButtonRight")))
		dynamic_cast<CharSpells &>(*_gui).moveSpell(this);
}

void WidgetListItemSpell::callbackHelp() {
	dynamic_cast<CharSpells &>(*_gui).showSpellHelp(_spell);
}

CharSpells::CharSpells(CharGenChoices &choices, Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_spells");

	_currentSpellLevel = SIZE_MAX;
	_abilityLimit = 0;

	_spellHelp = std::make_unique<CharHelp>("cg_spellinfo", console);

	_availListBox = getListBox("AvailBox", true);
	_availListBox->setMode(WidgetListBox::kModeSelectable);
	_knownListBox = getListBox("KnownBox", true);
	_knownListBox->setMode(WidgetListBox::kModeSelectable);

	for (uint32_t lvl = 0; lvl < 10; ++lvl) {
		WidgetButton *spellLvlButton = getButton("SpellLevel" + Common::composeString<uint32_t>(lvl), true);
		spellLvlButton->setTooltip(TalkMan.getString(68674 + lvl));
		spellLvlButton->setTooltipPosition(20.f, -40.f, -100.f);

		// TODO: The button should be render properly when pressed.
		spellLvlButton->setMode(WidgetButton::kModeUnchanged);
		spellLvlButton->setPressed(false);
	}

	makeSpellsList();
}

CharSpells::~CharSpells() {
	_availListBox->lock();
	_availListBox->clear();
	_availListBox->unlock();
	_knownListBox->lock();
	_knownListBox->clear();
	_knownListBox->unlock();
}

void CharSpells::reset() {
	makeSpellsList();
}

void CharSpells::fixWidgetType(const Common::UString &tag, GUI::WidgetType &type) {
	if (tag.beginsWith("SpellLevel"))
		type = kWidgetTypeButton;
}

void CharSpells::showSpellLevel(size_t spellLevel, bool forceUpdate) {
	if (_currentSpellLevel == spellLevel && !forceUpdate)
		return;

	// Show/hide label about empty spells list.
	Widget *label = 0;
	label = _availListBox->getChild("EmptySpellListLabel");
	if (_availSpells[spellLevel].empty() && label) {
		label->setInvisible(false);
		if (_availListBox->isVisible())
			label->show();
	} else if (label) {
		label->setInvisible(true);
		if (_availListBox->isVisible())
			label->hide();
	}

	// Show/Hide label about ability requirement.
	label = 0;
	label = _knownListBox->getChild("CannotLearn");
	if (spellLevel <= _abilityLimit && label) {
		label->setInvisible(true);
		if (_knownListBox->isVisible())
			label->hide();
	} else if (label) {
		label->setInvisible(false);
		if (_knownListBox->isVisible())
			label->show();
	}

	// Fill available spells.
	_availListBox->lock();
	_availListBox->clear();
	for (std::vector<Spell>::iterator sp = _availSpells[spellLevel].begin(); sp != _availSpells[spellLevel].end(); ++sp) {
		_availListBox->add(new WidgetListItemSpell(*this, *sp), true);
	}
	_availListBox->unlock();

	// Fill known spells.
	_knownListBox->lock();
	_knownListBox->clear();
	for (std::vector<Spell>::iterator sp = _knownSpells[spellLevel].begin(); sp != _knownSpells[spellLevel].end(); ++sp) {
		_knownListBox->add(new WidgetListItemSpell(*this, *sp, false), true);
	}
	_knownListBox->unlock();

	_currentSpellLevel = spellLevel;
	updateRemainLabel();
}

void CharSpells::moveSpell(WidgetListItemSpell *spellItem) {
	WidgetListBox *fromListBox = dynamic_cast<WidgetListBox *>(spellItem->_owner);
	if (!fromListBox)
		return;

	WidgetListBox *toListBox = 0;

	std::vector<Spell> *fromSpellList;
	std::vector<Spell> *toSpellList;

	if (fromListBox == _knownListBox) {
		toListBox     = _availListBox;
		toSpellList   = &_availSpells[_currentSpellLevel];
		fromSpellList = &_knownSpells[_currentSpellLevel];

		++_remainingSpells[_currentSpellLevel];

	} else {
		if (_remainingSpells[_currentSpellLevel] == 0)
			return;

		toListBox     = _knownListBox;
		toSpellList   = &_knownSpells[_currentSpellLevel];
		fromSpellList = &_availSpells[_currentSpellLevel];

		--_remainingSpells[_currentSpellLevel];
	}

	fromListBox->lock();
	fromListBox->remove(spellItem);
	fromListBox->sortByTag();
	fromListBox->unlock();

	spellItem->changeArrowDirection();

	toListBox->lock();
	toListBox->add(spellItem, true);
	toListBox->sortByTag();
	toListBox->unlock();

	for (std::vector<Spell>::iterator it = fromSpellList->begin(); it != fromSpellList->end(); ++it) {
		if ((*it).spellID == spellItem->_spell.spellID) {
			toSpellList->push_back(*it);
			fromSpellList->erase(it);
			break;
		}
	}

	updateRemainLabel();
}

void CharSpells::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		for (size_t lvl = 0; lvl < _knownSpells.size(); ++lvl) {
			for (std::vector<Spell>::iterator sp = _knownSpells[lvl].begin(); sp != _knownSpells[lvl].end(); ++sp) {
				_choices->setSpell(lvl, (*sp).spellID);
			}
		}
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		GfxMan.lockFrame();
		setRecommendedSpells();
		GfxMan.unlockFrame();
		return;
	}

	if (widget.getTag() == "ResetButton") {
		GfxMan.lockFrame();
		reset();
		showSpellLevel(_currentSpellLevel, true);
		GfxMan.unlockFrame();
		return;
	}

	for (uint32_t lvl = 0; lvl <= _maxLevel; ++lvl) {
		if (widget.getTag() == "SpellLevel" + Common::composeString<uint32_t>(lvl)) {
			showSpellLevel(lvl);
			return;
		}
	}
}

void CharSpells::makeSpellsList() {
	// Compute the maximum spell level.
	const Aurora::TwoDAFile &twodaClasses = TwoDAReg.get2DA("classes");
	const Aurora::TwoDARow &classRow = twodaClasses.getRow(_choices->getClass());
	const Common::UString gainTable = classRow.getString("SpellGainTable");
	const Aurora::TwoDAFile &twodaSpellGain = TwoDAReg.get2DA(gainTable);
	const Aurora::TwoDARow &spellLevelRow = twodaSpellGain.getRow(_choices->getCharacter().getHitDice());

	for (size_t lvl = 2; lvl < twodaSpellGain.getColumnCount(); ++lvl) {
		if (spellLevelRow.empty(lvl)) {
			_maxLevel = lvl - 3UL;
			break;
		}
	}

	_knownSpells.clear();
	_knownSpells.resize(_maxLevel + 1);
	_availSpells.clear();
	_availSpells.resize(_maxLevel + 1);
	_remainingSpells.clear();
	_remainingSpells.resize(_maxLevel + 1);

	computeRemainingSpells(classRow);

	// We need to know the spellCaster type. We can get it from the spellGain table.
	Common::UString casterName;
	if (gainTable == "CLS_SPGN_BARD") {
		casterName = "Bard";
	} else if (gainTable == "CLS_SPGN_CLER") {
		casterName = "Cleric";
	} else if (gainTable == "CLS_SPGN_DRUI") {
		casterName = "Druid";
	} else if (gainTable == "CLS_SPGN_PAL") {
		casterName = "Paladin";
	} else if (gainTable == "CLS_SPGN_RANG") {
		casterName = "Ranger";
	} else if (gainTable == "CLS_SPGN_WIZ" || gainTable == "CLS_SPGN_SORC") {
		casterName = "Wiz_Sorc";
	} else {
		error("Unknown caster name when building spells list: %s", gainTable.c_str());
	}

	Common::UString oppositeSchool = "";
	if (_choices->getSpellSchool() < UINT8_MAX) {
		const Aurora::TwoDAFile &twodaSpellsSchool = TwoDAReg.get2DA("spellschools");
		const Aurora::TwoDARow &rowSchool = twodaSpellsSchool.getRow(_choices->getSpellSchool());
		const Aurora::TwoDARow &rowOppSchool = twodaSpellsSchool.getRow(rowSchool.getInt("Opposition"));
		oppositeSchool = rowOppSchool.getString("Letter");
	}

	// Add spells to available and known list.
	const Aurora::TwoDAFile &twodaSpells = TwoDAReg.get2DA("spells");
	for (size_t sp = 0; sp < twodaSpells.getRowCount(); ++sp) {
		// TODO: Check if character already own the spell.
		const Aurora::TwoDARow &spellRow = twodaSpells.getRow(sp);

		if (spellRow.empty("Name"))
			continue;

		if (spellRow.empty(casterName))
			continue;

		// Check spell level.
		uint32_t spellLevel = static_cast<uint32_t>(spellRow.getInt(casterName));
		if (spellLevel > _maxLevel)
			continue;

		// Check spell school.
		if (spellRow.getString("School") == oppositeSchool)
			continue;

		// The wizard knows all the level 0 spells.
		if (gainTable == "CLS_SPGN_WIZ" && spellLevel == 0) {
			Spell spell;
			spell.spellID = sp;
			spell.name = TalkMan.getString(spellRow.getInt("Name"));
			spell.icon = spellRow.getString("IconResRef");
			spell.desc = TalkMan.getString(spellRow.getInt("SpellDesc"));
			_knownSpells[spellLevel].push_back(spell);
			continue;
		}

		Spell spell;
		spell.spellID = sp;
		spell.name = TalkMan.getString(spellRow.getInt("Name"));
		spell.icon = spellRow.getString("IconResRef");
		spell.desc = TalkMan.getString(spellRow.getInt("SpellDesc"));
		_availSpells[spellLevel].push_back(spell);
	}

	// Add icon to spells levels.
	for (uint32_t lvl = 0; lvl <= _maxLevel; ++lvl) {
		WidgetButton *spellLvlButton = getButton("SpellLevel" + Common::composeString<uint32_t>(lvl), true);

		Common::UString lvlStr = Common::composeString<uint32_t>(lvl);
		Common::UString iconName;
		if (lvl == 0) {
			iconName = "ir_cantrips";
		} else if (lvl > 0 && lvl < 7) {
			iconName = "ir_level" + lvlStr;
		} else {
			iconName = "ir_level789";
		}

		const std::vector<Common::UString> texture(1, iconName);
		spellLvlButton->getNode("Plane64")->setTextures(texture);
	}

	// Compute spell level limit due to ability.
	Common::UString abilityStr = classRow.getString("PrimaryAbil");
	const Aurora::TwoDAFile &twodaAbilities = TwoDAReg.get2DA("iprp_abilities");
	for (size_t ab = 0; ab < twodaAbilities.getRowCount(); ++ab) {
		const Aurora::TwoDARow &abilityRow = twodaAbilities.getRow(ab);
		if (abilityStr.toLower() == abilityRow.getString("Label").toLower()) {
			_abilityLimit = _choices->getTotalAbility(static_cast<Ability>(ab)) - 10U;
			break;
		}
	}

	// Show the first spells level that has a level to choose.
	size_t lvlToShow = 0;
	for (size_t lvl = _maxLevel; lvl == 0UL; --lvl)
		if (_remainingSpells[lvl] > 0)
			lvlToShow = lvl;

	showSpellLevel(lvlToShow);
	updateRemainLabel();
}

void CharSpells::computeRemainingSpells(const Aurora::TwoDARow &classRow) {
	uint16_t classLevel = _choices->getCharacter().getClassLevel(_choices->getClass());

	// Check if it is a first level wizard.
	if (classRow.empty("SpellKnownTable") && classLevel == 0) {
		_remainingSpells[1] = 3 + _choices->getAbilityModifier(kAbilityIntelligence);
		return;
	}

	const Aurora::TwoDAFile &twodaSpellKnown = TwoDAReg.get2DA(classRow.getString("SpellKnownTable"));
	const Aurora::TwoDARow &knownRow =twodaSpellKnown.getRow(classLevel);

	// TODO Compute difference between new spells and known ones.

	for (size_t c = 1; c < twodaSpellKnown.getColumnCount(); ++c) {
		if (knownRow.empty(c))
			break;

		_remainingSpells[c - 1] = static_cast<uint8_t>(knownRow.getInt(c));
	}

}

void CharSpells::updateRemainLabel() {
	getLabel("RemainLabel")->setText(Common::composeString<uint8_t>(_remainingSpells[_currentSpellLevel]));
}

void CharSpells::setRecommendedSpells() {
	std::vector<std::vector<uint16_t> > prefSpells;
	_choices->getPrefSpells(prefSpells);

	for (size_t lvl = 0; lvl < _remainingSpells.size(); ++lvl) {
		bool allSpellsMoved = false;
		while (_remainingSpells[lvl] != 0 && !allSpellsMoved) {
			allSpellsMoved = true;
			for (std::vector<Spell>::iterator s = _availSpells[lvl].begin(); s != _availSpells[lvl].end(); ++s) {
				if (_remainingSpells[lvl] == 0)
					break;

				if ((*s).spellID != prefSpells[lvl].front())
					continue;

				allSpellsMoved = false;
				_knownSpells[lvl].push_back(*s);
				_availSpells[lvl].erase(s);

				prefSpells[lvl].erase(prefSpells[lvl].begin());
				--_remainingSpells[lvl];

				break;
			}

			if (allSpellsMoved && prefSpells[lvl].size() > 0) {
				prefSpells[lvl].erase(prefSpells[lvl].begin());
				allSpellsMoved = false;
				continue;
			}

			if (prefSpells[lvl].size() == 0)
				break;
		}
	}

	showSpellLevel(_currentSpellLevel, true);
}

void CharSpells::showSpellHelp(Spell &spell) {
	_spellHelp->setContent(spell.name, spell.desc, spell.icon);

	_spellHelp->show();
	_spellHelp->run();
	_spellHelp->hide();
}

} // End of namespace NWN

} // End of namespace Engines
