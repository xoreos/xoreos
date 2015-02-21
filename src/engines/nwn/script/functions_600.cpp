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
 *  NWN script functions, 600-699.
 */

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/script/functions.h"

using Aurora::kObjectIDInvalid;

// NWScript
using Aurora::NWScript::kTypeVoid;
using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeEngineType;
using Aurora::NWScript::kTypeVector;
using Aurora::NWScript::kTypeScriptState;
using Aurora::NWScript::createSignature;
using Aurora::NWScript::createDefaults;

namespace Engines {

namespace NWN {

void ScriptFunctions::registerFunctions600(const Defaults &d) {
	FunctionMan.registerFunction("CopyObject", 600,
			boost::bind(&ScriptFunctions::copyObject, this, _1),
			createSignature(5, kTypeObject, kTypeObject, kTypeEngineType, kTypeObject, kTypeString),
			createDefaults(2, d.object0, d.stringEmpty));
	FunctionMan.registerFunction("DeleteCampaignVariable", 601,
			boost::bind(&ScriptFunctions::deleteCampaignVariable, this, _1),
			createSignature(4, kTypeVoid, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("StoreCampaignObject", 602,
			boost::bind(&ScriptFunctions::storeCampaignObject, this, _1),
			createSignature(5, kTypeInt, kTypeString, kTypeString, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("RetrieveCampaignObject", 603,
			boost::bind(&ScriptFunctions::retrieveCampaignObject, this, _1),
			createSignature(6, kTypeObject, kTypeString, kTypeString, kTypeEngineType, kTypeObject, kTypeObject),
			createDefaults(2, d.object0, d.object0));
	FunctionMan.registerFunction("EffectCutsceneDominated", 604,
			boost::bind(&ScriptFunctions::effectCutsceneDominated, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetItemStackSize", 605,
			boost::bind(&ScriptFunctions::getItemStackSize, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetItemStackSize", 606,
			boost::bind(&ScriptFunctions::setItemStackSize, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetItemCharges", 607,
			boost::bind(&ScriptFunctions::getItemCharges, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetItemCharges", 608,
			boost::bind(&ScriptFunctions::setItemCharges, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("AddItemProperty", 609,
			boost::bind(&ScriptFunctions::addItemProperty, this, _1),
			createSignature(5, kTypeVoid, kTypeInt, kTypeEngineType, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("RemoveItemProperty", 610,
			boost::bind(&ScriptFunctions::removeItemProperty, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeEngineType));
	FunctionMan.registerFunction("GetIsItemPropertyValid", 611,
			boost::bind(&ScriptFunctions::getIsItemPropertyValid, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetFirstItemProperty", 612,
			boost::bind(&ScriptFunctions::getFirstItemProperty, this, _1),
			createSignature(2, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("GetNextItemProperty", 613,
			boost::bind(&ScriptFunctions::getNextItemProperty, this, _1),
			createSignature(2, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("GetItemPropertyType", 614,
			boost::bind(&ScriptFunctions::getItemPropertyType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetItemPropertyDurationType", 615,
			boost::bind(&ScriptFunctions::getItemPropertyDurationType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyAbilityBonus", 616,
			boost::bind(&ScriptFunctions::itemPropertyAbilityBonus, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyACBonus", 617,
			boost::bind(&ScriptFunctions::itemPropertyACBonus, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyACBonusVsAlign", 618,
			boost::bind(&ScriptFunctions::itemPropertyACBonusVsAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyACBonusVsDmgType", 619,
			boost::bind(&ScriptFunctions::itemPropertyACBonusVsDmgType, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyACBonusVsRace", 620,
			boost::bind(&ScriptFunctions::itemPropertyACBonusVsRace, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyACBonusVsSAlign", 621,
			boost::bind(&ScriptFunctions::itemPropertyACBonusVsSAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyEnhancementBonus", 622,
			boost::bind(&ScriptFunctions::itemPropertyEnhancementBonus, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyEnhancementBonusVsAlign", 623,
			boost::bind(&ScriptFunctions::itemPropertyEnhancementBonusVsAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyEnhancementBonusVsRace", 624,
			boost::bind(&ScriptFunctions::itemPropertyEnhancementBonusVsRace, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyEnhancementBonusVsSAlign", 625,
			boost::bind(&ScriptFunctions::itemPropertyEnhancementBonusVsSAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyEnhancementPenalty", 626,
			boost::bind(&ScriptFunctions::itemPropertyEnhancementPenalty, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyWeightReduction", 627,
			boost::bind(&ScriptFunctions::itemPropertyWeightReduction, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyBonusFeat", 628,
			boost::bind(&ScriptFunctions::itemPropertyBonusFeat, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyBonusLevelSpell", 629,
			boost::bind(&ScriptFunctions::itemPropertyBonusLevelSpell, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyCastSpell", 630,
			boost::bind(&ScriptFunctions::itemPropertyCastSpell, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageBonus", 631,
			boost::bind(&ScriptFunctions::itemPropertyDamageBonus, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageBonusVsAlign", 632,
			boost::bind(&ScriptFunctions::itemPropertyDamageBonusVsAlign, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageBonusVsRace", 633,
			boost::bind(&ScriptFunctions::itemPropertyDamageBonusVsRace, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageBonusVsSAlign", 634,
			boost::bind(&ScriptFunctions::itemPropertyDamageBonusVsSAlign, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageImmunity", 635,
			boost::bind(&ScriptFunctions::itemPropertyDamageImmunity, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamagePenalty", 636,
			boost::bind(&ScriptFunctions::itemPropertyDamagePenalty, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageReduction", 637,
			boost::bind(&ScriptFunctions::itemPropertyDamageReduction, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageResistance", 638,
			boost::bind(&ScriptFunctions::itemPropertyDamageResistance, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDamageVulnerability", 639,
			boost::bind(&ScriptFunctions::itemPropertyDamageVulnerability, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDarkvision", 640,
			boost::bind(&ScriptFunctions::itemPropertyDarkvision, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyDecreaseAbility", 641,
			boost::bind(&ScriptFunctions::itemPropertyDecreaseAbility, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDecreaseAC", 642,
			boost::bind(&ScriptFunctions::itemPropertyDecreaseAC, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyDecreaseSkill", 643,
			boost::bind(&ScriptFunctions::itemPropertyDecreaseSkill, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyContainerReducedWeight", 644,
			boost::bind(&ScriptFunctions::itemPropertyContainerReducedWeight, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyExtraMeleeDamageType", 645,
			boost::bind(&ScriptFunctions::itemPropertyExtraMeleeDamageType, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyExtraRangeDamageType", 646,
			boost::bind(&ScriptFunctions::itemPropertyExtraRangeDamageType, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyHaste", 647,
			boost::bind(&ScriptFunctions::itemPropertyHaste, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyHolyAvenger", 648,
			boost::bind(&ScriptFunctions::itemPropertyHolyAvenger, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyImmunityMisc", 649,
			boost::bind(&ScriptFunctions::itemPropertyImmunityMisc, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyImprovedEvasion", 650,
			boost::bind(&ScriptFunctions::itemPropertyImprovedEvasion, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyBonusSpellResistance", 651,
			boost::bind(&ScriptFunctions::itemPropertyBonusSpellResistance, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyBonusSavingThrowVsX", 652,
			boost::bind(&ScriptFunctions::itemPropertyBonusSavingThrowVsX, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyBonusSavingThrow", 653,
			boost::bind(&ScriptFunctions::itemPropertyBonusSavingThrow, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyKeen", 654,
			boost::bind(&ScriptFunctions::itemPropertyKeen, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyLight", 655,
			boost::bind(&ScriptFunctions::itemPropertyLight, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyMaxRangeStrengthMod", 656,
			boost::bind(&ScriptFunctions::itemPropertyMaxRangeStrengthMod, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyNoDamage", 657,
			boost::bind(&ScriptFunctions::itemPropertyNoDamage, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyOnHitProps", 658,
			boost::bind(&ScriptFunctions::itemPropertyOnHitProps, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("ItemPropertyReducedSavingThrowVsX", 659,
			boost::bind(&ScriptFunctions::itemPropertyReducedSavingThrowVsX, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyReducedSavingThrow", 660,
			boost::bind(&ScriptFunctions::itemPropertyReducedSavingThrow, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyRegeneration", 661,
			boost::bind(&ScriptFunctions::itemPropertyRegeneration, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertySkillBonus", 662,
			boost::bind(&ScriptFunctions::itemPropertySkillBonus, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertySpellImmunitySpecific", 663,
			boost::bind(&ScriptFunctions::itemPropertySpellImmunitySpecific, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertySpellImmunitySchool", 664,
			boost::bind(&ScriptFunctions::itemPropertySpellImmunitySchool, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyThievesTools", 665,
			boost::bind(&ScriptFunctions::itemPropertyThievesTools, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAttackBonus", 666,
			boost::bind(&ScriptFunctions::itemPropertyAttackBonus, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAttackBonusVsAlign", 667,
			boost::bind(&ScriptFunctions::itemPropertyAttackBonusVsAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAttackBonusVsRace", 668,
			boost::bind(&ScriptFunctions::itemPropertyAttackBonusVsRace, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAttackBonusVsSAlign", 669,
			boost::bind(&ScriptFunctions::itemPropertyAttackBonusVsSAlign, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAttackPenalty", 670,
			boost::bind(&ScriptFunctions::itemPropertyAttackPenalty, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyUnlimitedAmmo", 671,
			boost::bind(&ScriptFunctions::itemPropertyUnlimitedAmmo, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.intIPAmmo));
	FunctionMan.registerFunction("ItemPropertyLimitUseByAlign", 672,
			boost::bind(&ScriptFunctions::itemPropertyLimitUseByAlign, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyLimitUseByClass", 673,
			boost::bind(&ScriptFunctions::itemPropertyLimitUseByClass, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyLimitUseByRace", 674,
			boost::bind(&ScriptFunctions::itemPropertyLimitUseByRace, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyLimitUseBySAlign", 675,
			boost::bind(&ScriptFunctions::itemPropertyLimitUseBySAlign, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("BadBadReplaceMeThisDoesNothing", 676,
			boost::bind(&ScriptFunctions::badBadReplaceMeThisDoesNothing, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyVampiricRegeneration", 677,
			boost::bind(&ScriptFunctions::itemPropertyVampiricRegeneration, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyTrap", 678,
			boost::bind(&ScriptFunctions::itemPropertyTrap, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyTrueSeeing", 679,
			boost::bind(&ScriptFunctions::itemPropertyTrueSeeing, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyOnMonsterHitProperties", 680,
			boost::bind(&ScriptFunctions::itemPropertyOnMonsterHitProperties, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("ItemPropertyTurnResistance", 681,
			boost::bind(&ScriptFunctions::itemPropertyTurnResistance, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyMassiveCritical", 682,
			boost::bind(&ScriptFunctions::itemPropertyMassiveCritical, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyFreeAction", 683,
			boost::bind(&ScriptFunctions::itemPropertyFreeAction, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyMonsterDamage", 684,
			boost::bind(&ScriptFunctions::itemPropertyMonsterDamage, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyImmunityToSpellLevel", 685,
			boost::bind(&ScriptFunctions::itemPropertyImmunityToSpellLevel, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertySpecialWalk", 686,
			boost::bind(&ScriptFunctions::itemPropertySpecialWalk, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("ItemPropertyHealersKit", 687,
			boost::bind(&ScriptFunctions::itemPropertyHealersKit, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyWeightIncrease", 688,
			boost::bind(&ScriptFunctions::itemPropertyWeightIncrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetIsSkillSuccessful", 689,
			boost::bind(&ScriptFunctions::getIsSkillSuccessful, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectSpellFailure", 690,
			boost::bind(&ScriptFunctions::effectSpellFailure, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(2, d.int100, d.intSpellsGeneral));
	FunctionMan.registerFunction("SpeakStringByStrRef", 691,
			boost::bind(&ScriptFunctions::speakStringByStrRef, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeInt),
			createDefaults(1, d.intTalk));
	FunctionMan.registerFunction("SetCutsceneMode", 692,
			boost::bind(&ScriptFunctions::setCutsceneMode, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(2, d.int1, d.int0));
	FunctionMan.registerFunction("GetLastPCToCancelCutscene", 693,
			boost::bind(&ScriptFunctions::getLastPCToCancelCutscene, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetDialogSoundLength", 694,
			boost::bind(&ScriptFunctions::getDialogSoundLength, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("FadeFromBlack", 695,
			boost::bind(&ScriptFunctions::fadeFromBlack, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.floatMedium));
	FunctionMan.registerFunction("FadeToBlack", 696,
			boost::bind(&ScriptFunctions::fadeToBlack, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.floatMedium));
	FunctionMan.registerFunction("StopFade", 697,
			boost::bind(&ScriptFunctions::stopFade, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("BlackScreen", 698,
			boost::bind(&ScriptFunctions::blackScreen, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetBaseAttackBonus", 699,
			boost::bind(&ScriptFunctions::getBaseAttackBonus, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
}

void ScriptFunctions::copyObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: CopyObject");
}

void ScriptFunctions::deleteCampaignVariable(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: DeleteCampaignVariable: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::storeCampaignObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	Object *object = convertObject(ctx.getParams()[3].getObject());
	if (!object)
		return;

	warning("TODO: StoreCampaignObject: \"%s\":\"%s\" to \"%s\"",
			dbName.c_str(), varName.c_str(), object->getTag().c_str());
}

void ScriptFunctions::retrieveCampaignObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	// Location *loc = convertLocation(ctx.getParams()[2].getEngineType());

	warning("TODO: RetrieveCampaignObject: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::effectCutsceneDominated(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectCutsceneDominated");
}

void ScriptFunctions::getItemStackSize(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetItemStackSize");
}

void ScriptFunctions::setItemStackSize(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetItemStackSize");
}

void ScriptFunctions::getItemCharges(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetItemCharges");
}

void ScriptFunctions::setItemCharges(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetItemCharges");
}

void ScriptFunctions::addItemProperty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AddItemProperty");
}

void ScriptFunctions::removeItemProperty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RemoveItemProperty");
}

void ScriptFunctions::getIsItemPropertyValid(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsItemPropertyValid");
}

void ScriptFunctions::getFirstItemProperty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFirstItemProperty");
}

void ScriptFunctions::getNextItemProperty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNextItemProperty");
}

void ScriptFunctions::getItemPropertyType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetItemPropertyType");
}

void ScriptFunctions::getItemPropertyDurationType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetItemPropertyDurationType");
}

void ScriptFunctions::itemPropertyAbilityBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAbilityBonus");
}

void ScriptFunctions::itemPropertyACBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyACBonus");
}

void ScriptFunctions::itemPropertyACBonusVsAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyACBonusVsAlign");
}

void ScriptFunctions::itemPropertyACBonusVsDmgType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyACBonusVsDmgType");
}

void ScriptFunctions::itemPropertyACBonusVsRace(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyACBonusVsRace");
}

void ScriptFunctions::itemPropertyACBonusVsSAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyACBonusVsSAlign");
}

void ScriptFunctions::itemPropertyEnhancementBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyEnhancementBonus");
}

void ScriptFunctions::itemPropertyEnhancementBonusVsAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyEnhancementBonusVsAlign");
}

void ScriptFunctions::itemPropertyEnhancementBonusVsRace(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyEnhancementBonusVsRace");
}

void ScriptFunctions::itemPropertyEnhancementBonusVsSAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyEnhancementBonusVsSAlign");
}

void ScriptFunctions::itemPropertyEnhancementPenalty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyEnhancementPenalty");
}

void ScriptFunctions::itemPropertyWeightReduction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyWeightReduction");
}

void ScriptFunctions::itemPropertyBonusFeat(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyBonusFeat");
}

void ScriptFunctions::itemPropertyBonusLevelSpell(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyBonusLevelSpell");
}

void ScriptFunctions::itemPropertyCastSpell(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyCastSpell");
}

void ScriptFunctions::itemPropertyDamageBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageBonus");
}

void ScriptFunctions::itemPropertyDamageBonusVsAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageBonusVsAlign");
}

void ScriptFunctions::itemPropertyDamageBonusVsRace(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageBonusVsRace");
}

void ScriptFunctions::itemPropertyDamageBonusVsSAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageBonusVsSAlign");
}

void ScriptFunctions::itemPropertyDamageImmunity(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageImmunity");
}

void ScriptFunctions::itemPropertyDamagePenalty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamagePenalty");
}

void ScriptFunctions::itemPropertyDamageReduction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageReduction");
}

void ScriptFunctions::itemPropertyDamageResistance(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageResistance");
}

void ScriptFunctions::itemPropertyDamageVulnerability(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDamageVulnerability");
}

void ScriptFunctions::itemPropertyDarkvision(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDarkvision");
}

void ScriptFunctions::itemPropertyDecreaseAbility(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDecreaseAbility");
}

void ScriptFunctions::itemPropertyDecreaseAC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDecreaseAC");
}

void ScriptFunctions::itemPropertyDecreaseSkill(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyDecreaseSkill");
}

void ScriptFunctions::itemPropertyContainerReducedWeight(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyContainerReducedWeight");
}

void ScriptFunctions::itemPropertyExtraMeleeDamageType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyExtraMeleeDamageType");
}

void ScriptFunctions::itemPropertyExtraRangeDamageType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyExtraRangeDamageType");
}

void ScriptFunctions::itemPropertyHaste(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyHaste");
}

void ScriptFunctions::itemPropertyHolyAvenger(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyHolyAvenger");
}

void ScriptFunctions::itemPropertyImmunityMisc(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyImmunityMisc");
}

void ScriptFunctions::itemPropertyImprovedEvasion(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyImprovedEvasion");
}

void ScriptFunctions::itemPropertyBonusSpellResistance(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyBonusSpellResistance");
}

void ScriptFunctions::itemPropertyBonusSavingThrowVsX(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyBonusSavingThrowVsX");
}

void ScriptFunctions::itemPropertyBonusSavingThrow(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyBonusSavingThrow");
}

void ScriptFunctions::itemPropertyKeen(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyKeen");
}

void ScriptFunctions::itemPropertyLight(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyLight");
}

void ScriptFunctions::itemPropertyMaxRangeStrengthMod(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyMaxRangeStrengthMod");
}

void ScriptFunctions::itemPropertyNoDamage(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyNoDamage");
}

void ScriptFunctions::itemPropertyOnHitProps(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyOnHitProps");
}

void ScriptFunctions::itemPropertyReducedSavingThrowVsX(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyReducedSavingThrowVsX");
}

void ScriptFunctions::itemPropertyReducedSavingThrow(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyReducedSavingThrow");
}

void ScriptFunctions::itemPropertyRegeneration(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyRegeneration");
}

void ScriptFunctions::itemPropertySkillBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertySkillBonus");
}

void ScriptFunctions::itemPropertySpellImmunitySpecific(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertySpellImmunitySpecific");
}

void ScriptFunctions::itemPropertySpellImmunitySchool(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertySpellImmunitySchool");
}

void ScriptFunctions::itemPropertyThievesTools(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyThievesTools");
}

void ScriptFunctions::itemPropertyAttackBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAttackBonus");
}

void ScriptFunctions::itemPropertyAttackBonusVsAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAttackBonusVsAlign");
}

void ScriptFunctions::itemPropertyAttackBonusVsRace(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAttackBonusVsRace");
}

void ScriptFunctions::itemPropertyAttackBonusVsSAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAttackBonusVsSAlign");
}

void ScriptFunctions::itemPropertyAttackPenalty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyAttackPenalty");
}

void ScriptFunctions::itemPropertyUnlimitedAmmo(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyUnlimitedAmmo");
}

void ScriptFunctions::itemPropertyLimitUseByAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyLimitUseByAlign");
}

void ScriptFunctions::itemPropertyLimitUseByClass(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyLimitUseByClass");
}

void ScriptFunctions::itemPropertyLimitUseByRace(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyLimitUseByRace");
}

void ScriptFunctions::itemPropertyLimitUseBySAlign(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyLimitUseBySAlign");
}

void ScriptFunctions::badBadReplaceMeThisDoesNothing(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: BadBadReplaceMeThisDoesNothing");
}

void ScriptFunctions::itemPropertyVampiricRegeneration(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyVampiricRegeneration");
}

void ScriptFunctions::itemPropertyTrap(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyTrap");
}

void ScriptFunctions::itemPropertyTrueSeeing(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyTrueSeeing");
}

void ScriptFunctions::itemPropertyOnMonsterHitProperties(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyOnMonsterHitProperties");
}

void ScriptFunctions::itemPropertyTurnResistance(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyTurnResistance");
}

void ScriptFunctions::itemPropertyMassiveCritical(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyMassiveCritical");
}

void ScriptFunctions::itemPropertyFreeAction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyFreeAction");
}

void ScriptFunctions::itemPropertyMonsterDamage(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyMonsterDamage");
}

void ScriptFunctions::itemPropertyImmunityToSpellLevel(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyImmunityToSpellLevel");
}

void ScriptFunctions::itemPropertySpecialWalk(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertySpecialWalk");
}

void ScriptFunctions::itemPropertyHealersKit(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyHealersKit");
}

void ScriptFunctions::itemPropertyWeightIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ItemPropertyWeightIncrease");
}

void ScriptFunctions::getIsSkillSuccessful(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsSkillSuccessful");
}

void ScriptFunctions::effectSpellFailure(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSpellFailure");
}

void ScriptFunctions::speakStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	// TODO: ScriptFunctions::speakStringByStrRef(): Volume
	uint32 strRef = (uint32) ctx.getParams()[0].getInt();
	uint32 volume = (uint32) ctx.getParams()[1].getInt();

	object->speakString(TalkMan.getString(strRef).c_str(), volume);
}

void ScriptFunctions::setCutsceneMode(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetCutsceneMode");
}

void ScriptFunctions::getLastPCToCancelCutscene(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPCToCancelCutscene");
}

void ScriptFunctions::getDialogSoundLength(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDialogSoundLength");
}

void ScriptFunctions::fadeFromBlack(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FadeFromBlack");
}

void ScriptFunctions::fadeToBlack(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FadeToBlack");
}

void ScriptFunctions::stopFade(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: StopFade");
}

void ScriptFunctions::blackScreen(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: BlackScreen");
}

void ScriptFunctions::getBaseAttackBonus(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetBaseAttackBonus");
}

} // End of namespace NWN

} // End of namespace Engines
