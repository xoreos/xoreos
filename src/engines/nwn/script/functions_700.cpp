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

/** @file engines/nwn/script/functions_700.cpp
 *  NWN script functions, 700-799.
 */

#include <boost/bind.hpp>

#include "common/util.h"
#include "common/error.h"

#include "aurora/talkman.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "engines/nwn/types.h"
#include "engines/nwn/area.h"
#include "engines/nwn/object.h"

#include "engines/nwn/script/functions.h"

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

void ScriptFunctions::registerFunctions700(const Defaults &d) {
	FunctionMan.registerFunction("SetImmortal", 700,
			boost::bind(&ScriptFunctions::setImmortal, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("OpenInventory", 701,
			boost::bind(&ScriptFunctions::openInventory, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("StoreCameraFacing", 702,
			boost::bind(&ScriptFunctions::storeCameraFacing, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("RestoreCameraFacing", 703,
			boost::bind(&ScriptFunctions::restoreCameraFacing, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("LevelUpHenchman", 704,
			boost::bind(&ScriptFunctions::levelUpHenchman, this, _1),
			createSignature(5, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(3, d.intClassInvalid, d.int0, d.intPackageInvalid));
	FunctionMan.registerFunction("SetDroppableFlag", 705,
			boost::bind(&ScriptFunctions::setDroppableFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetWeight", 706,
			boost::bind(&ScriptFunctions::getWeight, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetModuleItemAcquiredBy", 707,
			boost::bind(&ScriptFunctions::getModuleItemAcquiredBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetImmortal", 708,
			boost::bind(&ScriptFunctions::getImmortal, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("DoWhirlwindAttack", 709,
			boost::bind(&ScriptFunctions::doWhirlwindAttack, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeInt),
			createDefaults(2, d.int1, d.int0));
	FunctionMan.registerFunction("Get2DAString", 710,
			boost::bind(&ScriptFunctions::get2DAString, this, _1),
			createSignature(4, kTypeString, kTypeString, kTypeString, kTypeInt));
	FunctionMan.registerFunction("EffectEthereal", 711,
			boost::bind(&ScriptFunctions::effectEthereal, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetAILevel", 712,
			boost::bind(&ScriptFunctions::getAILevel, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetAILevel", 713,
			boost::bind(&ScriptFunctions::setAILevel, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetIsPossessedFamiliar", 714,
			boost::bind(&ScriptFunctions::getIsPossessedFamiliar, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("UnpossessFamiliar", 715,
			boost::bind(&ScriptFunctions::unpossessFamiliar, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetIsAreaInterior", 716,
			boost::bind(&ScriptFunctions::getIsAreaInterior, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SendMessageToPCByStrRef", 717,
			boost::bind(&ScriptFunctions::sendMessageToPCByStrRef, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("IncrementRemainingFeatUses", 718,
			boost::bind(&ScriptFunctions::incrementRemainingFeatUses, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("ExportSingleCharacter", 719,
			boost::bind(&ScriptFunctions::exportSingleCharacter, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("PlaySoundByStrRef", 720,
			boost::bind(&ScriptFunctions::playSoundByStrRef, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetSubRace", 721,
			boost::bind(&ScriptFunctions::setSubRace, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("SetDeity", 722,
			boost::bind(&ScriptFunctions::setDeity, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetIsDMPossessed", 723,
			boost::bind(&ScriptFunctions::getIsDMPossessed, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetWeather", 724,
			boost::bind(&ScriptFunctions::getWeather, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetIsAreaNatural", 725,
			boost::bind(&ScriptFunctions::getIsAreaNatural, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetIsAreaAboveGround", 726,
			boost::bind(&ScriptFunctions::getIsAreaAboveGround, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetPCItemLastEquipped", 727,
			boost::bind(&ScriptFunctions::getPCItemLastEquipped, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetPCItemLastEquippedBy", 728,
			boost::bind(&ScriptFunctions::getPCItemLastEquippedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetPCItemLastUnequipped", 729,
			boost::bind(&ScriptFunctions::getPCItemLastUnequipped, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetPCItemLastUnequippedBy", 730,
			boost::bind(&ScriptFunctions::getPCItemLastUnequippedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("CopyItemAndModify", 731,
			boost::bind(&ScriptFunctions::copyItemAndModify, this, _1),
			createSignature(6, kTypeObject, kTypeObject, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetItemAppearance", 732,
			boost::bind(&ScriptFunctions::getItemAppearance, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyOnHitCastSpell", 733,
			boost::bind(&ScriptFunctions::itemPropertyOnHitCastSpell, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetItemPropertySubType", 734,
			boost::bind(&ScriptFunctions::getItemPropertySubType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetActionMode", 735,
			boost::bind(&ScriptFunctions::getActionMode, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetActionMode", 736,
			boost::bind(&ScriptFunctions::setActionMode, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetArcaneSpellFailure", 737,
			boost::bind(&ScriptFunctions::getArcaneSpellFailure, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("ActionExamine", 738,
			boost::bind(&ScriptFunctions::actionExamine, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ItemPropertyVisualEffect", 739,
			boost::bind(&ScriptFunctions::itemPropertyVisualEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("SetLootable", 740,
			boost::bind(&ScriptFunctions::setLootable, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetLootable", 741,
			boost::bind(&ScriptFunctions::getLootable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetCutsceneCameraMoveRate", 742,
			boost::bind(&ScriptFunctions::getCutsceneCameraMoveRate, this, _1),
			createSignature(2, kTypeFloat, kTypeObject));
	FunctionMan.registerFunction("SetCutsceneCameraMoveRate", 743,
			boost::bind(&ScriptFunctions::setCutsceneCameraMoveRate, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat));
	FunctionMan.registerFunction("GetItemCursedFlag", 744,
			boost::bind(&ScriptFunctions::getItemCursedFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetItemCursedFlag", 745,
			boost::bind(&ScriptFunctions::setItemCursedFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetMaxHenchmen", 746,
			boost::bind(&ScriptFunctions::setMaxHenchmen, this, _1),
			createSignature(2, kTypeVoid, kTypeInt));
	FunctionMan.registerFunction("GetMaxHenchmen", 747,
			boost::bind(&ScriptFunctions::getMaxHenchmen, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetAssociateType", 748,
			boost::bind(&ScriptFunctions::getAssociateType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetSpellResistance", 749,
			boost::bind(&ScriptFunctions::getSpellResistance, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("DayToNight", 750,
			boost::bind(&ScriptFunctions::dayToNight, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("NightToDay", 751,
			boost::bind(&ScriptFunctions::nightToDay, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("LineOfSightObject", 752,
			boost::bind(&ScriptFunctions::lineOfSightObject, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("LineOfSightVector", 753,
			boost::bind(&ScriptFunctions::lineOfSightVector, this, _1),
			createSignature(3, kTypeInt, kTypeVector, kTypeVector));
	FunctionMan.registerFunction("GetLastSpellCastClass", 754,
			boost::bind(&ScriptFunctions::getLastSpellCastClass, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("SetBaseAttackBonus", 755,
			boost::bind(&ScriptFunctions::setBaseAttackBonus, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("RestoreBaseAttackBonus", 756,
			boost::bind(&ScriptFunctions::restoreBaseAttackBonus, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectCutsceneGhost", 757,
			boost::bind(&ScriptFunctions::effectCutsceneGhost, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ItemPropertyArcaneSpellFailure", 758,
			boost::bind(&ScriptFunctions::itemPropertyArcaneSpellFailure, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetStoreGold", 759,
			boost::bind(&ScriptFunctions::getStoreGold, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetStoreGold", 760,
			boost::bind(&ScriptFunctions::setStoreGold, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetStoreMaxBuyPrice", 761,
			boost::bind(&ScriptFunctions::getStoreMaxBuyPrice, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetStoreMaxBuyPrice", 762,
			boost::bind(&ScriptFunctions::setStoreMaxBuyPrice, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetStoreIdentifyCost", 763,
			boost::bind(&ScriptFunctions::getStoreIdentifyCost, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetStoreIdentifyCost", 764,
			boost::bind(&ScriptFunctions::setStoreIdentifyCost, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetCreatureAppearanceType", 765,
			boost::bind(&ScriptFunctions::setCreatureAppearanceType, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetCreatureStartingPackage", 766,
			boost::bind(&ScriptFunctions::getCreatureStartingPackage, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("EffectCutsceneImmobilize", 767,
			boost::bind(&ScriptFunctions::effectCutsceneImmobilize, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetIsInSubArea", 768,
			boost::bind(&ScriptFunctions::getIsInSubArea, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetItemPropertyCostTable", 769,
			boost::bind(&ScriptFunctions::getItemPropertyCostTable, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetItemPropertyCostTableValue", 770,
			boost::bind(&ScriptFunctions::getItemPropertyCostTableValue, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetItemPropertyParam1", 771,
			boost::bind(&ScriptFunctions::getItemPropertyParam1, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetItemPropertyParam1Value", 772,
			boost::bind(&ScriptFunctions::getItemPropertyParam1Value, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetIsCreatureDisarmable", 773,
			boost::bind(&ScriptFunctions::getIsCreatureDisarmable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetStolenFlag", 774,
			boost::bind(&ScriptFunctions::setStolenFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("ForceRest", 775,
			boost::bind(&ScriptFunctions::forceRest, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("SetCameraHeight", 776,
			boost::bind(&ScriptFunctions::setCameraHeight, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("SetSkyBox", 777,
			boost::bind(&ScriptFunctions::setSkyBox, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetPhenoType", 778,
			boost::bind(&ScriptFunctions::getPhenoType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetPhenoType", 779,
			boost::bind(&ScriptFunctions::setPhenoType, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetFogColor", 780,
			boost::bind(&ScriptFunctions::setFogColor, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCutsceneMode", 781,
			boost::bind(&ScriptFunctions::getCutsceneMode, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetSkyBox", 782,
			boost::bind(&ScriptFunctions::getSkyBox, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetFogColor", 783,
			boost::bind(&ScriptFunctions::getFogColor, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetFogAmount", 784,
			boost::bind(&ScriptFunctions::setFogAmount, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetFogAmount", 785,
			boost::bind(&ScriptFunctions::getFogAmount, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetPickpocketableFlag", 786,
			boost::bind(&ScriptFunctions::getPickpocketableFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetPickpocketableFlag", 787,
			boost::bind(&ScriptFunctions::setPickpocketableFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetFootstepType", 788,
			boost::bind(&ScriptFunctions::getFootstepType, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetFootstepType", 789,
			boost::bind(&ScriptFunctions::setFootstepType, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureWingType", 790,
			boost::bind(&ScriptFunctions::getCreatureWingType, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCreatureWingType", 791,
			boost::bind(&ScriptFunctions::setCreatureWingType, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureBodyPart", 792,
			boost::bind(&ScriptFunctions::getCreatureBodyPart, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCreatureBodyPart", 793,
			boost::bind(&ScriptFunctions::setCreatureBodyPart, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureTailType", 794,
			boost::bind(&ScriptFunctions::getCreatureTailType, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCreatureTailType", 795,
			boost::bind(&ScriptFunctions::setCreatureTailType, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetHardness", 796,
			boost::bind(&ScriptFunctions::getHardness, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetHardness", 797,
			boost::bind(&ScriptFunctions::setHardness, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetLockKeyRequired", 798,
			boost::bind(&ScriptFunctions::setLockKeyRequired, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetLockKeyTag", 799,
			boost::bind(&ScriptFunctions::setLockKeyTag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
}

void ScriptFunctions::setImmortal(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetImmortal");
}

void ScriptFunctions::openInventory(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: OpenInventory");
}

void ScriptFunctions::storeCameraFacing(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: StoreCameraFacing");
}

void ScriptFunctions::restoreCameraFacing(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: RestoreCameraFacing");
}

void ScriptFunctions::levelUpHenchman(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LevelUpHenchman");
}

void ScriptFunctions::setDroppableFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetDroppableFlag");
}

void ScriptFunctions::getWeight(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetWeight");
}

void ScriptFunctions::getModuleItemAcquiredBy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetModuleItemAcquiredBy");
}

void ScriptFunctions::getImmortal(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetImmortal");
}

void ScriptFunctions::doWhirlwindAttack(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: DoWhirlwindAttack");
}

void ScriptFunctions::get2DAString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &file =          ctx.getParams()[0].getString();
	const Common::UString &col  =          ctx.getParams()[1].getString();
	const uint32           row  = (uint32) ctx.getParams()[2].getInt();

	if (file.empty() || col.empty())
		return;

	const Aurora::TwoDAFile &twoda = TwoDAReg.get(file);

	ctx.getReturn() = twoda.getRow(row).getString(col);
}

void ScriptFunctions::effectEthereal(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectEthereal");
}

void ScriptFunctions::getAILevel(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAILevel");
}

void ScriptFunctions::setAILevel(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetAILevel");
}

void ScriptFunctions::getIsPossessedFamiliar(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsPossessedFamiliar");
}

void ScriptFunctions::unpossessFamiliar(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: UnpossessFamiliar");
}

void ScriptFunctions::getIsAreaInterior(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsAreaInterior");
}

void ScriptFunctions::sendMessageToPCByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SendMessageToPCByStrRef");
}

void ScriptFunctions::incrementRemainingFeatUses(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: IncrementRemainingFeatUses");
}

void ScriptFunctions::exportSingleCharacter(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ExportSingleCharacter");
}

void ScriptFunctions::playSoundByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	uint32 strRef = (uint32) ctx.getParams()[0].getInt();

	// TODO: ScriptFunctions::playSoundByStrRef(): Run as action
	// bool runAsAction = ctx.getParams()[1].getInt() != 0;

	object->playSound(TalkMan.getSoundResRef(strRef));
}

void ScriptFunctions::setSubRace(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetSubRace");
}

void ScriptFunctions::setDeity(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetDeity");
}

void ScriptFunctions::getIsDMPossessed(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsDMPossessed");
}

void ScriptFunctions::getWeather(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetWeather");
}

void ScriptFunctions::getIsAreaNatural(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsAreaNatural");
}

void ScriptFunctions::getIsAreaAboveGround(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsAreaAboveGround");
}

void ScriptFunctions::getPCItemLastEquipped(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCItemLastEquipped");
}

void ScriptFunctions::getPCItemLastEquippedBy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCItemLastEquippedBy");
}

void ScriptFunctions::getPCItemLastUnequipped(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCItemLastUnequipped");
}

void ScriptFunctions::getPCItemLastUnequippedBy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCItemLastUnequippedBy");
}

void ScriptFunctions::copyItemAndModify(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: CopyItemAndModify");
}

void ScriptFunctions::getItemAppearance(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemAppearance");
}

void ScriptFunctions::itemPropertyOnHitCastSpell(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyOnHitCastSpell");
}

void ScriptFunctions::getItemPropertySubType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemPropertySubType");
}

void ScriptFunctions::getActionMode(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetActionMode");
}

void ScriptFunctions::setActionMode(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetActionMode");
}

void ScriptFunctions::getArcaneSpellFailure(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetArcaneSpellFailure");
}

void ScriptFunctions::actionExamine(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionExamine");
}

void ScriptFunctions::itemPropertyVisualEffect(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyVisualEffect");
}

void ScriptFunctions::setLootable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLootable");
}

void ScriptFunctions::getLootable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLootable");
}

void ScriptFunctions::getCutsceneCameraMoveRate(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCutsceneCameraMoveRate");
}

void ScriptFunctions::setCutsceneCameraMoveRate(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCutsceneCameraMoveRate");
}

void ScriptFunctions::getItemCursedFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemCursedFlag");
}

void ScriptFunctions::setItemCursedFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetItemCursedFlag");
}

void ScriptFunctions::setMaxHenchmen(Aurora::NWScript::FunctionContext &ctx) {
	int n = ctx.getParams()[0].getInt();

	warning("TODO: SetMaxHenchmen: %d", n);
}

void ScriptFunctions::getMaxHenchmen(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetMaxHenchmen");
	ctx.getReturn() = 0;
}

void ScriptFunctions::getAssociateType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAssociateType");
}

void ScriptFunctions::getSpellResistance(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetSpellResistance");
}

void ScriptFunctions::dayToNight(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: DayToNight");
}

void ScriptFunctions::nightToDay(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: NightToDay");
}

void ScriptFunctions::lineOfSightObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LineOfSightObject");
}

void ScriptFunctions::lineOfSightVector(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LineOfSightVector");
}

void ScriptFunctions::getLastSpellCastClass(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastSpellCastClass");
}

void ScriptFunctions::setBaseAttackBonus(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetBaseAttackBonus");
}

void ScriptFunctions::restoreBaseAttackBonus(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: RestoreBaseAttackBonus");
}

void ScriptFunctions::effectCutsceneGhost(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectCutsceneGhost");
}

void ScriptFunctions::itemPropertyArcaneSpellFailure(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyArcaneSpellFailure");
}

void ScriptFunctions::getStoreGold(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetStoreGold: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::setStoreGold(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	int32 gold = ctx.getParams()[1].getInt();

	warning("TODO: SetStoreGold: \"%s\", %d", object->getTag().c_str(), gold);
}

void ScriptFunctions::getStoreMaxBuyPrice(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetStoreMaxBuyPrice: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::setStoreMaxBuyPrice(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	int32 gold = ctx.getParams()[1].getInt();

	warning("TODO: GetStoreMaxBuyPrice: \"%s\", %d", object->getTag().c_str(), gold);
}

void ScriptFunctions::getStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetStoreIdentifyCost: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::setStoreIdentifyCost(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	int32 gold = ctx.getParams()[1].getInt();

	warning("TODO: SetStoreIdentifyCost: \"%s\", %d", object->getTag().c_str(), gold);
}

void ScriptFunctions::setCreatureAppearanceType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCreatureAppearanceType");
}

void ScriptFunctions::getCreatureStartingPackage(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureStartingPackage");
}

void ScriptFunctions::effectCutsceneImmobilize(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectCutsceneImmobilize");
}

void ScriptFunctions::getIsInSubArea(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsInSubArea");
}

void ScriptFunctions::getItemPropertyCostTable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemPropertyCostTable");
}

void ScriptFunctions::getItemPropertyCostTableValue(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemPropertyCostTableValue");
}

void ScriptFunctions::getItemPropertyParam1(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemPropertyParam1");
}

void ScriptFunctions::getItemPropertyParam1Value(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemPropertyParam1Value");
}

void ScriptFunctions::getIsCreatureDisarmable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsCreatureDisarmable");
}

void ScriptFunctions::setStolenFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetStolenFlag");
}

void ScriptFunctions::forceRest(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ForceRest");
}

void ScriptFunctions::setCameraHeight(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCameraHeight");
}

void ScriptFunctions::setSkyBox(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetSkyBox");
}

void ScriptFunctions::getPhenoType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPhenoType");
}

void ScriptFunctions::setPhenoType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPhenoType");
}

void ScriptFunctions::setFogColor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetFogColor");
}

void ScriptFunctions::getCutsceneMode(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCutsceneMode");
}

void ScriptFunctions::getSkyBox(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetSkyBox");
}

void ScriptFunctions::getFogColor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFogColor");
}

void ScriptFunctions::setFogAmount(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetFogAmount");
}

void ScriptFunctions::getFogAmount(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFogAmount");
}

void ScriptFunctions::getPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPickpocketableFlag");
}

void ScriptFunctions::setPickpocketableFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPickpocketableFlag");
}

void ScriptFunctions::getFootstepType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFootstepType");
}

void ScriptFunctions::setFootstepType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetFootstepType");
}

void ScriptFunctions::getCreatureWingType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureWingType");
}

void ScriptFunctions::setCreatureWingType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCreatureWingType");
}

void ScriptFunctions::getCreatureBodyPart(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureBodyPart");
}

void ScriptFunctions::setCreatureBodyPart(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCreatureBodyPart");
}

void ScriptFunctions::getCreatureTailType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureTailType");
}

void ScriptFunctions::setCreatureTailType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetCreatureTailType");
}

void ScriptFunctions::getHardness(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetHardness");
}

void ScriptFunctions::setHardness(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetHardness");
}

void ScriptFunctions::setLockKeyRequired(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLockKeyRequired");
}

void ScriptFunctions::setLockKeyTag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLockKeyTag");
}

} // End of namespace NWN

} // End of namespace Engines
