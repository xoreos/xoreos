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
 *  The ability score allocation menu for custom character creation.
 */

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/engines/odyssey/button.h"
#include "src/engines/odyssey/label.h"

#include "src/engines/kotorbase/gui/chargeninfo.h"

#include "src/engines/kotor/gui/chargen/chargenabilities.h"

namespace Engines {

namespace KotOR {

// The point-buy pool available at level 1.
static const int kAbilityPointPool = 30;
// Minimum ability score allowed during character creation.
static const uint32_t kAbilityMin = 8;
// Maximum ability score allowed during character creation.
static const uint32_t kAbilityMax = 18;

CharacterGenerationAbilitiesMenu::CharacterGenerationAbilitiesMenu(
		KotORBase::CharacterGenerationInfo &info,
		Console *console) :
		CharacterGenerationBaseMenu(info, console) {

	load("abitems");

	addBackground(KotORBase::kBackgroundTypeMenu);

	// Initialise working values from whatever is currently stored.
	_str  = info.getAbilities().strength;
	_dex  = info.getAbilities().dexterity;
	_con  = info.getAbilities().constitution;
	_intl = info.getAbilities().intelligence;
	_wis  = info.getAbilities().wisdom;
	_cha  = info.getAbilities().charisma;

	// Any scores above the minimum have already "spent" some points; account
	// for that so the pool is consistent when the menu opens.
	auto spentFor = [](uint32_t score) -> int {
		int spent = 0;
		for (uint32_t s = kAbilityMin; s < score; ++s)
			spent += raiseCost(s);
		return spent;
	};

	_remainingPoints = kAbilityPointPool
	                   - spentFor(_str) - spentFor(_dex) - spentFor(_con)
	                   - spentFor(_intl) - spentFor(_wis) - spentFor(_cha);

	updateLabels();
}

int CharacterGenerationAbilitiesMenu::raiseCost(uint32_t current) {
	if (current <= 13) return 1;
	if (current <= 15) return 2;
	if (current == 16) return 3;
	if (current == 17) return 4;
	return 5; // 18
}

int CharacterGenerationAbilitiesMenu::lowerCost(uint32_t current) {
	// Refund equals what raising cost to reach current from current-1.
	return raiseCost(current - 1);
}

void CharacterGenerationAbilitiesMenu::updateLabels() {
	// Helper to set a widget's text if the widget exists.
	// Tries label first, then button, to handle either widget type.
	auto setWidgetText = [this](const char *tag, const Common::UString &text) {
		Odyssey::WidgetLabel *lbl = getLabel(tag);
		if (lbl) {
			lbl->setText(text);
			return;
		}
		Odyssey::WidgetButton *btn = getButton(tag);
		if (btn)
			btn->setText(text);
	};

	setWidgetText("STR_POINTS_BTN", Common::composeString(_str));
	setWidgetText("DEX_POINTS_BTN", Common::composeString(_dex));
	setWidgetText("CON_POINTS_BTN", Common::composeString(_con));
	setWidgetText("INT_POINTS_BTN", Common::composeString(_intl));
	setWidgetText("WIS_POINTS_BTN", Common::composeString(_wis));
	setWidgetText("CHA_POINTS_BTN", Common::composeString(_cha));

	setWidgetText("REMAINING_SELECTIONS_LBL", Common::composeString(_remainingPoints));
}

void CharacterGenerationAbilitiesMenu::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();

	struct AbilityRef {
		const char *plusTag;
		const char *minusTag;
		uint32_t   *value;
	};

	static const AbilityRef kAbilityRefs[] = {
		{ "BTN_STR_PLUS", "BTN_STR_MINUS", &_str  },
		{ "BTN_DEX_PLUS", "BTN_DEX_MINUS", &_dex  },
		{ "BTN_CON_PLUS", "BTN_CON_MINUS", &_con  },
		{ "BTN_INT_PLUS", "BTN_INT_MINUS", &_intl },
		{ "BTN_WIS_PLUS", "BTN_WIS_MINUS", &_wis  },
		{ "BTN_CHA_PLUS", "BTN_CHA_MINUS", &_cha  },
	};

	for (size_t i = 0; i < ARRAYSIZE(kAbilityRefs); ++i) {
		if (tag == kAbilityRefs[i].plusTag) {
			uint32_t &v = *kAbilityRefs[i].value;
			if (v < kAbilityMax) {
				int cost = raiseCost(v);
				if (_remainingPoints >= cost) {
					_remainingPoints -= cost;
					++v;
					updateLabels();
				}
			}
			return;
		}
		if (tag == kAbilityRefs[i].minusTag) {
			uint32_t &v = *kAbilityRefs[i].value;
			if (v > kAbilityMin) {
				_remainingPoints += lowerCost(v);
				--v;
				updateLabels();
			}
			return;
		}
	}

	if (tag == "BTN_RECOMMENDED") {
		// Reset to class defaults and re-display.
		const KotORBase::CreatureInfo::Abilities &base = _info.getAbilities();
		_str  = base.strength;
		_dex  = base.dexterity;
		_con  = base.constitution;
		_intl = base.intelligence;
		_wis  = base.wisdom;
		_cha  = base.charisma;
		_remainingPoints = kAbilityPointPool;
		updateLabels();
		return;
	}

	if (tag == "BTN_BACK") {
		_returnCode = 1;
		return;
	}

	if (tag == "BTN_ACCEPT") {
		_info.setAbilityScore(KotORBase::kAbilityStrength,     _str);
		_info.setAbilityScore(KotORBase::kAbilityDexterity,    _dex);
		_info.setAbilityScore(KotORBase::kAbilityConstitution, _con);
		_info.setAbilityScore(KotORBase::kAbilityIntelligence, _intl);
		_info.setAbilityScore(KotORBase::kAbilityWisdom,       _wis);
		_info.setAbilityScore(KotORBase::kAbilityCharisma,     _cha);
		accept();
		_returnCode = 1;
		return;
	}
}

} // End of namespace KotOR

} // End of namespace Engines
