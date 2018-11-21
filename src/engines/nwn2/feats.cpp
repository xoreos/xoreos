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
 *  Feats for a Neverwinter Nights 2 creature.
 */

#include "src/common/debug.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/feats.h"

namespace Engines {

namespace NWN2 {

Feats::Feats() {
	clear();
}

Feats::~Feats() {
}

/** Clear all feat information and reset the modifiers */
void Feats::clear() {
	_feats.clear();
	initParameters();
}

/** Add the feat to the list */
void Feats::featAdd(const uint32 id) {
	_feats.push_back(id);
	applyFeat(id);
}

/** Remove the feat from the list */
void Feats::featRemove(const uint32 id) {
	// Look for a matching feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it) {
		if (*it == id) {
			// Found a match, so remove it
			_feats.erase(it);

			// Rebuild the data
			resetFeats();
			break;
		}
	}
}

/** Return true if the feat is in the list */
bool Feats::getHasFeat(uint32 id) const {
	// Look for a matching feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it)
		if (*it == id)
			return true;

	return false;
}

/** Return the cumulative modifier for this skill */
int Feats::getFeatsSkillBonus(uint32 skill) const {
	assert(skill < kSkillMAX);
	return _skillBonus[skill];
}

int Feats::getFeatsSaveVsBonus(uint32 type) const {
	assert(type < kSaveMAX);
	return _saveVsBonus[type];
}

/** Return the cumulative Fortitude modifier */
int Feats::getFeatsFortBonus() const {
	return _fortBonus;
}

/** Return the cumulative Reflex modifier */
int Feats::getFeatsRefBonus() const {
	return _refBonus;
}

/** Return the cumulative Willpower modifier */
int Feats::getFeatsWillBonus() const {
	return _willBonus;
}

/** Return the initiative modifier */
int Feats::getFeatsInitBonus() const {
	return _initBonus;
}

/** Return the luck AC modifier */
int Feats::getFeatsLuckACBonus() const {
	return _luckACBonus;
}

/** Return true if the custom code feat is flagged */
bool Feats::getHasCustomFeat(Custom feat) const {
	assert((feat > -1) && (feat < kCustomMAX));
	return _hasCustomFeat[feat];
}

/** Initialize the data */
void Feats::initParameters() {
	uint i;

	// Skill modifiers
	for (i = 0; i < kSkillMAX; i++)
		_skillBonus[i] = 0;

	// Save Vs. modifiers
	for (i = 0; i < kSaveMAX; i++)
		_saveVsBonus[i] = 0;

	// Save modifiers
	_fortBonus = 0;
	_refBonus = 0;
	_willBonus = 0;

	// Other modifiers
	_luckACBonus = 0;
	_initBonus = 0;

	// Custom code flags
	for (i = 0; i < kCustomMAX; i++)
		_hasCustomFeat[i] = false;
}

/** Apply all feats */
void Feats::resetFeats() {
	// Reset the parameters
	initParameters();

	// Apply modifiers for each feat
	for (std::vector<uint32>::const_iterator it = _feats.begin(); it != _feats.end(); ++it)
		applyFeat(*it);
}

/**
 * Update the instance data for the feat with row
 * number 'id' in the feats.2da table.
 *
 * If 'isAdd' is true, this change is a feat addition.
 * Otherwise, it is for a feat removal.
 *
 * This function uses a single large switch statement
 * so the compiler can build an indexed branch table.
 */
void Feats::applyFeat(const uint32 id) {
	switch(id) {

		/* ---- PC History feats ---- */

		case kFeatHistoryAppraiser:
			// +2 Appraise, +1 Lore, -1 Spot, -2 Bluff, -2 Sleight of hand
			_skillBonus[kSkillAppraise]      += +2;
			_skillBonus[kSkillLore]          += +1;
			_skillBonus[kSkillSpot]          += -1;
			_skillBonus[kSkillBluff]         += -2;
			_skillBonus[kSkillSleightOfHand] += -2;
			break;

		case kFeatHistoryBully:
			// +1 Intimidate, +1 Fortitude Save, -1 Bluff, -2 Diplomacy
			_skillBonus[kSkillIntimidate] += +2;
			_fortBonus                    += +1;
			_skillBonus[kSkillBluff]      += -1;
			_skillBonus[kSkillDiplomacy]  += -2;
			break;

		case kFeatHistoryComplex:
			// Free focus skill (any)
			// Note: this feat was not available in the original game
			_hasCustomFeat[kCustomComplex] = true;
			break;

		case kFeatHistoryConfidant:
			// +1 Bluff, -1 Intimidate, -1 Taunt
			_skillBonus[kSkillBluff]      += +1;
			_skillBonus[kSkillIntimidate] += -1;
			_skillBonus[kSkillTaunt]      += -1;
			break;

		case kFeatHistoryDevout:
			// +1 Will Save, +1 Concentration, -1 Diplomacy, -1 Bluff
			_skillBonus[kSkillConcentration] += +1;
			_skillBonus[kSkillDiplomacy]     += -1;
			_skillBonus[kSkillBluff]         += -1;
			_willBonus                       += +1;
			break;

		case kFeatHistoryFarmer:
			// +1 Survival, +1 Spot, -2 Lore
			_skillBonus[kSkillSurvival] += +1;
			_skillBonus[kSkillSpot]     += +1;
			_skillBonus[kSkillLore]     += -2;
			break;

		case kFeatHistoryForeigner:
			// +1 Lore, -1 Diplomacy
			_skillBonus[kSkillLore]      += +1;
			_skillBonus[kSkillDiplomacy] += -1;
			break;

		case kFeatHistoryLadiesMan:
			// +1 Listen, +1 Diplomacy, -2 Intimidate
			_skillBonus[kSkillListen]     += +1;
			_skillBonus[kSkillDiplomacy]  += +1;
			_skillBonus[kSkillIntimidate] += -2;
			break;

		case kFeatHistoryMilitia:
			// +2 Parry, +1 Craft armor, +1 Craft weapon, -2 Will Save
			_skillBonus[kSkillParry]       += +2;
			_skillBonus[kSkillCraftArmor]  += +1;
			_skillBonus[kSkillCraftWeapon] += +1;
			_willBonus                     += -2;
			break;

		case kFeatHistoryNaturalLeader:
			// +1 to companions' attack rolls, -1 to your saving throws
			_hasCustomFeat[kCustomNaturalLeader] = true;
			_fortBonus += -1;
			_refBonus  += -1;
			_willBonus += -1;
			break;

		case kFeatHistorySavvy:
			// +1 Diplomacy, -1 Will Saving Throws
			_skillBonus[kSkillDiplomacy] += +1;
			_willBonus                   += -1;
			break;

		case kFeatHistorySurvivor:
			// +1 Search, +1 Spot, +1 Survival, -2 Fortitude Saving Throws
			_skillBonus[kSkillSearch]   += +1;
			_skillBonus[kSkillSpot]     += +1;
			_skillBonus[kSkillSurvival] += +1;
			_fortBonus                  += -2;
			break;

		case kFeatHistoryTaleTeller:
			// +1 Lore, +1 Bluff, -2 Concentration
			_skillBonus[kSkillLore]          += +1;
			_skillBonus[kSkillBluff]         += +1;
			_skillBonus[kSkillConcentration] += -2;
			break;

		case kFeatHistoryTalent:
			// +1 Perform, -1 Will Saving Throws
			_skillBonus[kSkillPerform] += +1;
			_willBonus                 += -1;
			break;

		case kFeatHistoryTheFlirt:
			// +1 Listen, +1 Diplomacy, -2 Intimidate
			_skillBonus[kSkillListen]     += +1;
			_skillBonus[kSkillDiplomacy]  += +1;
			_skillBonus[kSkillIntimidate] += -2;
			break;

		case kFeatHistoryTroublemaker:
			// +1 Reflex Save, +1 Set trap, +1 Sleight of Hand, -2 Will Save
			_refBonus                        += +1;
			_skillBonus[kSkillSetTrap]       += +1;
			_skillBonus[kSkillSleightOfHand] += -1;
			_willBonus                       += -2;
			break;

		case kFeatHistoryVeteran:
			// +1 Fortitude Save, -1 Diplomacy
			_fortBonus                   += +1;
			_skillBonus[kSkillDiplomacy] += -1;
			break;

		case kFeatHistoryWildChild:
			// +1 Survival, +1 Tumble, +1 Hide, +1 Move Silently, -2 Lore, -2 Appraise
			_skillBonus[kSkillSurvival]     += +1;
			_skillBonus[kSkillTumble]       += +1;
			_skillBonus[kSkillHide]         += +1;
			_skillBonus[kSkillMoveSilently] += +1;
			_skillBonus[kSkillLore]         += -2;
			_skillBonus[kSkillAppraise]     += -2;
			break;

		case kFeatHistoryWizardsApprentice:
			// +1 Spellcraft, +1 Lore, +1 Craft Alchemy, -1 Fortitude Save, -1 Spot
			_skillBonus[kSkillSpellcraft]   += +1;
			_skillBonus[kSkillLore]         += +1;
			_skillBonus[kSkillCraftAlchemy] += +1;
			_fortBonus                      += -1;
			_skillBonus[kSkillSpot]         += -1;
			break;

		/* ---- Racial feats ---- */

		case kFeatSkillAffinitySearch:
			// +2 to Search
			_skillBonus[kSkillSearch] += +2;
			break;

		case kFeatPSASearch:
			// +1 to Search
			_skillBonus[kSkillSearch] += +1;
			break;

		case kFeatStonecunning:
			// +2 to Search while in interior area
			_hasCustomFeat[kCustomStonecunning] = true;
			break;

		/* ---- Background feats ---- */

		case kFeatBkgdArtist:
			// +2 bonus on Perform and Diplomacy checks
			_skillBonus[kSkillPerform]   += +2;
			_skillBonus[kSkillDiplomacy] += +2;
			break;

		case kFeatBkgdBlooded:
			// +2 bonus on Initiative and Spot checks
			_skillBonus[kSkillSpot] += +2;
			_initBonus              += +2;
			break;

		case kFeatBkgdBullheaded:
			// +2 bonus to resist Taunt, and +1 bonus to Will saving throws
			_skillBonus[kSkillTaunt] += +2;
			_willBonus               += +1;
			break;

		case kFeatBkgdCourteousMagocracy:
			// +2 bonus on Lore and Spellcraft checks
			_skillBonus[kSkillLore]       += +2;
			_skillBonus[kSkillSpellcraft] += +2;
			break;

		case kFeatBkgdLuckOfHeroes:
			// +1 bonus on all saving throws plus a +1 luck bonus to armor class
			_fortBonus   += +1;
			_refBonus    += +1;
			_willBonus   += +1;
			_luckACBonus += +1;
			break;

		case kFeatBkgdMindOverBody:
			/* Uses Int modifier instead of Con for bonus hit points
			 * at level 1, and any metamagic or spellcasting feat
			 * grants +1 hit points.
			 */
			_hasCustomFeat[kCustomNatureSense] = true;
			break;

		case kFeatBkgdSilverPalm:
			// +2 bonus on Appraise, Bluff, and Diplomacy checks
			_skillBonus[kSkillAppraise]  += +2;
			_skillBonus[kSkillBluff]     += +2;
			_skillBonus[kSkillDiplomacy] += +2;
			break;

		case kFeatBkgdSnakeBlood:
			// +1 bonus to Reflex saving throws, and +2 saving throw bonus against poisons
			_saveVsBonus[kSavePoison] += +2;
			_refBonus                 += +1;
			break;

		case kFeatBkgdSpellcastingProdigy:
			/* treats primary spellcasting ability score as 2 points
			 * higher than usual, effectively having +1 to modifier.
			 */
			_hasCustomFeat[kCustomSpellcastingProdigy] = true;
			break;

		case kFeatBkgdStrongSoul:
			// +1 bonus to Fortitude & Will saving throws, and +1 saving throw bonus against Death magic
			_saveVsBonus[kSaveDeath] += 1;
			_fortBonus               += 1;
			_willBonus               += 1;
			break;

		case kFeatBkgdThug:
			// +2 bonus on Appraise, Initiative, and Intimidate checks 
			_skillBonus[kSkillAppraise]   += 2;
			_skillBonus[kSkillIntimidate] += 2;
			_initBonus                    += 2;
			break;

		/* ---- Skill feats ---- */

		case kFeatAlertness:
			// +2 to Listen and Spot
			_skillBonus[kSkillListen]        += +2;
			_skillBonus[kSkillSpot]          += +2;
			break;

		case kFeatNegotiator:
			// +2 to Bluff and Diplomacy
			_skillBonus[kSkillBluff]         += +2;
			_skillBonus[kSkillDiplomacy]     += +2;
			break;

		case kFeatNimbleFingers:
			// +2 to Open Locks and Disable Device
			_skillBonus[kSkillOpenLock]      += +2;
			_skillBonus[kSkillDisableDevice] += +2;
			break;

		case kFeatSelfSufficient:
			// +2 to Heal and Survival
			_skillBonus[kSkillHeal]          += +2;
			_skillBonus[kSkillSurvival]      += +2;
			break;

		case kFeatStealthy:
			// +2 to Hide and Move Silently
			_skillBonus[kSkillHide]          += +2;
			_skillBonus[kSkillMoveSilently]  += +2;
			break;

		/* ---- Skill focus feats ---- */

		case kFeatSkillFocusAppraise:
			// +3 to Appraise
			_skillBonus[kSkillAppraise]       += 3;
			break;

		case kFeatSkillFocusBluff:
			// +3 to Bluff
			_skillBonus[kSkillBluff]          += 3;
			break;

		case kFeatSkillFocusConcentrate:
			// +3 to Concentrate
			_skillBonus[kSkillConcentration]  += +3;
			break;

		case kFeatSkillFocusCraftAlchemy:
			// +3 to Craft Alchemy
			_skillBonus[kSkillCraftAlchemy]   += +3;
			break;

		case kFeatSkillFocusCraftArmor:
			// +3 to Craft Armor
			_skillBonus[kSkillCraftArmor]     += +3;
			break;

		case kFeatSkillFocusCraftTrap:
			// +3 to Craft Trap
			_skillBonus[kSkillCraftTrap]      += +3;
			break;

		case kFeatSkillFocusCraftWeapon:
			// +3 to Craft Weapon
			_skillBonus[kSkillCraftWeapon]    += +3;
			break;

		case kFeatSkillFocusDisableDevice:
			// +3 to Disable Device
			_skillBonus[kSkillDisableDevice]  += +3;
			break;

		case kFeatSkillFocusHeal:
			// +3 to Heal
			_skillBonus[kSkillHeal]           += +3;
			break;

		case kFeatSkillFocusHide:
			// +3 to Hide
			_skillBonus[kSkillHide]           += +3;
			break;

		case kFeatSkillFocusIntimidate:
			// +3 to Intimidate
			_skillBonus[kSkillIntimidate]     += +3;
			break;

		case kFeatSkillFocusListen:
			// +3 to Listen
			_skillBonus[kSkillListen]         += +3;
			break;

		case kFeatSkillFocusLore:
			// +3 to Lore
			_skillBonus[kSkillLore]           += +3;
			break;

		case kFeatSkillFocusMoveSilently:
			// +3 to Move Silently
			_skillBonus[kSkillMoveSilently]   += +3;
			break;

		case kFeatSkillFocusOpenLock:
			// +3 to Open Lock
			_skillBonus[kSkillOpenLock]       += +3;
			break;

		case kFeatSkillFocusParry:
			// +3 to Parry
			_skillBonus[kSkillParry]          += +3;
			break;

		case kFeatSkillFocusPerform:
			// +3 to Perform
			_skillBonus[kSkillPerform]        += +3;
			break;

		case kFeatSkillFocusSearch:
			// +3 to Search
			_skillBonus[kSkillSearch]         += +3;
			break;

		case kFeatSkillFocusSetTrap:
			// +3 to Set Trap
			_skillBonus[kSkillSetTrap]        += +3;
			break;

		case kFeatSkillFocusSleightOfHand:
			// +3 to Sleight of Hand
			_skillBonus[kSkillSleightOfHand]  += +3;
			break;

		case kFeatSkillFocusSpellcraft:
			// +3 to Spellcraft
			_skillBonus[kSkillSpellcraft]     += +3;
			break;

		case kFeatSkillFocusSpot:
			// +3 to Spot
			_skillBonus[kSkillSpot]           += +3;
			break;

		case kFeatSkillFocusSurvival:
			// +3 to Survival
			_skillBonus[kSkillSurvival]       += +3;
			break;

		case kFeatSkillFocusTaunt:
			// +3 to Taunt
			_skillBonus[kSkillTaunt]          += +3;
			break;

		case kFeatSkillFocusTumble:
			// +3 to Tumble
			_skillBonus[kSkillTumble]         += +3;
			break;

		case kFeatSkillFocusUseMagicDevice:
			// +3 to Use Magic Device
			_skillBonus[kSkillUseMagicDevice] += +3;
			break;

		/* ---- Class feats ---- */

		case kFeatNatureSense:
			// +2 to Survival in all environments and a +2
			// to Search and Spot while in wilderness areas
			_hasCustomFeat[kCustomNatureSense] = true;
			_skillBonus[kSkillSurvival] += +2;
			break;

		default:
			// Many feats are informational only
			break;
	}
}

} // End of namespace NWN2

} // End of namespace Engines
