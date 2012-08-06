/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/script/functions_300.cpp
 *  NWN script functions, 300-399.
 */

#include <boost/bind.hpp>

#include "common/util.h"
#include "common/error.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "engines/nwn/types.h"
#include "engines/nwn/object.h"
#include "engines/nwn/situated.h"
#include "engines/nwn/creature.h"

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

void ScriptFunctions::registerFunctions300(const Defaults &d) {
	FunctionMan.registerFunction("PlayAnimation", 300,
			boost::bind(&ScriptFunctions::playAnimation, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeFloat, kTypeFloat),
			createDefaults(2, d.float1_0, d.float0_0));
	FunctionMan.registerFunction("TalentSpell", 301,
			boost::bind(&ScriptFunctions::talentSpell, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("TalentFeat", 302,
			boost::bind(&ScriptFunctions::talentFeat, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("TalentSkill", 303,
			boost::bind(&ScriptFunctions::talentSkill, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetHasSpellEffect", 304,
			boost::bind(&ScriptFunctions::getHasSpellEffect, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetEffectSpellId", 305,
			boost::bind(&ScriptFunctions::getEffectSpellId, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetCreatureHasTalent", 306,
			boost::bind(&ScriptFunctions::getCreatureHasTalent, this, _1),
			createSignature(3, kTypeInt, kTypeEngineType, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureTalentRandom", 307,
			boost::bind(&ScriptFunctions::getCreatureTalentRandom, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureTalentBest", 308,
			boost::bind(&ScriptFunctions::getCreatureTalentBest, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("ActionUseTalentOnObject", 309,
			boost::bind(&ScriptFunctions::actionUseTalentOnObject, this, _1),
			createSignature(3, kTypeVoid, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("ActionUseTalentAtLocation", 310,
			boost::bind(&ScriptFunctions::actionUseTalentAtLocation, this, _1),
			createSignature(3, kTypeVoid, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("GetGoldPieceValue", 311,
			boost::bind(&ScriptFunctions::getGoldPieceValue, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetIsPlayableRacialType", 312,
			boost::bind(&ScriptFunctions::getIsPlayableRacialType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("JumpToLocation", 313,
			boost::bind(&ScriptFunctions::jumpToLocation, this, _1),
			createSignature(2, kTypeVoid, kTypeEngineType));
	FunctionMan.registerFunction("EffectTemporaryHitpoints", 314,
			boost::bind(&ScriptFunctions::effectTemporaryHitpoints, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetSkillRank", 315,
			boost::bind(&ScriptFunctions::getSkillRank, this, _1),
			createSignature(4, kTypeInt, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int0));
	FunctionMan.registerFunction("GetAttackTarget", 316,
			boost::bind(&ScriptFunctions::getAttackTarget, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastAttackType", 317,
			boost::bind(&ScriptFunctions::getLastAttackType, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastAttackMode", 318,
			boost::bind(&ScriptFunctions::getLastAttackMode, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetMaster", 319,
			boost::bind(&ScriptFunctions::getMaster, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsInCombat", 320,
			boost::bind(&ScriptFunctions::getIsInCombat, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastAssociateCommand", 321,
			boost::bind(&ScriptFunctions::getLastAssociateCommand, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GiveGoldToCreature", 322,
			boost::bind(&ScriptFunctions::giveGoldToCreature, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetIsDestroyable", 323,
			boost::bind(&ScriptFunctions::setIsDestroyable, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(2, d.int1, d.int0));
	FunctionMan.registerFunction("SetLocked", 324,
			boost::bind(&ScriptFunctions::setLocked, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetLocked", 325,
			boost::bind(&ScriptFunctions::getLocked, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetClickingObject", 326,
			boost::bind(&ScriptFunctions::getClickingObject, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("SetAssociateListenPatterns", 327,
			boost::bind(&ScriptFunctions::setAssociateListenPatterns, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastWeaponUsed", 328,
			boost::bind(&ScriptFunctions::getLastWeaponUsed, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("ActionInteractObject", 329,
			boost::bind(&ScriptFunctions::actionInteractObject, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetLastUsedBy", 330,
			boost::bind(&ScriptFunctions::getLastUsedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetAbilityModifier", 331,
			boost::bind(&ScriptFunctions::getAbilityModifier, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIdentified", 332,
			boost::bind(&ScriptFunctions::getIdentified, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetIdentified", 333,
			boost::bind(&ScriptFunctions::setIdentified, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SummonAnimalCompanion", 334,
			boost::bind(&ScriptFunctions::summonAnimalCompanion, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SummonFamiliar", 335,
			boost::bind(&ScriptFunctions::summonFamiliar, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetBlockingDoor", 336,
			boost::bind(&ScriptFunctions::getBlockingDoor, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetIsDoorActionPossible", 337,
			boost::bind(&ScriptFunctions::getIsDoorActionPossible, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("DoDoorAction", 338,
			boost::bind(&ScriptFunctions::doDoorAction, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetFirstItemInInventory", 339,
			boost::bind(&ScriptFunctions::getFirstItemInInventory, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetNextItemInInventory", 340,
			boost::bind(&ScriptFunctions::getNextItemInInventory, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetClassByPosition", 341,
			boost::bind(&ScriptFunctions::getClassByPosition, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLevelByPosition", 342,
			boost::bind(&ScriptFunctions::getLevelByPosition, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLevelByClass", 343,
			boost::bind(&ScriptFunctions::getLevelByClass, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetDamageDealtByType", 344,
			boost::bind(&ScriptFunctions::getDamageDealtByType, this, _1),
			createSignature(2, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetTotalDamageDealt", 345,
			boost::bind(&ScriptFunctions::getTotalDamageDealt, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetLastDamager", 346,
			boost::bind(&ScriptFunctions::getLastDamager, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastDisarmed", 347,
			boost::bind(&ScriptFunctions::getLastDisarmed, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastDisturbed", 348,
			boost::bind(&ScriptFunctions::getLastDisturbed, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastLocked", 349,
			boost::bind(&ScriptFunctions::getLastLocked, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastUnlocked", 350,
			boost::bind(&ScriptFunctions::getLastUnlocked, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("EffectSkillIncrease", 351,
			boost::bind(&ScriptFunctions::effectSkillIncrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetInventoryDisturbType", 352,
			boost::bind(&ScriptFunctions::getInventoryDisturbType, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetInventoryDisturbItem", 353,
			boost::bind(&ScriptFunctions::getInventoryDisturbItem, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetHenchman", 354,
			boost::bind(&ScriptFunctions::getHenchman, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("VersusAlignmentEffect", 355,
			boost::bind(&ScriptFunctions::versusAlignmentEffect, this, _1),
			createSignature(4, kTypeEngineType, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(2, d.intAlignAll, d.intAlignAll));
	FunctionMan.registerFunction("VersusRacialTypeEffect", 356,
			boost::bind(&ScriptFunctions::versusRacialTypeEffect, this, _1),
			createSignature(3, kTypeEngineType, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("VersusTrapEffect", 357,
			boost::bind(&ScriptFunctions::versusTrapEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("GetGender", 358,
			boost::bind(&ScriptFunctions::getGender, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetIsTalentValid", 359,
			boost::bind(&ScriptFunctions::getIsTalentValid, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("ActionMoveAwayFromLocation", 360,
			boost::bind(&ScriptFunctions::actionMoveAwayFromLocation, this, _1),
			createSignature(4, kTypeVoid, kTypeEngineType, kTypeInt, kTypeFloat),
			createDefaults(2, d.int0, d.float40_0));
	FunctionMan.registerFunction("GetAttemptedAttackTarget", 361,
			boost::bind(&ScriptFunctions::getAttemptedAttackTarget, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetTypeFromTalent", 362,
			boost::bind(&ScriptFunctions::getTypeFromTalent, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetIdFromTalent", 363,
			boost::bind(&ScriptFunctions::getIdFromTalent, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetAssociate", 364,
			boost::bind(&ScriptFunctions::getAssociate, this, _1),
			createSignature(4, kTypeObject, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("AddHenchman", 365,
			boost::bind(&ScriptFunctions::addHenchman, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("RemoveHenchman", 366,
			boost::bind(&ScriptFunctions::removeHenchman, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("AddJournalQuestEntry", 367,
			boost::bind(&ScriptFunctions::addJournalQuestEntry, this, _1),
			createSignature(7, kTypeVoid, kTypeString, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(3, d.int1, d.int0, d.int0));
	FunctionMan.registerFunction("RemoveJournalQuestEntry", 368,
			boost::bind(&ScriptFunctions::removeJournalQuestEntry, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(2, d.int1, d.int0));
	FunctionMan.registerFunction("GetPCPublicCDKey", 369,
			boost::bind(&ScriptFunctions::getPCPublicCDKey, this, _1),
			createSignature(3, kTypeString, kTypeObject, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetPCIPAddress", 370,
			boost::bind(&ScriptFunctions::getPCIPAddress, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetPCPlayerName", 371,
			boost::bind(&ScriptFunctions::getPCPlayerName, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("SetPCLike", 372,
			boost::bind(&ScriptFunctions::setPCLike, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("SetPCDislike", 373,
			boost::bind(&ScriptFunctions::setPCDislike, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("SendMessageToPC", 374,
			boost::bind(&ScriptFunctions::sendMessageToPC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetAttemptedSpellTarget", 375,
			boost::bind(&ScriptFunctions::getAttemptedSpellTarget, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastOpenedBy", 376,
			boost::bind(&ScriptFunctions::getLastOpenedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetHasSpell", 377,
			boost::bind(&ScriptFunctions::getHasSpell, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("OpenStore", 378,
			boost::bind(&ScriptFunctions::openStore, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(2, d.int0, d.int0));
	FunctionMan.registerFunction("EffectTurned", 379,
			boost::bind(&ScriptFunctions::effectTurned, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetFirstFactionMember", 380,
			boost::bind(&ScriptFunctions::getFirstFactionMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetNextFactionMember", 381,
			boost::bind(&ScriptFunctions::getNextFactionMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("ActionForceMoveToLocation", 382,
			boost::bind(&ScriptFunctions::actionForceMoveToLocation, this, _1),
			createSignature(4, kTypeVoid, kTypeEngineType, kTypeInt, kTypeFloat),
			createDefaults(2, d.int0, d.float30_0));
	FunctionMan.registerFunction("ActionForceMoveToObject", 383,
			boost::bind(&ScriptFunctions::actionForceMoveToObject, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeInt, kTypeFloat, kTypeFloat),
			createDefaults(3, d.int0, d.float1_0, d.float30_0));
	FunctionMan.registerFunction("GetJournalQuestExperience", 384,
			boost::bind(&ScriptFunctions::getJournalQuestExperience, this, _1),
			createSignature(2, kTypeInt, kTypeString));
	FunctionMan.registerFunction("JumpToObject", 385,
			boost::bind(&ScriptFunctions::jumpToObject, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetMapPinEnabled", 386,
			boost::bind(&ScriptFunctions::setMapPinEnabled, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("EffectHitPointChangeWhenDying", 387,
			boost::bind(&ScriptFunctions::effectHitPointChangeWhenDying, this, _1),
			createSignature(2, kTypeEngineType, kTypeFloat));
	FunctionMan.registerFunction("PopUpGUIPanel", 388,
			boost::bind(&ScriptFunctions::popUpGUIPanel, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("ClearPersonalReputation", 389,
			boost::bind(&ScriptFunctions::clearPersonalReputation, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetIsTemporaryFriend", 390,
			boost::bind(&ScriptFunctions::setIsTemporaryFriend, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(3, d.object0, d.int0, d.float180_0));
	FunctionMan.registerFunction("SetIsTemporaryEnemy", 391,
			boost::bind(&ScriptFunctions::setIsTemporaryEnemy, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(3, d.object0, d.int0, d.float180_0));
	FunctionMan.registerFunction("SetIsTemporaryNeutral", 392,
			boost::bind(&ScriptFunctions::setIsTemporaryNeutral, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(3, d.object0, d.int0, d.float180_0));
	FunctionMan.registerFunction("GiveXPToCreature", 393,
			boost::bind(&ScriptFunctions::giveXPToCreature, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetXP", 394,
			boost::bind(&ScriptFunctions::setXP, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetXP", 395,
			boost::bind(&ScriptFunctions::getXP, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("IntToHexString", 396,
			boost::bind(&ScriptFunctions::intToHexString, this, _1),
			createSignature(2, kTypeString, kTypeInt));
	FunctionMan.registerFunction("GetBaseItemType", 397,
			boost::bind(&ScriptFunctions::getBaseItemType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetItemHasItemProperty", 398,
			boost::bind(&ScriptFunctions::getItemHasItemProperty, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("ActionEquipMostDamagingMelee", 399,
			boost::bind(&ScriptFunctions::actionEquipMostDamagingMelee, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int0));
}

void ScriptFunctions::playAnimation(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	Animation animation = (Animation) ctx.getParams()[0].getInt();

	// TODO: ScriptFunctions::playAnimation(): speed, second
	// float speed   = ctx.getParams()[1].getFloat();
	// float seconds = ctx.getParams()[2].getFloat();

	object->playAnimation(animation);
}

void ScriptFunctions::talentSpell(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: TalentSpell");
}

void ScriptFunctions::talentFeat(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: TalentFeat");
}

void ScriptFunctions::talentSkill(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: TalentSkill");
}

void ScriptFunctions::getHasSpellEffect(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	int spell = ctx.getParams()[0].getInt();

	warning("TODO: GetHasSpellEffect: \"%s\", %d", object->getTag().c_str(), spell);
}

void ScriptFunctions::getEffectSpellId(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetEffectSpellId");
}

void ScriptFunctions::getCreatureHasTalent(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureHasTalent");
}

void ScriptFunctions::getCreatureTalentRandom(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureTalentRandom");
}

void ScriptFunctions::getCreatureTalentBest(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureTalentBest");
}

void ScriptFunctions::actionUseTalentOnObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionUseTalentOnObject");
}

void ScriptFunctions::actionUseTalentAtLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionUseTalentAtLocation");
}

void ScriptFunctions::getGoldPieceValue(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetGoldPieceValue");
}

void ScriptFunctions::getIsPlayableRacialType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsPlayableRacialType");
}

void ScriptFunctions::jumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	Object   *object = convertObject(ctx.getCaller());
	Location *moveTo = convertLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void ScriptFunctions::effectTemporaryHitpoints(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectTemporaryHitpoints");
}

void ScriptFunctions::getSkillRank(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	int skill = ctx.getParams()[0].getInt();

	// TODO: ScriptFunctions::getSkillRank(): base
	// bool base  = ctx.getParams()[2].getInt() != 0;

	ctx.getReturn() = creature->getSkillRank(skill);
}

void ScriptFunctions::getAttackTarget(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAttackTarget");
}

void ScriptFunctions::getLastAttackType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastAttackType");
}

void ScriptFunctions::getLastAttackMode(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastAttackMode");
}

void ScriptFunctions::getMaster(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	ctx.getReturn() = creature->getMaster();
}

void ScriptFunctions::getIsInCombat(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsInCombat");
}

void ScriptFunctions::getLastAssociateCommand(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastAssociateCommand");
}

void ScriptFunctions::giveGoldToCreature(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GiveGoldToCreature");
}

void ScriptFunctions::setIsDestroyable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetIsDestroyable");
}

void ScriptFunctions::setLocked(Aurora::NWScript::FunctionContext &ctx) {
	Situated *situated = convertSituated(ctx.getParams()[0].getObject());
	if (!situated)
		return;

	bool locked = ctx.getParams()[1].getInt() != 0;
	situated->setLocked(locked);
}

void ScriptFunctions::getLocked(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Situated *situated = convertSituated(ctx.getParams()[0].getObject());
	if (!situated)
		return;

	ctx.getReturn() = situated->isLocked();
}

void ScriptFunctions::getClickingObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void ScriptFunctions::setAssociateListenPatterns(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	warning("TODO: SetAssociateListenPatterns: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getLastWeaponUsed(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastWeaponUsed");
}

void ScriptFunctions::actionInteractObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionInteractObject");
}

void ScriptFunctions::getLastUsedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void ScriptFunctions::getAbilityModifier(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAbilityModifier");
}

void ScriptFunctions::getIdentified(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIdentified");
}

void ScriptFunctions::setIdentified(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetIdentified");
}

void ScriptFunctions::summonAnimalCompanion(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SummonAnimalCompanion");
}

void ScriptFunctions::summonFamiliar(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SummonFamiliar");
}

void ScriptFunctions::getBlockingDoor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetBlockingDoor");
}

void ScriptFunctions::getIsDoorActionPossible(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsDoorActionPossible");
}

void ScriptFunctions::doDoorAction(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: DoDoorAction");
}

void ScriptFunctions::getFirstItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	warning("TODO: GetFirstItemInInventory: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getNextItemInInventory(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	warning("TODO: GetNextItemInInventory: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getClassByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kClassInvalid;

	uint32 position = (uint32) ctx.getParams()[0].getInt() - 1;

	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());

	if (creature) {
		uint32 classID;
		uint16 level;
		creature->getClass(position, classID, level);

		ctx.getReturn() = (int32) classID;
	}
}

void ScriptFunctions::getLevelByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	uint32 position = (uint32) ctx.getParams()[0].getInt() - 1;

	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());

	if (creature) {
		uint32 classID;
		uint16 level;
		creature->getClass(position, classID, level);

		ctx.getReturn() = (int32) level;
	}
}

void ScriptFunctions::getLevelByClass(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	uint32 classID = (uint32) ctx.getParams()[0].getInt();

	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());

	if (creature)
		ctx.getReturn() = creature->getClassLevel(classID);
}

void ScriptFunctions::getDamageDealtByType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetDamageDealtByType");
}

void ScriptFunctions::getTotalDamageDealt(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTotalDamageDealt");
}

void ScriptFunctions::getLastDamager(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastDamager");
}

void ScriptFunctions::getLastDisarmed(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastDisarmed");
}

void ScriptFunctions::getLastDisturbed(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastDisturbed");
}

void ScriptFunctions::getLastLocked(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastLocked");
}

void ScriptFunctions::getLastUnlocked(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastUnlocked");
}

void ScriptFunctions::effectSkillIncrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSkillIncrease");
}

void ScriptFunctions::getInventoryDisturbType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetInventoryDisturbType");
}

void ScriptFunctions::getInventoryDisturbItem(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetInventoryDisturbItem");
}

void ScriptFunctions::getHenchman(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	int nth  = ctx.getParams()[1].getInt();

	ctx.getReturn() = creature->getAssociate(kAssociateTypeHenchman, nth);
}

void ScriptFunctions::versusAlignmentEffect(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: VersusAlignmentEffect");
}

void ScriptFunctions::versusRacialTypeEffect(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: VersusRacialTypeEffect");
}

void ScriptFunctions::versusTrapEffect(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: VersusTrapEffect");
}

void ScriptFunctions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kGenderNone;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getGender();
}

void ScriptFunctions::getIsTalentValid(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsTalentValid");
}

void ScriptFunctions::actionMoveAwayFromLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionMoveAwayFromLocation");
}

void ScriptFunctions::getAttemptedAttackTarget(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	warning("TODO: GetAttemptedAttackTarget: \"%s\"", creature->getTag().c_str());
}

void ScriptFunctions::getTypeFromTalent(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTypeFromTalent");
}

void ScriptFunctions::getIdFromTalent(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIdFromTalent");
}

void ScriptFunctions::getAssociate(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	int type = ctx.getParams()[0].getInt();
	int nth  = ctx.getParams()[2].getInt();

	ctx.getReturn() = creature->getAssociate((AssociateType) type, nth);
}

void ScriptFunctions::addHenchman(Aurora::NWScript::FunctionContext &ctx) {
	Creature *master   = convertCreature(ctx.getParams()[0].getObject());
	Creature *henchman = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		henchman = convertCreature(ctx.getCaller());

	if (!master || !henchman)
		return;

	master->addAssociate(*henchman, kAssociateTypeHenchman);
}

void ScriptFunctions::removeHenchman(Aurora::NWScript::FunctionContext &ctx) {
	Creature *master   = convertCreature(ctx.getParams()[0].getObject());
	Creature *henchman = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		henchman = convertCreature(ctx.getCaller());

	if (!master || !henchman)
		return;

	master->removeAssociate(*henchman);
}

void ScriptFunctions::addJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &plot = ctx.getParams()[0].getString();
	int state = ctx.getParams()[1].getInt();

	Creature *pc = convertPC(ctx.getParams()[2].getObject());

	bool allPartyMembers = ctx.getParams()[3].getInt();
	bool allPlayers      = ctx.getParams()[4].getInt();
	bool allowOverride   = ctx.getParams()[5].getInt();

	warning("TODO: AddJournalQuestEntry: %s: \"%s\" to %d (%d, %d, %d)", gTag(pc).c_str(),
	        plot.c_str(), state, allPartyMembers, allPlayers, allowOverride);
}

void ScriptFunctions::removeJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: RemoveJournalQuestEntry");
}

void ScriptFunctions::getPCPublicCDKey(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCPublicCDKey");
}

void ScriptFunctions::getPCIPAddress(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCIPAddress");
}

void ScriptFunctions::getPCPlayerName(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCPlayerName");
}

void ScriptFunctions::setPCLike(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPCLike");
}

void ScriptFunctions::setPCDislike(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPCDislike");
}

void ScriptFunctions::sendMessageToPC(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Creature *pc = convertPC(params[0].getObject());
	if (!pc)
		return;

	const Common::UString &msg = params[1].getString();

	warning("Send message to PC \"%s\": \"%s\"", pc->getName().c_str(), msg.c_str());
}

void ScriptFunctions::getAttemptedSpellTarget(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	warning("TODO: GetAttemptedSpellTarget: \"%s\"", creature->getTag().c_str());
}

void ScriptFunctions::getLastOpenedBy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastOpenedBy");
}

void ScriptFunctions::getHasSpell(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetHasSpell");
}

void ScriptFunctions::openStore(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: OpenStore");
}

void ScriptFunctions::effectTurned(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectTurned");
}

void ScriptFunctions::getFirstFactionMember(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFirstFactionMember");
}

void ScriptFunctions::getNextFactionMember(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetNextFactionMember");
}

void ScriptFunctions::actionForceMoveToLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionForceMoveToLocation");
}

void ScriptFunctions::actionForceMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionForceMoveToObject");
}

void ScriptFunctions::getJournalQuestExperience(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetJournalQuestExperience");
}

void ScriptFunctions::jumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::jumpToObject(): walkStraightLineToPoint
	// bool walkStraightLineToPoint = ctx.getParams()[1].getInt() != 0;

	Object *object = convertObject(ctx.getCaller());
	Object *moveTo = convertObject(ctx.getParams()[0].getObject());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void ScriptFunctions::setMapPinEnabled(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetMapPinEnabled");
}

void ScriptFunctions::effectHitPointChangeWhenDying(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectHitPointChangeWhenDying");
}

void ScriptFunctions::popUpGUIPanel(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: PopUpGUIPanel");
}

void ScriptFunctions::clearPersonalReputation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ClearPersonalReputation");
}

void ScriptFunctions::setIsTemporaryFriend(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetIsTemporaryFriend");
}

void ScriptFunctions::setIsTemporaryEnemy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetIsTemporaryEnemy");
}

void ScriptFunctions::setIsTemporaryNeutral(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetIsTemporaryNeutral");
}

void ScriptFunctions::giveXPToCreature(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GiveXPToCreature");
}

void ScriptFunctions::setXP(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetXP");
}

void ScriptFunctions::getXP(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *pc = convertPC(ctx.getParams()[0].getObject());
	if (!pc)
		return;

	ctx.getReturn() = (int32) pc->getXP();
}

void ScriptFunctions::intToHexString(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: IntToHexString");
}

void ScriptFunctions::getBaseItemType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetBaseItemType");
}

void ScriptFunctions::getItemHasItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemHasItemProperty");
}

void ScriptFunctions::actionEquipMostDamagingMelee(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionEquipMostDamagingMelee");
}

} // End of namespace NWN

} // End of namespace Engines
