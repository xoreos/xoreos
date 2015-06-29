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
 *  NWN script functions, 100-199.
 */

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/waypoint.h"
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

void ScriptFunctions::registerFunctions100(const Defaults &d) {
	FunctionMan.registerFunction("d10", 100,
			boost::bind(&ScriptFunctions::d10, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d12", 101,
			boost::bind(&ScriptFunctions::d12, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d20", 102,
			boost::bind(&ScriptFunctions::d20, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d100", 103,
			boost::bind(&ScriptFunctions::d100, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("VectorMagnitude", 104,
			boost::bind(&ScriptFunctions::vectorMagnitude, this, _1),
			createSignature(2, kTypeFloat, kTypeVector));
	FunctionMan.registerFunction("GetMetaMagicFeat", 105,
			boost::bind(&ScriptFunctions::getMetaMagicFeat, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetObjectType", 106,
			boost::bind(&ScriptFunctions::getObjectType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetRacialType", 107,
			boost::bind(&ScriptFunctions::getRacialType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("FortitudeSave", 108,
			boost::bind(&ScriptFunctions::fortitudeSave, this, _1),
			createSignature(5, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(2, d.intSaveNone, d.object0));
	FunctionMan.registerFunction("ReflexSave", 109,
			boost::bind(&ScriptFunctions::reflexSave, this, _1),
			createSignature(5, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(2, d.intSaveNone, d.object0));
	FunctionMan.registerFunction("WillSave", 110,
			boost::bind(&ScriptFunctions::willSave, this, _1),
			createSignature(5, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(2, d.intSaveNone, d.object0));
	FunctionMan.registerFunction("GetSpellSaveDC", 111,
			boost::bind(&ScriptFunctions::getSpellSaveDC, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("MagicalEffect", 112,
			boost::bind(&ScriptFunctions::magicalEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("SupernaturalEffect", 113,
			boost::bind(&ScriptFunctions::supernaturalEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("ExtraordinaryEffect", 114,
			boost::bind(&ScriptFunctions::extraordinaryEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeEngineType));
	FunctionMan.registerFunction("EffectACIncrease", 115,
			boost::bind(&ScriptFunctions::effectACIncrease, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(2, d.intACDodge, d.intACAll));
	FunctionMan.registerFunction("GetAC", 116,
			boost::bind(&ScriptFunctions::getAC, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("EffectSavingThrowIncrease", 117,
			boost::bind(&ScriptFunctions::effectSavingThrowIncrease, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.intSaveAll));
	FunctionMan.registerFunction("EffectAttackIncrease", 118,
			boost::bind(&ScriptFunctions::effectAttackIncrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intAttackMisc));
	FunctionMan.registerFunction("EffectDamageReduction", 119,
			boost::bind(&ScriptFunctions::effectDamageReduction, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("EffectDamageIncrease", 120,
			boost::bind(&ScriptFunctions::effectDamageIncrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intDamageMagical));
	FunctionMan.registerFunction("RoundsToSeconds", 121,
			boost::bind(&ScriptFunctions::roundsToSeconds, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("HoursToSeconds", 122,
			boost::bind(&ScriptFunctions::hoursToSeconds, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("TurnsToSeconds", 123,
			boost::bind(&ScriptFunctions::turnsToSeconds, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("GetLawChaosValue", 124,
			boost::bind(&ScriptFunctions::getLawChaosValue, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetGoodEvilValue", 125,
			boost::bind(&ScriptFunctions::getGoodEvilValue, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetAlignmentLawChaos", 126,
			boost::bind(&ScriptFunctions::getAlignmentLawChaos, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetAlignmentGoodEvil", 127,
			boost::bind(&ScriptFunctions::getAlignmentGoodEvil, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFirstObjectInShape", 128,
			boost::bind(&ScriptFunctions::getFirstObjectInShape, this, _1),
			createSignature(7, kTypeObject, kTypeInt, kTypeFloat, kTypeEngineType, kTypeInt, kTypeInt, kTypeVector),
			createDefaults(3, d.int0, d.intObjectCreature, d.vector0));
	FunctionMan.registerFunction("GetNextObjectInShape", 129,
			boost::bind(&ScriptFunctions::getNextObjectInShape, this, _1),
			createSignature(7, kTypeObject, kTypeInt, kTypeFloat, kTypeEngineType, kTypeInt, kTypeInt, kTypeVector),
			createDefaults(3, d.int0, d.intObjectCreature, d.vector0));
	FunctionMan.registerFunction("EffectEntangle", 130,
			boost::bind(&ScriptFunctions::effectEntangle, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("SignalEvent", 131,
			boost::bind(&ScriptFunctions::signalEvent, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeEngineType));
	FunctionMan.registerFunction("EventUserDefined", 132,
			boost::bind(&ScriptFunctions::eventUserDefined, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectDeath", 133,
			boost::bind(&ScriptFunctions::effectDeath, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(2, d.int0, d.int1));
	FunctionMan.registerFunction("EffectKnockdown", 134,
			boost::bind(&ScriptFunctions::effectKnockdown, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ActionGiveItem", 135,
			boost::bind(&ScriptFunctions::actionGiveItem, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("ActionTakeItem", 136,
			boost::bind(&ScriptFunctions::actionTakeItem, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("VectorNormalize", 137,
			boost::bind(&ScriptFunctions::vectorNormalize, this, _1),
			createSignature(2, kTypeVector, kTypeVector));
	FunctionMan.registerFunction("EffectCurse", 138,
			boost::bind(&ScriptFunctions::effectCurse, this, _1),
			createSignature(7, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(6, d.int1, d.int1, d.int1, d.int1, d.int1, d.int1));
	FunctionMan.registerFunction("GetAbilityScore", 139,
			boost::bind(&ScriptFunctions::getAbilityScore, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetIsDead", 140,
			boost::bind(&ScriptFunctions::getIsDead, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("PrintVector", 141,
			boost::bind(&ScriptFunctions::printVector, this, _1),
			createSignature(3, kTypeVoid, kTypeVector, kTypeInt));
	FunctionMan.registerFunction("Vector", 142,
			boost::bind(&ScriptFunctions::vector, this, _1),
			createSignature(4, kTypeVector, kTypeFloat, kTypeFloat, kTypeFloat),
			createDefaults(3, d.float0_0, d.float0_0, d.float0_0));
	FunctionMan.registerFunction("SetFacingPoint", 143,
			boost::bind(&ScriptFunctions::setFacingPoint, this, _1),
			createSignature(2, kTypeVoid, kTypeVector));
	FunctionMan.registerFunction("AngleToVector", 144,
			boost::bind(&ScriptFunctions::angleToVector, this, _1),
			createSignature(2, kTypeVector, kTypeFloat));
	FunctionMan.registerFunction("VectorToAngle", 145,
			boost::bind(&ScriptFunctions::vectorToAngle, this, _1),
			createSignature(2, kTypeFloat, kTypeVector));
	FunctionMan.registerFunction("TouchAttackMelee", 146,
			boost::bind(&ScriptFunctions::touchAttackMelee, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("TouchAttackRanged", 147,
			boost::bind(&ScriptFunctions::touchAttackRanged, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("EffectParalyze", 148,
			boost::bind(&ScriptFunctions::effectParalyze, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectSpellImmunity", 149,
			boost::bind(&ScriptFunctions::effectSpellImmunity, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.intSpellsAll));
	FunctionMan.registerFunction("EffectDeaf", 150,
			boost::bind(&ScriptFunctions::effectDeaf, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetDistanceBetween", 151,
			boost::bind(&ScriptFunctions::getDistanceBetween, this, _1),
			createSignature(3, kTypeFloat, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("SetLocalLocation", 152,
			boost::bind(&ScriptFunctions::setLocalLocation, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeEngineType));
	FunctionMan.registerFunction("GetLocalLocation", 153,
			boost::bind(&ScriptFunctions::getLocalLocation, this, _1),
			createSignature(3, kTypeEngineType, kTypeObject, kTypeString));
	FunctionMan.registerFunction("EffectSleep", 154,
			boost::bind(&ScriptFunctions::effectSleep, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetItemInSlot", 155,
			boost::bind(&ScriptFunctions::getItemInSlot, this, _1),
			createSignature(3, kTypeObject, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectCharmed", 156,
			boost::bind(&ScriptFunctions::effectCharmed, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectConfused", 157,
			boost::bind(&ScriptFunctions::effectConfused, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectFrightened", 158,
			boost::bind(&ScriptFunctions::effectFrightened, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectDominated", 159,
			boost::bind(&ScriptFunctions::effectDominated, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectDazed", 160,
			boost::bind(&ScriptFunctions::effectDazed, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectStunned", 161,
			boost::bind(&ScriptFunctions::effectStunned, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("SetCommandable", 162,
			boost::bind(&ScriptFunctions::setCommandable, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCommandable", 163,
			boost::bind(&ScriptFunctions::getCommandable, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectRegenerate", 164,
			boost::bind(&ScriptFunctions::effectRegenerate, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeFloat));
	FunctionMan.registerFunction("EffectMovementSpeedIncrease", 165,
			boost::bind(&ScriptFunctions::effectMovementSpeedIncrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetHitDice", 166,
			boost::bind(&ScriptFunctions::getHitDice, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("ActionForceFollowObject", 167,
			boost::bind(&ScriptFunctions::actionForceFollowObject, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeFloat),
			createDefaults(1, d.float0_0));
	FunctionMan.registerFunction("GetTag", 168,
			boost::bind(&ScriptFunctions::getTag, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("ResistSpell", 169,
			boost::bind(&ScriptFunctions::resistSpell, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetEffectType", 170,
			boost::bind(&ScriptFunctions::getEffectType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("EffectAreaOfEffect", 171,
			boost::bind(&ScriptFunctions::effectAreaOfEffect, this, _1),
			createSignature(5, kTypeEngineType, kTypeInt, kTypeString, kTypeString, kTypeString),
			createDefaults(3, d.stringEmpty, d.stringEmpty, d.stringEmpty));
	FunctionMan.registerFunction("GetFactionEqual", 172,
			boost::bind(&ScriptFunctions::getFactionEqual, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("ChangeFaction", 173,
			boost::bind(&ScriptFunctions::changeFaction, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetIsListening", 174,
			boost::bind(&ScriptFunctions::getIsListening, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetListening", 175,
			boost::bind(&ScriptFunctions::setListening, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetListenPattern", 176,
			boost::bind(&ScriptFunctions::setListenPattern, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("TestStringAgainstPattern", 177,
			boost::bind(&ScriptFunctions::testStringAgainstPattern, this, _1),
			createSignature(3, kTypeInt, kTypeString, kTypeString));
	FunctionMan.registerFunction("GetMatchedSubstring", 178,
			boost::bind(&ScriptFunctions::getMatchedSubstring, this, _1),
			createSignature(2, kTypeString, kTypeInt));
	FunctionMan.registerFunction("GetMatchedSubstringsCount", 179,
			boost::bind(&ScriptFunctions::getMatchedSubstringsCount, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("EffectVisualEffect", 180,
			boost::bind(&ScriptFunctions::effectVisualEffect, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetFactionWeakestMember", 181,
			boost::bind(&ScriptFunctions::getFactionWeakestMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetFactionStrongestMember", 182,
			boost::bind(&ScriptFunctions::getFactionStrongestMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetFactionMostDamagedMember", 183,
			boost::bind(&ScriptFunctions::getFactionMostDamagedMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetFactionLeastDamagedMember", 184,
			boost::bind(&ScriptFunctions::getFactionLeastDamagedMember, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetFactionGold", 185,
			boost::bind(&ScriptFunctions::getFactionGold, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionAverageReputation", 186,
			boost::bind(&ScriptFunctions::getFactionAverageReputation, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetFactionAverageGoodEvilAlignment", 187,
			boost::bind(&ScriptFunctions::getFactionAverageGoodEvilAlignment, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionAverageLawChaosAlignment", 188,
			boost::bind(&ScriptFunctions::getFactionAverageLawChaosAlignment, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionAverageLevel", 189,
			boost::bind(&ScriptFunctions::getFactionAverageLevel, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionAverageXP", 190,
			boost::bind(&ScriptFunctions::getFactionAverageXP, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionMostFrequentClass", 191,
			boost::bind(&ScriptFunctions::getFactionMostFrequentClass, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFactionWorstAC", 192,
			boost::bind(&ScriptFunctions::getFactionWorstAC, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetFactionBestAC", 193,
			boost::bind(&ScriptFunctions::getFactionBestAC, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("ActionSit", 194,
			boost::bind(&ScriptFunctions::actionSit, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetListenPatternNumber", 195,
			boost::bind(&ScriptFunctions::getListenPatternNumber, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("ActionJumpToObject", 196,
			boost::bind(&ScriptFunctions::actionJumpToObject, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetWaypointByTag", 197,
			boost::bind(&ScriptFunctions::getWaypointByTag, this, _1),
			createSignature(2, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetTransitionTarget", 198,
			boost::bind(&ScriptFunctions::getTransitionTarget, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("EffectLinkEffects", 199,
			boost::bind(&ScriptFunctions::effectLinkEffects, this, _1),
			createSignature(3, kTypeEngineType, kTypeEngineType, kTypeEngineType));
}

void ScriptFunctions::d10(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 10, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d12(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 12, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d20(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 20, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d100(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 100, ctx.getParams()[0].getInt());
}

void ScriptFunctions::vectorMagnitude(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: VectorMagnitude");
}

void ScriptFunctions::getMetaMagicFeat(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetMetaMagicFeat");
}

void ScriptFunctions::getObjectType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kObjectTypeNone;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	ctx.getReturn() = (int32) object->getType();
}

void ScriptFunctions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kRaceInvalid;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getRace();
}

void ScriptFunctions::fortitudeSave(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FortitudeSave");
}

void ScriptFunctions::reflexSave(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ReflexSave");
}

void ScriptFunctions::willSave(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: WillSave");
}

void ScriptFunctions::getSpellSaveDC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSpellSaveDC");
}

void ScriptFunctions::magicalEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: MagicalEffect");
}

void ScriptFunctions::supernaturalEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SupernaturalEffect");
}

void ScriptFunctions::extraordinaryEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ExtraordinaryEffect");
}

void ScriptFunctions::effectACIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectACIncrease");
}

void ScriptFunctions::getAC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetAC");
}

void ScriptFunctions::effectSavingThrowIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSavingThrowIncrease");
}

void ScriptFunctions::effectAttackIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectAttackIncrease");
}

void ScriptFunctions::effectDamageReduction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDamageReduction");
}

void ScriptFunctions::effectDamageIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDamageIncrease");
}

void ScriptFunctions::roundsToSeconds(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RoundsToSeconds");
}

void ScriptFunctions::hoursToSeconds(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: HoursToSeconds");
}

void ScriptFunctions::turnsToSeconds(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: TurnsToSeconds");
}

void ScriptFunctions::getLawChaosValue(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getLawChaos();
}

void ScriptFunctions::getGoodEvilValue(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getGoodEvil();
}

void ScriptFunctions::getAlignmentLawChaos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) NWN::getAlignmentLawChaos(creature->getLawChaos());
}

void ScriptFunctions::getAlignmentGoodEvil(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) NWN::getAlignmentGoodEvil(creature->getGoodEvil());
}

void ScriptFunctions::getFirstObjectInShape(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFirstObjectInShape");
}

void ScriptFunctions::getNextObjectInShape(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNextObjectInShape");
}

void ScriptFunctions::effectEntangle(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectEntangle");
}

void ScriptFunctions::signalEvent(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SignalEvent");
}

void ScriptFunctions::eventUserDefined(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EventUserDefined");
}

void ScriptFunctions::effectDeath(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDeath");
}

void ScriptFunctions::effectKnockdown(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectKnockdown");
}

void ScriptFunctions::actionGiveItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionGiveItem");
}

void ScriptFunctions::actionTakeItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionTakeItem");
}

void ScriptFunctions::vectorNormalize(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: VectorNormalize");
}

void ScriptFunctions::effectCurse(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectCurse");
}

void ScriptFunctions::getAbilityScore(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getAbilityScore(): nBaseAbilityScore

	ctx.getReturn() = 0;

	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Creature *creature = convertCreature(params[0].getObject());
	Ability   ability  = (Ability) params[1].getInt();

	if (creature)
		ctx.getReturn() = (int32) creature->getAbility(ability);
}

void ScriptFunctions::getIsDead(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (!creature)
		return;

	ctx.getReturn() = creature->getCurrentHP() <= 0;
}

void ScriptFunctions::printVector(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	bool prepend = ctx.getParams()[1].getInt() != 0;

	status("NWN: %s%f, %f, %f", prepend ? "PRINTVECTOR:" : "", x, y, z);
}

void ScriptFunctions::vector(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(ctx.getParams()[0].getFloat(),
	                          ctx.getParams()[1].getFloat(),
	                          ctx.getParams()[2].getFloat());
}

void ScriptFunctions::setFacingPoint(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	warning("TODO: SetFacingPoint: \"%s\" to %f, %f, %f", object->getTag().c_str(), x, y, z);
}

void ScriptFunctions::angleToVector(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AngleToVector");
}

void ScriptFunctions::vectorToAngle(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: VectorToAngle");
}

void ScriptFunctions::touchAttackMelee(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: TouchAttackMelee");
}

void ScriptFunctions::touchAttackRanged(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: TouchAttackRanged");
}

void ScriptFunctions::effectParalyze(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectParalyze");
}

void ScriptFunctions::effectSpellImmunity(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSpellImmunity");
}

void ScriptFunctions::effectDeaf(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDeaf");
}

void ScriptFunctions::getDistanceBetween(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDistanceBetween");
}

void ScriptFunctions::setLocalLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetLocalLocation");
}

void ScriptFunctions::getLocalLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLocalLocation");
}

void ScriptFunctions::effectSleep(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSleep");
}

void ScriptFunctions::getItemInSlot(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetItemInSlot");
}

void ScriptFunctions::effectCharmed(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectCharmed");
}

void ScriptFunctions::effectConfused(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectConfused");
}

void ScriptFunctions::effectFrightened(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectFrightened");
}

void ScriptFunctions::effectDominated(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDominated");
}

void ScriptFunctions::effectDazed(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDazed");
}

void ScriptFunctions::effectStunned(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectStunned");
}

void ScriptFunctions::setCommandable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = convertCreature(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	creature->setCommandable(ctx.getParams()[0].getInt() != 0);
}

void ScriptFunctions::getCommandable(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	ctx.getReturn() = creature->isCommandable();
}

void ScriptFunctions::effectRegenerate(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectRegenerate");
}

void ScriptFunctions::effectMovementSpeedIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectMovementSpeedIncrease");
}

void ScriptFunctions::getHitDice(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = creature->getHitDice();
}

void ScriptFunctions::actionForceFollowObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionForceFollowObject");
}

void ScriptFunctions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object)
		ctx.getReturn() = object->getTag();
}

void ScriptFunctions::resistSpell(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ResistSpell");
}

void ScriptFunctions::getEffectType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEffectType");
}

void ScriptFunctions::effectAreaOfEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectAreaOfEffect");
}

void ScriptFunctions::getFactionEqual(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionEqual");
}

void ScriptFunctions::changeFaction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ChangeFaction");
}

void ScriptFunctions::getIsListening(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsListening");
}

void ScriptFunctions::setListening(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetListening");
}

void ScriptFunctions::setListenPattern(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetListenPattern");
}

void ScriptFunctions::testStringAgainstPattern(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: TestStringAgainstPattern");
}

void ScriptFunctions::getMatchedSubstring(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetMatchedSubstring");
}

void ScriptFunctions::getMatchedSubstringsCount(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetMatchedSubstringsCount");
}

void ScriptFunctions::effectVisualEffect(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::EngineType *) 0;

	int  effectID = ctx.getParams()[0].getInt();
	bool miss     = ctx.getParams()[1].getInt() != 0;

	warning("TODO: EffectVisualEffect: %d, %d", effectID, miss);
}

void ScriptFunctions::getFactionWeakestMember(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionWeakestMember");
}

void ScriptFunctions::getFactionStrongestMember(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionStrongestMember");
}

void ScriptFunctions::getFactionMostDamagedMember(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionMostDamagedMember");
}

void ScriptFunctions::getFactionLeastDamagedMember(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionLeastDamagedMember");
}

void ScriptFunctions::getFactionGold(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionGold");
}

void ScriptFunctions::getFactionAverageReputation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionAverageReputation");
}

void ScriptFunctions::getFactionAverageGoodEvilAlignment(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionAverageGoodEvilAlignment");
}

void ScriptFunctions::getFactionAverageLawChaosAlignment(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionAverageLawChaosAlignment");
}

void ScriptFunctions::getFactionAverageLevel(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionAverageLevel");
}

void ScriptFunctions::getFactionAverageXP(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionAverageXP");
}

void ScriptFunctions::getFactionMostFrequentClass(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionMostFrequentClass");
}

void ScriptFunctions::getFactionWorstAC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionWorstAC");
}

void ScriptFunctions::getFactionBestAC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionBestAC");
}

void ScriptFunctions::actionSit(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionSit");
}

void ScriptFunctions::getListenPatternNumber(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	warning("TODO: GetListenPatternNumber");
}

void ScriptFunctions::actionJumpToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionJumpToObject(): /Action/

	// TODO: ScriptFunctions::actionJumpToObject(): walkStraightLineToPoint
	// bool walkStraightLineToPoint = ctx.getParams()[1].getInt() != 0;

	Object *object = convertObject(ctx.getCaller());
	Object *moveTo = convertObject(ctx.getParams()[0].getObject());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);
}

void ScriptFunctions::getWaypointByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Module *module = _engine->getModule();
	if (!module)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	Aurora::NWScript::ObjectSearch *search = module->findObjectsByTag(tag);
	Aurora::NWScript::Object       *object = 0;

	while ((object = search->next())) {
		Waypoint *waypoint = convertWaypoint(object);

		if (waypoint) {
			ctx.getReturn() = waypoint;
			break;
		}
	}

	delete search;
}

void ScriptFunctions::getTransitionTarget(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTransitionTarget");
}

void ScriptFunctions::effectLinkEffects(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectLinkEffects");
}

} // End of namespace NWN

} // End of namespace Engines
