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
 *  NWN script functions, 200-299.
 */

#include <cstdio>

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/ncsfile.h"

#include "src/engines/aurora/tokenman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/location.h"

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

void ScriptFunctions::registerFunctions200(const Defaults &d) {
	FunctionMan.registerFunction("GetObjectByTag", 200,
			boost::bind(&ScriptFunctions::getObjectByTag, this, _1),
			createSignature(3, kTypeObject, kTypeString, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("AdjustAlignment", 201,
			boost::bind(&ScriptFunctions::adjustAlignment, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("ActionWait", 202,
			boost::bind(&ScriptFunctions::actionWait, this, _1),
			createSignature(2, kTypeVoid, kTypeFloat));
	FunctionMan.registerFunction("SetAreaTransitionBMP", 203,
			boost::bind(&ScriptFunctions::setAreaTransitionBMP, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeString),
			createDefaults(1, d.stringEmpty));
	FunctionMan.registerFunction("ActionStartConversation", 204,
			boost::bind(&ScriptFunctions::actionStartConversation, this, _1),
			createSignature(5, kTypeVoid, kTypeObject, kTypeString, kTypeInt, kTypeInt),
			createDefaults(3, d.stringEmpty, d.int0, d.int1));
	FunctionMan.registerFunction("ActionPauseConversation", 205,
			boost::bind(&ScriptFunctions::actionPauseConversation, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("ActionResumeConversation", 206,
			boost::bind(&ScriptFunctions::actionResumeConversation, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("EffectBeam", 207,
			boost::bind(&ScriptFunctions::effectBeam, this, _1),
			createSignature(5, kTypeEngineType, kTypeInt, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetReputation", 208,
			boost::bind(&ScriptFunctions::getReputation, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("AdjustReputation", 209,
			boost::bind(&ScriptFunctions::adjustReputation, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetSittingCreature", 210,
			boost::bind(&ScriptFunctions::getSittingCreature, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetGoingToBeAttackedBy", 211,
			boost::bind(&ScriptFunctions::getGoingToBeAttackedBy, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("EffectSpellResistanceIncrease", 212,
			boost::bind(&ScriptFunctions::effectSpellResistanceIncrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetLocation", 213,
			boost::bind(&ScriptFunctions::getLocation, this, _1),
			createSignature(2, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("ActionJumpToLocation", 214,
			boost::bind(&ScriptFunctions::actionJumpToLocation, this, _1),
			createSignature(2, kTypeVoid, kTypeEngineType));
	FunctionMan.registerFunction("Location", 215,
			boost::bind(&ScriptFunctions::location, this, _1),
			createSignature(4, kTypeEngineType, kTypeObject, kTypeVector, kTypeFloat));
	FunctionMan.registerFunction("ApplyEffectAtLocation", 216,
			boost::bind(&ScriptFunctions::applyEffectAtLocation, this, _1),
			createSignature(5, kTypeVoid, kTypeInt, kTypeEngineType, kTypeEngineType, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("GetIsPC", 217,
			boost::bind(&ScriptFunctions::getIsPC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("FeetToMeters", 218,
			boost::bind(&ScriptFunctions::feetToMeters, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("YardsToMeters", 219,
			boost::bind(&ScriptFunctions::yardsToMeters, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("ApplyEffectToObject", 220,
			boost::bind(&ScriptFunctions::applyEffectToObject, this, _1),
			createSignature(5, kTypeVoid, kTypeInt, kTypeEngineType, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("SpeakString", 221,
			boost::bind(&ScriptFunctions::speakString, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeInt),
			createDefaults(1, d.intTalk));
	FunctionMan.registerFunction("GetSpellTargetLocation", 222,
			boost::bind(&ScriptFunctions::getSpellTargetLocation, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetPositionFromLocation", 223,
			boost::bind(&ScriptFunctions::getPositionFromLocation, this, _1),
			createSignature(2, kTypeVector, kTypeEngineType));
	FunctionMan.registerFunction("GetAreaFromLocation", 224,
			boost::bind(&ScriptFunctions::getAreaFromLocation, this, _1),
			createSignature(2, kTypeObject, kTypeEngineType));
	FunctionMan.registerFunction("GetFacingFromLocation", 225,
			boost::bind(&ScriptFunctions::getFacingFromLocation, this, _1),
			createSignature(2, kTypeFloat, kTypeEngineType));
	FunctionMan.registerFunction("GetNearestCreatureToLocation", 226,
			boost::bind(&ScriptFunctions::getNearestCreatureToLocation, this, _1),
			createSignature(9, kTypeObject, kTypeInt, kTypeInt, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(5, d.int1, d.intm1, d.intm1, d.intm1, d.intm1));
	FunctionMan.registerFunction("GetNearestObject", 227,
			boost::bind(&ScriptFunctions::getNearestObject, this, _1),
			createSignature(4, kTypeObject, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(3, d.intObjectTypeAll, d.object0, d.int1));
	FunctionMan.registerFunction("GetNearestObjectToLocation", 228,
			boost::bind(&ScriptFunctions::getNearestObjectToLocation, this, _1),
			createSignature(4, kTypeObject, kTypeInt, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetNearestObjectByTag", 229,
			boost::bind(&ScriptFunctions::getNearestObjectByTag, this, _1),
			createSignature(4, kTypeObject, kTypeString, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("IntToFloat", 230,
			boost::bind(&ScriptFunctions::intToFloat, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("FloatToInt", 231,
			boost::bind(&ScriptFunctions::floatToInt, this, _1),
			createSignature(2, kTypeInt, kTypeFloat));
	FunctionMan.registerFunction("StringToInt", 232,
			boost::bind(&ScriptFunctions::stringToInt, this, _1),
			createSignature(2, kTypeInt, kTypeString));
	FunctionMan.registerFunction("StringToFloat", 233,
			boost::bind(&ScriptFunctions::stringToFloat, this, _1),
			createSignature(2, kTypeFloat, kTypeString));
	FunctionMan.registerFunction("ActionCastSpellAtLocation", 234,
			boost::bind(&ScriptFunctions::actionCastSpellAtLocation, this, _1),
			createSignature(7, kTypeVoid, kTypeInt, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(4, d.intMetaMagicAny, d.int0, d.intProjPathDefault, d.int0));
	FunctionMan.registerFunction("GetIsEnemy", 235,
			boost::bind(&ScriptFunctions::getIsEnemy, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsFriend", 236,
			boost::bind(&ScriptFunctions::getIsFriend, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsNeutral", 237,
			boost::bind(&ScriptFunctions::getIsNeutral, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetPCSpeaker", 238,
			boost::bind(&ScriptFunctions::getPCSpeaker, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetStringByStrRef", 239,
			boost::bind(&ScriptFunctions::getStringByStrRef, this, _1),
			createSignature(3, kTypeString, kTypeInt, kTypeInt),
			createDefaults(1, d.intMale));
	FunctionMan.registerFunction("ActionSpeakStringByStrRef", 240,
			boost::bind(&ScriptFunctions::actionSpeakStringByStrRef, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeInt),
			createDefaults(1, d.intTalk));
	FunctionMan.registerFunction("DestroyObject", 241,
			boost::bind(&ScriptFunctions::destroyObject, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("GetModule", 242,
			boost::bind(&ScriptFunctions::getModule, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("CreateObject", 243,
			boost::bind(&ScriptFunctions::createObject, this, _1),
			createSignature(6, kTypeObject, kTypeInt, kTypeString, kTypeEngineType, kTypeInt, kTypeString),
			createDefaults(2, d.int0, d.stringEmpty));
	FunctionMan.registerFunction("EventSpellCastAt", 244,
			boost::bind(&ScriptFunctions::eventSpellCastAt, this, _1),
			createSignature(4, kTypeEngineType, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetLastSpellCaster", 245,
			boost::bind(&ScriptFunctions::getLastSpellCaster, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastSpell", 246,
			boost::bind(&ScriptFunctions::getLastSpell, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetUserDefinedEventNumber", 247,
			boost::bind(&ScriptFunctions::getUserDefinedEventNumber, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetSpellId", 248,
			boost::bind(&ScriptFunctions::getSpellId, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("RandomName", 249,
			boost::bind(&ScriptFunctions::randomName, this, _1),
			createSignature(2, kTypeString, kTypeInt),
			createDefaults(1, d.intFirstGenericMale));
	FunctionMan.registerFunction("EffectPoison", 250,
			boost::bind(&ScriptFunctions::effectPoison, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectDisease", 251,
			boost::bind(&ScriptFunctions::effectDisease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectSilence", 252,
			boost::bind(&ScriptFunctions::effectSilence, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetName", 253,
			boost::bind(&ScriptFunctions::getName, this, _1),
			createSignature(3, kTypeString, kTypeObject, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetLastSpeaker", 254,
			boost::bind(&ScriptFunctions::getLastSpeaker, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("BeginConversation", 255,
			boost::bind(&ScriptFunctions::beginConversation, this, _1),
			createSignature(3, kTypeInt, kTypeString, kTypeObject),
			createDefaults(2, d.stringEmpty, d.object0));
	FunctionMan.registerFunction("GetLastPerceived", 256,
			boost::bind(&ScriptFunctions::getLastPerceived, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastPerceptionHeard", 257,
			boost::bind(&ScriptFunctions::getLastPerceptionHeard, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetLastPerceptionInaudible", 258,
			boost::bind(&ScriptFunctions::getLastPerceptionInaudible, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetLastPerceptionSeen", 259,
			boost::bind(&ScriptFunctions::getLastPerceptionSeen, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetLastClosedBy", 260,
			boost::bind(&ScriptFunctions::getLastClosedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetLastPerceptionVanished", 261,
			boost::bind(&ScriptFunctions::getLastPerceptionVanished, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetFirstInPersistentObject", 262,
			boost::bind(&ScriptFunctions::getFirstInPersistentObject, this, _1),
			createSignature(4, kTypeObject, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(3, d.object0, d.intObjectCreature, d.intPersZoneActive));
	FunctionMan.registerFunction("GetNextInPersistentObject", 263,
			boost::bind(&ScriptFunctions::getNextInPersistentObject, this, _1),
			createSignature(4, kTypeObject, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(3, d.object0, d.intObjectCreature, d.intPersZoneActive));
	FunctionMan.registerFunction("GetAreaOfEffectCreator", 264,
			boost::bind(&ScriptFunctions::getAreaOfEffectCreator, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("DeleteLocalInt", 265,
			boost::bind(&ScriptFunctions::deleteLocalInt, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("DeleteLocalFloat", 266,
			boost::bind(&ScriptFunctions::deleteLocalFloat, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("DeleteLocalString", 267,
			boost::bind(&ScriptFunctions::deleteLocalString, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("DeleteLocalObject", 268,
			boost::bind(&ScriptFunctions::deleteLocalObject, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("DeleteLocalLocation", 269,
			boost::bind(&ScriptFunctions::deleteLocalLocation, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("EffectHaste", 270,
			boost::bind(&ScriptFunctions::effectHaste, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectSlow", 271,
			boost::bind(&ScriptFunctions::effectSlow, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ObjectToString", 272,
			boost::bind(&ScriptFunctions::objectToString, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("EffectImmunity", 273,
			boost::bind(&ScriptFunctions::effectImmunity, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetIsImmune", 274,
			boost::bind(&ScriptFunctions::getIsImmune, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectDamageImmunityIncrease", 275,
			boost::bind(&ScriptFunctions::effectDamageImmunityIncrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetEncounterActive", 276,
			boost::bind(&ScriptFunctions::getEncounterActive, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetEncounterActive", 277,
			boost::bind(&ScriptFunctions::setEncounterActive, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetEncounterSpawnsMax", 278,
			boost::bind(&ScriptFunctions::getEncounterSpawnsMax, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetEncounterSpawnsMax", 279,
			boost::bind(&ScriptFunctions::setEncounterSpawnsMax, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetEncounterSpawnsCurrent", 280,
			boost::bind(&ScriptFunctions::getEncounterSpawnsCurrent, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetEncounterSpawnsCurrent", 281,
			boost::bind(&ScriptFunctions::setEncounterSpawnsCurrent, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetModuleItemAcquired", 282,
			boost::bind(&ScriptFunctions::getModuleItemAcquired, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetModuleItemAcquiredFrom", 283,
			boost::bind(&ScriptFunctions::getModuleItemAcquiredFrom, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("SetCustomToken", 284,
			boost::bind(&ScriptFunctions::setCustomToken, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeString));
	FunctionMan.registerFunction("GetHasFeat", 285,
			boost::bind(&ScriptFunctions::getHasFeat, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetHasSkill", 286,
			boost::bind(&ScriptFunctions::getHasSkill, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("ActionUseFeat", 287,
			boost::bind(&ScriptFunctions::actionUseFeat, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("ActionUseSkill", 288,
			boost::bind(&ScriptFunctions::actionUseSkill, this, _1),
			createSignature(5, kTypeVoid, kTypeInt, kTypeObject, kTypeInt, kTypeObject),
			createDefaults(2, d.int0, d.object0));
	FunctionMan.registerFunction("GetObjectSeen", 289,
			boost::bind(&ScriptFunctions::getObjectSeen, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetObjectHeard", 290,
			boost::bind(&ScriptFunctions::getObjectHeard, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastPlayerDied", 291,
			boost::bind(&ScriptFunctions::getLastPlayerDied, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetModuleItemLost", 292,
			boost::bind(&ScriptFunctions::getModuleItemLost, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetModuleItemLostBy", 293,
			boost::bind(&ScriptFunctions::getModuleItemLostBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("ActionDoCommand", 294,
			boost::bind(&ScriptFunctions::actionDoCommand, this, _1),
			createSignature(2, kTypeVoid, kTypeScriptState));
	FunctionMan.registerFunction("EventConversation", 295,
			boost::bind(&ScriptFunctions::eventConversation, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("SetEncounterDifficulty", 296,
			boost::bind(&ScriptFunctions::setEncounterDifficulty, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetEncounterDifficulty", 297,
			boost::bind(&ScriptFunctions::getEncounterDifficulty, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetDistanceBetweenLocations", 298,
			boost::bind(&ScriptFunctions::getDistanceBetweenLocations, this, _1),
			createSignature(3, kTypeFloat, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("GetReflexAdjustedDamage", 299,
			boost::bind(&ScriptFunctions::getReflexAdjustedDamage, this, _1),
			createSignature(6, kTypeInt, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(2, d.intSaveNone, d.object0));
}

void ScriptFunctions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Module *module = _engine->getModule();
	if (!module)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	int nth = ctx.getParams()[1].getInt();

	Aurora::NWScript::ObjectSearch *search = module->findObjectsByTag(tag);
	while (nth-- >= 0)
		search->next();

	ctx.getReturn() = search->get();
	delete search;
}

void ScriptFunctions::adjustAlignment(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AdjustAlignment");
}

void ScriptFunctions::actionWait(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionWait");
}

void ScriptFunctions::setAreaTransitionBMP(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetAreaTransitionBMP");
}

void ScriptFunctions::actionStartConversation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionStartConversation(): /Action/

	Module *module = _engine->getModule();
	if (!module)
		return;

	Object *source = convertObject(ctx.getCaller());
	Object *target = convertObject(ctx.getParams()[0].getObject());
	if (!source || !target)
		return;

	Creature *pc = convertPC(target);
	if (!pc) {
		warning("TODO: ActionStartConversation: Non-PC target \"%s\"", target->getTag().c_str());
		return;
	}

	if (source->getPCSpeaker()) {
		if (source->getPCSpeaker() != pc) {
			Creature *otherPC = convertPC(source->getPCSpeaker());

			warning("ScriptFunctions::actionStartConversation(): "
			        "Object \"%s\" already in conversation with PC \"%s\"",
			        source->getTag().c_str(), otherPC ? otherPC->getName().c_str() : "");
			return;
		}
	}

	Common::UString conversation = ctx.getParams()[1].getString();
	if (conversation.empty())
		conversation = source->getConversation();

	// TODO: ScriptFunctions::actionStartConversation(): privateConv
	// bool privateConv = ctx.getParams()[2].getInt() != 0;

	bool playHello = ctx.getParams()[2].getInt() != 0;

	module->startConversation(conversation, *pc, *source, playHello);
}

void ScriptFunctions::actionPauseConversation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionPauseConversation");
}

void ScriptFunctions::actionResumeConversation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionResumeConversation");
}

void ScriptFunctions::effectBeam(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectBeam");
}

void ScriptFunctions::getReputation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetReputation");
}

void ScriptFunctions::adjustReputation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AdjustReputation");
}

void ScriptFunctions::getSittingCreature(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSittingCreature");
}

void ScriptFunctions::getGoingToBeAttackedBy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetGoingToBeAttackedBy");
}

void ScriptFunctions::effectSpellResistanceIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSpellResistanceIncrease");
}

void ScriptFunctions::getLocation(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	ctx.getReturn() = object->getLocation();
}

void ScriptFunctions::actionJumpToLocation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionJumpToLocation(): /Action/

	Object   *object = convertObject(ctx.getCaller());
	Location *moveTo = convertLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void ScriptFunctions::location(Aurora::NWScript::FunctionContext &ctx) {
	Location loc;

	loc.setArea  (convertArea(ctx.getParams()[0].getObject()));
	loc.setFacing(ctx.getParams()[2].getFloat());

	float x, y, z;
	ctx.getParams()[1].getVector(x, y, z);
	loc.setPosition(x, y, z);

	ctx.getReturn() = loc;
}

void ScriptFunctions::applyEffectAtLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ApplyEffectAtLocation");
}

void ScriptFunctions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = convertPC(ctx.getParams()[0].getObject()) != 0;
}

void ScriptFunctions::feetToMeters(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FeetToMeters");
}

void ScriptFunctions::yardsToMeters(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: YardsToMeters");
}

void ScriptFunctions::applyEffectToObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ApplyEffectToObject");
}

void ScriptFunctions::speakString(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &str = ctx.getParams()[0].getString();

	// TODO: ScriptFunctions::speakString(): Volume
	uint32 volume = (uint32) ctx.getParams()[1].getInt();

	object->speakString(str, volume);
}

void ScriptFunctions::getSpellTargetLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSpellTargetLocation");
}

void ScriptFunctions::getPositionFromLocation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Location *loc = convertLocation(ctx.getParams()[0].getEngineType());
	if (!loc)
		return;

	float x, y, z;
	loc->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

void ScriptFunctions::getAreaFromLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetAreaFromLocation");
}

void ScriptFunctions::getFacingFromLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFacingFromLocation");
}

void ScriptFunctions::getNearestCreatureToLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNearestCreatureToLocation");
}

void ScriptFunctions::getNearestObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Module *module = _engine->getModule();
	if (!module)
		return;

	Object *target = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		target = convertObject(ctx.getCaller());

	if (!target)
		return;

	ObjectType type = (ObjectType) ctx.getParams()[0].getInt();
	int nth = ctx.getParams()[2].getInt() - 1;

	Aurora::NWScript::ObjectSearch *search = module->findObjects();
	Aurora::NWScript::Object       *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		Object *nwnObject = convertObject(object);

		if (nwnObject && (nwnObject != target) && (nwnObject->getType() == type) &&
		    (nwnObject->getArea() == target->getArea()))
			objects.push_back(nwnObject);
	}

	delete search;

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (int n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
}

void ScriptFunctions::getNearestObjectToLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNearestObjectToLocation");
}

void ScriptFunctions::getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Object *) 0;
	Module *module = _engine->getModule();
	if (!module)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	Object *target = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		target = convertObject(ctx.getCaller());
	if (!target)
		return;

	int nth = ctx.getParams()[2].getInt() - 1;

	Aurora::NWScript::ObjectSearch *search = module->findObjectsByTag(tag);
	Aurora::NWScript::Object       *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		Object *nwnObject = convertObject(object);

		if (nwnObject && (nwnObject != target) && (nwnObject->getArea() == target->getArea()))
			objects.push_back(nwnObject);
	}

	delete search;

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (int n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
}

void ScriptFunctions::intToFloat(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (float) ctx.getParams()[0].getInt();
}

void ScriptFunctions::floatToInt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) ctx.getParams()[0].getFloat();
}

void ScriptFunctions::stringToInt(Aurora::NWScript::FunctionContext &ctx) {
	int i;
	std::sscanf(ctx.getParams()[0].getString().c_str(), "%d", &i);

	ctx.getReturn() = (int32) i;
}

void ScriptFunctions::stringToFloat(Aurora::NWScript::FunctionContext &ctx) {
	float f;
	std::sscanf(ctx.getParams()[0].getString().c_str(), "%f", &f);

	ctx.getReturn() = f;
}

void ScriptFunctions::actionCastSpellAtLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionCastSpellAtLocation");
}

void ScriptFunctions::getIsEnemy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsEnemy");
}

void ScriptFunctions::getIsFriend(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsFriend");
}

void ScriptFunctions::getIsNeutral(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsNeutral");
}

void ScriptFunctions::getPCSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *speaker = 0;
	Object *object = convertObject(ctx.getCaller());
	if (object)
		speaker = object->getPCSpeaker();

	ctx.getReturn() = speaker;
}

void ScriptFunctions::getStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	const uint32 strRef = (uint32) ctx.getParams()[0].getInt();
	const Aurora::LanguageGender gender = (Aurora::LanguageGender) ctx.getParams()[1].getInt();

	ctx.getReturn() = TalkMan.getString(strRef, gender);
}

void ScriptFunctions::actionSpeakStringByStrRef(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionSpeakStringByStrRef");
}

void ScriptFunctions::destroyObject(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	float delay = ctx.getParams()[1].getFloat();

	warning("TODO: DestroyObject: \"%s\" (%f)", object->getTag().c_str(), delay);
}

void ScriptFunctions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _engine->getModule();
}

void ScriptFunctions::createObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	ObjectType type = (ObjectType) ctx.getParams()[0].getInt();
	const Common::UString &templ = ctx.getParams()[1].getString();

	// Location *loc = convertLocation(ctx.getParams()[2].getEngineType());
	// bool useAppearAnimation = ctx.getParams()[3].getInt() != 0;

	const Common::UString &newTag = ctx.getParams()[4].getString();

	warning("TODO: CreateObject: %d: \"%s\" (\"%s\")", type, templ.c_str(), newTag.c_str());
}

void ScriptFunctions::eventSpellCastAt(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EventSpellCastAt");
}

void ScriptFunctions::getLastSpellCaster(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastSpellCaster");
}

void ScriptFunctions::getLastSpell(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastSpell");
}

void ScriptFunctions::getUserDefinedEventNumber(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetUserDefinedEventNumber");
}

void ScriptFunctions::getSpellId(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSpellId");
}

void ScriptFunctions::randomName(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RandomName");
}

void ScriptFunctions::effectPoison(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectPoison");
}

void ScriptFunctions::effectDisease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDisease");
}

void ScriptFunctions::effectSilence(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSilence");
}

void ScriptFunctions::getName(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getName(): bOriginalName

	ctx.getReturn().getString().clear();

	Module *module = convertModule(ctx.getParams()[0].getObject());
	Area   *area   = convertArea  (ctx.getParams()[0].getObject());
	Object *object = convertObject(ctx.getParams()[0].getObject());

	if      (module)
		ctx.getReturn().getString() = module->getName();
	else if (area)
		ctx.getReturn().getString() = area->getName();
	else if (object)
		ctx.getReturn().getString() = object->getName();
}

void ScriptFunctions::getLastSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	warning("TODO: GetLastSpeaker");
}

void ScriptFunctions::beginConversation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Module *module = _engine->getModule();
	if (!module)
		return;

	const Aurora::NWScript::Parameters &params = ctx.getParams();

	// Get the script object parameters
	Aurora::NWScript::Object *obj1 = ctx.getCaller();
	Aurora::NWScript::Object *obj2 = params[1].getObject();
	if (!obj2)
		obj2 = ctx.getTriggerer();
	if (!obj2)
		obj2 = getPC();

	// Try to convert them to an NWN Creature and Object
	Creature *pc     = convertPC(obj2);
	Object   *object = convertObject(obj1);

	// Try the other way round, if necessary
	if (!pc || !object) {
		pc     = convertPC(obj1);
		object = convertObject(obj2);
	}

	// Fail
	if (!pc || !object)
		return;

	if (object->getPCSpeaker()) {
		if (object->getPCSpeaker() != pc) {
			Creature *otherPC = convertPC(object->getPCSpeaker());

			warning("ScriptFunctions::beginConversation(): "
			        "Object \"%s\" already in conversation with PC \"%s\"",
			        object->getTag().c_str(), otherPC ? otherPC->getName().c_str() : "");
			return;
		}
	}

	Common::UString conversation = params[0].getString();
	if (conversation.empty())
		conversation = object->getConversation();

	ctx.getReturn() = module->startConversation(conversation, *pc, *object);
}

void ScriptFunctions::getLastPerceived(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPerceived");
}

void ScriptFunctions::getLastPerceptionHeard(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPerceptionHeard");
}

void ScriptFunctions::getLastPerceptionInaudible(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPerceptionInaudible");
}

void ScriptFunctions::getLastPerceptionSeen(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPerceptionSeen");
}

void ScriptFunctions::getLastClosedBy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastClosedBy");
}

void ScriptFunctions::getLastPerceptionVanished(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPerceptionVanished");
}

void ScriptFunctions::getFirstInPersistentObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFirstInPersistentObject");
}

void ScriptFunctions::getNextInPersistentObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNextInPersistentObject");
}

void ScriptFunctions::getAreaOfEffectCreator(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetAreaOfEffectCreator");
}

void ScriptFunctions::deleteLocalInt(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DeleteLocalInt");
}

void ScriptFunctions::deleteLocalFloat(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DeleteLocalFloat");
}

void ScriptFunctions::deleteLocalString(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DeleteLocalString");
}

void ScriptFunctions::deleteLocalObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DeleteLocalObject");
}

void ScriptFunctions::deleteLocalLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DeleteLocalLocation");
}

void ScriptFunctions::effectHaste(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectHaste");
}

void ScriptFunctions::effectSlow(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSlow");
}

void ScriptFunctions::objectToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::format("%p", (void *) ctx.getParams()[0].getObject());
}

void ScriptFunctions::effectImmunity(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectImmunity");
}

void ScriptFunctions::getIsImmune(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsImmune");
}

void ScriptFunctions::effectDamageImmunityIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDamageImmunityIncrease");
}

void ScriptFunctions::getEncounterActive(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEncounterActive");
}

void ScriptFunctions::setEncounterActive(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetEncounterActive");
}

void ScriptFunctions::getEncounterSpawnsMax(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEncounterSpawnsMax");
}

void ScriptFunctions::setEncounterSpawnsMax(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetEncounterSpawnsMax");
}

void ScriptFunctions::getEncounterSpawnsCurrent(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEncounterSpawnsCurrent");
}

void ScriptFunctions::setEncounterSpawnsCurrent(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetEncounterSpawnsCurrent");
}

void ScriptFunctions::getModuleItemAcquired(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleItemAcquired");
}

void ScriptFunctions::getModuleItemAcquiredFrom(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleItemAcquiredFrom");
}

void ScriptFunctions::setCustomToken(Aurora::NWScript::FunctionContext &ctx) {
	int32 tokenNumber = ctx.getParams()[0].getInt();
	const Common::UString &tokenValue = ctx.getParams()[1].getString();

	const Common::UString tokenName = Common::UString::format("<CUSTOM%d>", tokenNumber);

	TokenMan.set(tokenName, tokenValue);
}

void ScriptFunctions::getHasFeat(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetHasFeat");
}

void ScriptFunctions::getHasSkill(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetHasSkill");
}

void ScriptFunctions::actionUseFeat(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionUseFeat");
}

void ScriptFunctions::actionUseSkill(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionUseSkill");
}

void ScriptFunctions::getObjectSeen(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *target = convertObject(ctx.getParams()[0].getObject());
	Object *source = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		source = convertObject(ctx.getCaller());

	if (!target || !source)
		return;

	warning("TODO: GetObjectSeen: \"%s\" by \"%s\"", target->getTag().c_str(),
			source->getTag().c_str());
}

void ScriptFunctions::getObjectHeard(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *target = convertObject(ctx.getParams()[0].getObject());
	Object *source = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		source = convertObject(ctx.getCaller());

	if (!target || !source)
		return;

	warning("TODO: GetObjectHeard: \"%s\" by \"%s\"", target->getTag().c_str(),
			source->getTag().c_str());
}

void ScriptFunctions::getLastPlayerDied(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPlayerDied");
}

void ScriptFunctions::getModuleItemLost(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleItemLost");
}

void ScriptFunctions::getModuleItemLostBy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleItemLostBy");
}

void ScriptFunctions::actionDoCommand(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionDoCommand(): /Action/

	Module *module = _engine->getModule();
	if (!module)
		return;

	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("ScriptFunctions::actionDoCommand(): Script needed");

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[0].getScriptState();

	module->delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), 0);
}

void ScriptFunctions::eventConversation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EventConversation");
}

void ScriptFunctions::setEncounterDifficulty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetEncounterDifficulty");
}

void ScriptFunctions::getEncounterDifficulty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEncounterDifficulty");
}

void ScriptFunctions::getDistanceBetweenLocations(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDistanceBetweenLocations");
}

void ScriptFunctions::getReflexAdjustedDamage(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetReflexAdjustedDamage");
}

} // End of namespace NWN

} // End of namespace Engines
