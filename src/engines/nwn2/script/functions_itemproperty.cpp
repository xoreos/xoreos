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
 *  Neverwinter Nights 2 engine functions messing with item properties.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/itemproperty.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::getItemPropertyType(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	ctx.getReturn() = (prop) ? prop->getItemPropertyType() : -1;
}

void Functions::getItemPropertySubType(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint16_t subType = prop->getItemPropertySubType();
		if (subType != UINT16_MAX)
			result = static_cast<int>(subType);
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyParam1(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8_t param1 = prop->getItemPropertyParam1();
		if (param1 != UINT8_MAX)
			result = static_cast<int>(param1);
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyParam1Value(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8_t param1Value = prop->getItemPropertyParam1Value();
		if (param1Value != UINT8_MAX)
			result = static_cast<int>(param1Value);
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyCostTable(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8_t costTable = prop->getItemPropertyCostTable();
		if (costTable != UINT8_MAX)
			result = static_cast<int>(costTable);
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyCostTableValue(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8_t costTableValue = prop->getItemPropertyCostTableValue();
		if (costTableValue != UINT8_MAX)
			result = static_cast<int>(costTableValue);
	}
	ctx.getReturn() = result;
}

void Functions::getIsItemPropertyValid(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	if (prop)
		ctx.getReturn() = prop->getIsItemPropertyValid();
}

void Functions::itemPropertyAbilityBonus(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyAbilityBonus;
	uint16_t ability = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (ability >= static_cast<uint16_t>(kAbilityMAX) || bonus < 1 || bonus > kMaxAbilityBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, ability, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyACBonus(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyACBonus;
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxACBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyACBonusVsAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyACBonusVsAlignmentGroup;
	uint16_t alignGroup = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxACBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, alignGroup, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyACBonusVsDmgType(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type =  kItemPropertyACBonusVsDamageType;
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (damageType > kMaxDamageType || bonus < 1 || bonus > kMaxACBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, damageType, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyACBonusVsRace(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyACBonusVsRacialGroup;
	uint16_t race = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxACBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, race, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyACBonusVsSAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyACBonusVsSpecificAlignment;
	uint16_t align = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxACBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, align, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyEnhancementBonus(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyEnhancementBonus;
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxEnhanceBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyEnhancementBonusVsAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyEnhancementBonusVsAlignmentGroup;
	uint16_t alignGroup = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxEnhanceBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, alignGroup, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyEnhancementBonusVsRace(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyEnhancementBonusVsRacialGroup;
	uint16_t race = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxEnhanceBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, race, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyEnhancementBonusVsSAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyEnhancementBonusVsSpecificAlignment;
	uint16_t align = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxEnhanceBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, align, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyEnhancementPenalty(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedEnhancementModifier;
	uint16_t penalty = static_cast<uint16_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxEnhancePenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyWeightReduction(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t reduction = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyBaseItemWeightReduction, UINT16_MAX, UINT8_MAX, UINT8_MAX, reduction);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusFeat(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t feat = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyBonusFeat, feat, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusLevelSpell(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyBonusSpellSlotOfLevelN;
	uint16_t spellClass = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t spellLevel = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (spellLevel > kMaxSpellLevel)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, spellClass, UINT8_MAX, UINT8_MAX, spellLevel);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyCastSpell(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t spell = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t numUses = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyCastSpell, spell, UINT8_MAX, UINT8_MAX, numUses);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageBonus(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyDamageBonus, damageType, UINT8_MAX, UINT8_MAX, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageBonusVsAlign(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t align = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t damageType = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[2].getInt());
	ItemProperty prop(kItemPropertyDamageBonusVsSpecificAlignment, align, UINT8_MAX, damageType, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageBonusVsRace(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t race = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t damageType = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[2].getInt());
	ItemProperty prop(kItemPropertyDamageBonusVsRacialGroup, race, UINT8_MAX, damageType, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageBonusVsSAlign(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t align = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t damageType = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[2].getInt());
	ItemProperty prop(kItemPropertyDamageBonusVsSpecificAlignment, align, UINT8_MAX, damageType, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageImmunity(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t damageType = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[2].getInt());
	ItemProperty prop(kItemPropertyImmunityDamageType, damageType, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamagePenalty(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedDamage;
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxDamagePenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageResistance(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t hpResist = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyDamageResistance, damageType, UINT8_MAX, UINT8_MAX, hpResist);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageVulnerability(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t vulnerability = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyDamageVulnerability, damageType, UINT8_MAX, UINT8_MAX, vulnerability);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDarkvision(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyDarkvision, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDecreaseAbility(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedAbilityScore;
	uint16_t ability = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t modifier = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (ability >= static_cast<uint16_t>(kAbilityMAX) || modifier < 1 || modifier > kMaxAbilityPenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, ability, UINT8_MAX, UINT8_MAX, modifier);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDecreaseAC(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedAC;
	uint16_t modifierType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxDamagePenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, modifierType, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDecreaseSkill(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedSkillModifier;
	uint16_t skill = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxSkillPenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, skill, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyContainerReducedWeight(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t containerType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyEnhancedContainerReducedWeight, UINT16_MAX, UINT8_MAX, UINT8_MAX, containerType);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyExtraMeleeDamageType(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyExtraMeleeDamageType;
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (damageType > kMaxDamageType)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, damageType, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyExtraRangeDamageType(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyExtraRangedDamageType;
	uint16_t damageType = static_cast<uint16_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (damageType > kMaxDamageType)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, damageType, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyHaste(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyHaste, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyHolyAvenger(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyHolyAvenger, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyImprovedEvasion(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyImprovedEvasion, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusSpellResistance(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertySpellResistance, UINT16_MAX, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyImmunityMisc(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t immunityType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyImmunityMiscellaneous, immunityType, UINT8_MAX, UINT8_MAX, 1);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertySavingThrowBonus;
	uint16_t baseSaveType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxSaveBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, baseSaveType, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusSavingThrowVsX(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertySavingThrowBonusSpecific;
	uint16_t bonusType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxSaveBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, bonusType, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyKeen(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyKeen, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyLight(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t brightness = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	uint8_t color = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyLight, UINT16_MAX, UINT8_MAX, color, brightness);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyMaxRangeStrengthMod(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyMighty;
	uint8_t modifier = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (modifier < 1 || modifier > kMaxRangeStrMod)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, modifier);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyNoDamage(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyNoDamage, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyOnHitProps(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t property = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t saveDC = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	uint8_t special = static_cast<uint8_t>(ctx.getParams()[2].getInt());

	// Load the property table row from the 2da On-Hit data
	uint8_t param1 = UINT8_MAX;
	const Aurora::TwoDAFile &twoDA = TwoDAReg.get2DA("iprp_onhit");
	if (property < twoDA.getRowCount()) {
		const Aurora::TwoDARow &row = twoDA.getRow(property);
		param1 = static_cast<uint8_t>(row.getInt("Param1ResRef"));
	}

	ItemProperty prop(kItemPropertyOnHitProperties, property, param1, special, saveDC);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyReducedSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedSavingThrows;
	uint16_t bonusType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxSavePenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, bonusType, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyReducedSavingThrowVsX(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedSavingThrowsSpecific;
	uint16_t baseSaveType = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxSavePenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, baseSaveType, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyRegeneration(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyRegeneration;
	uint8_t regeneration = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (regeneration < 1 || regeneration > kMaxRegeneration)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, regeneration);
	ctx.getReturn() = prop;
}

void Functions::itemPropertySkillBonus(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertySkillBonus;
	uint16_t skill = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxSkillBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, skill, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertySpellImmunitySpecific(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t spell = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyImmunitySpecificSpell, UINT16_MAX, UINT8_MAX, UINT8_MAX, spell);
	ctx.getReturn() = prop;
}

void Functions::itemPropertySpellImmunitySchool(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t school = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyImmunitySpellSchool, school, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyThievesTools(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyThievesTools;
	uint8_t modifier = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (modifier < 1 || modifier > kMaxThievesTools)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, modifier);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyAttackBonus(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyAttackBonus;
	uint16_t bonus = static_cast<uint16_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxAttackBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyAttackBonusVsAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyAttackBonusVsAlignmentGroup;
	uint16_t alignGroup = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxAttackBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, alignGroup, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyAttackBonusVsRace(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyAttackBonusVsRacialGroup;
	uint16_t race = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxAttackBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, race, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyAttackBonusVsSAlign(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyAttackBonusVsSpecificAlignment;
	uint16_t align = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t bonus = static_cast<uint8_t>(ctx.getParams()[1].getInt());

	// Check the range
	if (bonus < 1 || bonus > kMaxAttackBonus)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, align, UINT8_MAX, UINT8_MAX, bonus);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyAttackPenalty(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyDecreasedAttackModifier;
	uint8_t penalty = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (penalty < 1 || penalty > kMaxACPenalty)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, penalty);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyUnlimitedAmmo(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t ammoDamage = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyUnlimitedAmmunition, UINT16_MAX, UINT8_MAX, UINT8_MAX, ammoDamage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyLimitUseByAlign(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t useAlign = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyUseLimitationAlignmentGroup, useAlign, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyLimitUseByClass(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t useClass = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyUseLimitationClass, useClass, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyLimitUseByRace(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t useRace = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyUseLimitationRacialType, useRace, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyLimitUseBySAlign(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t useSAlign = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyUseLimitationSpecificAlignment, useSAlign, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyBonusHitpoints(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t bonusType = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyBonusHitpoints, UINT16_MAX, UINT8_MAX, UINT8_MAX, bonusType);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyVampiricRegeneration(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t regenAmount = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyRegenerationVampiric, UINT16_MAX, UINT8_MAX, UINT8_MAX, regenAmount);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyTrap(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t level = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t type = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyTrap, level, UINT8_MAX, UINT8_MAX, type);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyTrueSeeing(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyTrueSeeing, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyOnMonsterHitProperties(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t property = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t special = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyOnMonsterHit, property, UINT8_MAX, special, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyTurnResistance(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyTurnResistance;
	uint8_t modifier = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (modifier < 1 || modifier > kMaxTurnResist)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, modifier);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyMassiveCritical(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyMassiveCriticals, UINT16_MAX, UINT8_MAX, UINT8_MAX, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyFreeAction(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty prop(kItemPropertyFreedomOfMovement, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyMonsterDamage(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t damage = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyMonsterDamage, UINT16_MAX, UINT8_MAX, UINT8_MAX, damage);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyImmunityToSpellLevel(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyImmunitySpellsByLevel;
	uint8_t level = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	/*
	 * In the game engine, the value returned by the getItemPropertyCostTableValue
	 * call for this item property is level-1. A level of '1' is invalid in the
	 * game engine, in contradiction to the command's documentation.
	 */

	// Check the range
	if (level < 2 || level > kMaxSpellLevel)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, level - 1);
	ctx.getReturn() = prop;
}

void Functions::itemPropertySpecialWalk(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t walk = static_cast<uint8_t>(ctx.getParams()[0].getInt()); // Doesn't seem to get used
	ItemProperty prop(kItemPropertySpecialWalk, UINT16_MAX, UINT8_MAX, UINT8_MAX, walk);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyHealersKit(Aurora::NWScript::FunctionContext &ctx) {
	ItemPropertyType type = kItemPropertyHealersKit;
	uint8_t modifier = static_cast<uint8_t>(ctx.getParams()[0].getInt());

	// Check the range
	if (modifier < 1 || modifier > kMaxHealersKit)
		type = kItemPropertyInvalid;

	ItemProperty prop(type, UINT16_MAX, UINT8_MAX, UINT8_MAX, modifier);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyWeightIncrease(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t weight = static_cast<uint8_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyWeightIncrease, UINT16_MAX, UINT8_MAX, weight, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyOnHitCastSpell(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t spell = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	uint8_t level = static_cast<uint8_t>(ctx.getParams()[1].getInt());
	ItemProperty prop(kItemPropertyOnHitCastSpell, spell, UINT8_MAX, UINT8_MAX, level);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyVisualEffect(Aurora::NWScript::FunctionContext &ctx) {
	uint16_t effect = static_cast<uint16_t>(ctx.getParams()[0].getInt());
	ItemProperty prop(kItemPropertyWeightIncrease, effect, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyArcaneSpellFailure(Aurora::NWScript::FunctionContext &ctx) {
	uint8_t modLevel = static_cast<uint8_t>(ctx.getParams()[0].getInt());
        ItemProperty prop(kItemPropertyArcaneSpellFailure, UINT16_MAX, UINT8_MAX, UINT8_MAX, modLevel);
	ctx.getReturn() = prop;
}

void Functions::itemPropertyDamageReduction(Aurora::NWScript::FunctionContext &ctx) {
	/*
	 * This script operation is non-functional in the game, so this will
	 * return an invalid ItemProperty for backwards compatibility. It can
	 * still be applied from the toolset or an effect.
	 */
	ItemProperty prop(kItemPropertyInvalid, UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
	ctx.getReturn() = prop;
}

} // End of namespace NWN2

} // End of namespace Engines
