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
 */

/** @file engines/nwn/script/functions_400.cpp
 *  NWN script functions, 400-499.
 */

#include <boost/bind.hpp>

#include "common/util.h"
#include "common/error.h"

#include "aurora/ssffile.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "engines/nwn/types.h"

#include "engines/nwn/area.h"
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

void ScriptFunctions::registerFunctions400(const Defaults &d) {
	FunctionMan.registerFunction("ActionEquipMostDamagingRanged", 400,
			boost::bind(&ScriptFunctions::actionEquipMostDamagingRanged, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetItemACValue", 401,
			boost::bind(&ScriptFunctions::getItemACValue, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("ActionRest", 402,
			boost::bind(&ScriptFunctions::actionRest, this, _1),
			createSignature(2, kTypeVoid, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("ExploreAreaForPlayer", 403,
			boost::bind(&ScriptFunctions::exploreAreaForPlayer, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("ActionEquipMostEffectiveArmor", 404,
			boost::bind(&ScriptFunctions::actionEquipMostEffectiveArmor, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("GetIsDay", 405,
			boost::bind(&ScriptFunctions::getIsDay, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetIsNight", 406,
			boost::bind(&ScriptFunctions::getIsNight, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetIsDawn", 407,
			boost::bind(&ScriptFunctions::getIsDawn, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetIsDusk", 408,
			boost::bind(&ScriptFunctions::getIsDusk, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetIsEncounterCreature", 409,
			boost::bind(&ScriptFunctions::getIsEncounterCreature, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastPlayerDying", 410,
			boost::bind(&ScriptFunctions::getLastPlayerDying, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetStartingLocation", 411,
			boost::bind(&ScriptFunctions::getStartingLocation, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("ChangeToStandardFaction", 412,
			boost::bind(&ScriptFunctions::changeToStandardFaction, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SoundObjectPlay", 413,
			boost::bind(&ScriptFunctions::soundObjectPlay, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("SoundObjectStop", 414,
			boost::bind(&ScriptFunctions::soundObjectStop, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("SoundObjectSetVolume", 415,
			boost::bind(&ScriptFunctions::soundObjectSetVolume, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SoundObjectSetPosition", 416,
			boost::bind(&ScriptFunctions::soundObjectSetPosition, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeVector));
	FunctionMan.registerFunction("SpeakOneLinerConversation", 417,
			boost::bind(&ScriptFunctions::speakOneLinerConversation, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject),
			createDefaults(2, d.stringEmpty, d.object0));
	FunctionMan.registerFunction("GetGold", 418,
			boost::bind(&ScriptFunctions::getGold, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastRespawnButtonPresser", 419,
			boost::bind(&ScriptFunctions::getLastRespawnButtonPresser, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetIsDM", 420,
			boost::bind(&ScriptFunctions::getIsDM, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("PlayVoiceChat", 421,
			boost::bind(&ScriptFunctions::playVoiceChat, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsWeaponEffective", 422,
			boost::bind(&ScriptFunctions::getIsWeaponEffective, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int0));
	FunctionMan.registerFunction("GetLastSpellHarmful", 423,
			boost::bind(&ScriptFunctions::getLastSpellHarmful, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("EventActivateItem", 424,
			boost::bind(&ScriptFunctions::eventActivateItem, this, _1),
			createSignature(4, kTypeEngineType, kTypeObject, kTypeEngineType, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("MusicBackgroundPlay", 425,
			boost::bind(&ScriptFunctions::musicBackgroundPlay, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("MusicBackgroundStop", 426,
			boost::bind(&ScriptFunctions::musicBackgroundStop, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("MusicBackgroundSetDelay", 427,
			boost::bind(&ScriptFunctions::musicBackgroundSetDelay, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("MusicBackgroundChangeDay", 428,
			boost::bind(&ScriptFunctions::musicBackgroundChangeDay, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("MusicBackgroundChangeNight", 429,
			boost::bind(&ScriptFunctions::musicBackgroundChangeNight, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("MusicBattlePlay", 430,
			boost::bind(&ScriptFunctions::musicBattlePlay, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("MusicBattleStop", 431,
			boost::bind(&ScriptFunctions::musicBattleStop, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("MusicBattleChange", 432,
			boost::bind(&ScriptFunctions::musicBattleChange, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("AmbientSoundPlay", 433,
			boost::bind(&ScriptFunctions::ambientSoundPlay, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("AmbientSoundStop", 434,
			boost::bind(&ScriptFunctions::ambientSoundStop, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("AmbientSoundChangeDay", 435,
			boost::bind(&ScriptFunctions::ambientSoundChangeDay, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("AmbientSoundChangeNight", 436,
			boost::bind(&ScriptFunctions::ambientSoundChangeNight, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetLastKiller", 437,
			boost::bind(&ScriptFunctions::getLastKiller, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetSpellCastItem", 438,
			boost::bind(&ScriptFunctions::getSpellCastItem, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetItemActivated", 439,
			boost::bind(&ScriptFunctions::getItemActivated, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetItemActivator", 440,
			boost::bind(&ScriptFunctions::getItemActivator, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetItemActivatedTargetLocation", 441,
			boost::bind(&ScriptFunctions::getItemActivatedTargetLocation, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetItemActivatedTarget", 442,
			boost::bind(&ScriptFunctions::getItemActivatedTarget, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetIsOpen", 443,
			boost::bind(&ScriptFunctions::getIsOpen, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("TakeGoldFromCreature", 444,
			boost::bind(&ScriptFunctions::takeGoldFromCreature, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("IsInConversation", 445,
			boost::bind(&ScriptFunctions::isInConversation, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("EffectAbilityDecrease", 446,
			boost::bind(&ScriptFunctions::effectAbilityDecrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectAttackDecrease", 447,
			boost::bind(&ScriptFunctions::effectAttackDecrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intAttackMisc));
	FunctionMan.registerFunction("EffectDamageDecrease", 448,
			boost::bind(&ScriptFunctions::effectDamageDecrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intDamageMagical));
	FunctionMan.registerFunction("EffectDamageImmunityDecrease", 449,
			boost::bind(&ScriptFunctions::effectDamageImmunityDecrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectACDecrease", 450,
			boost::bind(&ScriptFunctions::effectACDecrease, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(2, d.intACDodge, d.intACAll));
	FunctionMan.registerFunction("EffectMovementSpeedDecrease", 451,
			boost::bind(&ScriptFunctions::effectMovementSpeedDecrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectSavingThrowDecrease", 452,
			boost::bind(&ScriptFunctions::effectSavingThrowDecrease, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.intSaveAll));
	FunctionMan.registerFunction("EffectSkillDecrease", 453,
			boost::bind(&ScriptFunctions::effectSkillDecrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectSpellResistanceDecrease", 454,
			boost::bind(&ScriptFunctions::effectSpellResistanceDecrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetPlotFlag", 455,
			boost::bind(&ScriptFunctions::getPlotFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetPlotFlag", 456,
			boost::bind(&ScriptFunctions::setPlotFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("EffectInvisibility", 457,
			boost::bind(&ScriptFunctions::effectInvisibility, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectConcealment", 458,
			boost::bind(&ScriptFunctions::effectConcealment, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intMissNormal));
	FunctionMan.registerFunction("EffectDarkness", 459,
			boost::bind(&ScriptFunctions::effectDarkness, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectDispelMagicAll", 460,
			boost::bind(&ScriptFunctions::effectDispelMagicAll, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.intCreatureLevel));
	FunctionMan.registerFunction("EffectUltravision", 461,
			boost::bind(&ScriptFunctions::effectUltravision, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectNegativeLevel", 462,
			boost::bind(&ScriptFunctions::effectNegativeLevel, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("EffectPolymorph", 463,
			boost::bind(&ScriptFunctions::effectPolymorph, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("EffectSanctuary", 464,
			boost::bind(&ScriptFunctions::effectSanctuary, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectTrueSeeing", 465,
			boost::bind(&ScriptFunctions::effectTrueSeeing, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectSeeInvisible", 466,
			boost::bind(&ScriptFunctions::effectSeeInvisible, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectTimeStop", 467,
			boost::bind(&ScriptFunctions::effectTimeStop, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectBlindness", 468,
			boost::bind(&ScriptFunctions::effectBlindness, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetIsReactionTypeFriendly", 469,
			boost::bind(&ScriptFunctions::getIsReactionTypeFriendly, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsReactionTypeNeutral", 470,
			boost::bind(&ScriptFunctions::getIsReactionTypeNeutral, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsReactionTypeHostile", 471,
			boost::bind(&ScriptFunctions::getIsReactionTypeHostile, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectSpellLevelAbsorption", 472,
			boost::bind(&ScriptFunctions::effectSpellLevelAbsorption, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(2, d.int0, d.intSpellsGeneral));
	FunctionMan.registerFunction("EffectDispelMagicBest", 473,
			boost::bind(&ScriptFunctions::effectDispelMagicBest, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.intCreatureLevel));
	FunctionMan.registerFunction("ActivatePortal", 474,
			boost::bind(&ScriptFunctions::activatePortal, this, _1),
			createSignature(6, kTypeVoid, kTypeObject, kTypeString, kTypeString, kTypeString, kTypeInt),
			createDefaults(4, d.stringEmpty, d.stringEmpty, d.stringEmpty, d.int0));
	FunctionMan.registerFunction("GetNumStackedItems", 475,
			boost::bind(&ScriptFunctions::getNumStackedItems, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SurrenderToEnemies", 476,
			boost::bind(&ScriptFunctions::surrenderToEnemies, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("EffectMissChance", 477,
			boost::bind(&ScriptFunctions::effectMissChance, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt),
			createDefaults(1, d.intMissNormal));
	FunctionMan.registerFunction("GetTurnResistanceHD", 478,
			boost::bind(&ScriptFunctions::getTurnResistanceHD, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCreatureSize", 479,
			boost::bind(&ScriptFunctions::getCreatureSize, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("EffectDisappearAppear", 480,
			boost::bind(&ScriptFunctions::effectDisappearAppear, this, _1),
			createSignature(3, kTypeEngineType, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("EffectDisappear", 481,
			boost::bind(&ScriptFunctions::effectDisappear, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("EffectAppear", 482,
			boost::bind(&ScriptFunctions::effectAppear, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("ActionUnlockObject", 483,
			boost::bind(&ScriptFunctions::actionUnlockObject, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ActionLockObject", 484,
			boost::bind(&ScriptFunctions::actionLockObject, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("EffectModifyAttacks", 485,
			boost::bind(&ScriptFunctions::effectModifyAttacks, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("GetLastTrapDetected", 486,
			boost::bind(&ScriptFunctions::getLastTrapDetected, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("EffectDamageShield", 487,
			boost::bind(&ScriptFunctions::effectDamageShield, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetNearestTrapToObject", 488,
			boost::bind(&ScriptFunctions::getNearestTrapToObject, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetDeity", 489,
			boost::bind(&ScriptFunctions::getDeity, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetSubRace", 490,
			boost::bind(&ScriptFunctions::getSubRace, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetFortitudeSavingThrow", 491,
			boost::bind(&ScriptFunctions::getFortitudeSavingThrow, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetWillSavingThrow", 492,
			boost::bind(&ScriptFunctions::getWillSavingThrow, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetReflexSavingThrow", 493,
			boost::bind(&ScriptFunctions::getReflexSavingThrow, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetChallengeRating", 494,
			boost::bind(&ScriptFunctions::getChallengeRating, this, _1),
			createSignature(2, kTypeFloat, kTypeObject));
	FunctionMan.registerFunction("GetAge", 495,
			boost::bind(&ScriptFunctions::getAge, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetMovementRate", 496,
			boost::bind(&ScriptFunctions::getMovementRate, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFamiliarCreatureType", 497,
			boost::bind(&ScriptFunctions::getFamiliarCreatureType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetAnimalCompanionCreatureType", 498,
			boost::bind(&ScriptFunctions::getAnimalCompanionCreatureType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFamiliarName", 499,
			boost::bind(&ScriptFunctions::getFamiliarName, this, _1),
			createSignature(2, kTypeString, kTypeObject));
}

void ScriptFunctions::actionEquipMostDamagingRanged(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionEquipMostDamagingRanged");
}

void ScriptFunctions::getItemACValue(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemACValue");
}

void ScriptFunctions::actionRest(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionRest");
}

void ScriptFunctions::exploreAreaForPlayer(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ExploreAreaForPlayer");
}

void ScriptFunctions::actionEquipMostEffectiveArmor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionEquipMostEffectiveArmor");
}

void ScriptFunctions::getIsDay(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getIsDay()

	ctx.getReturn() = (int32) true;
}

void ScriptFunctions::getIsNight(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getIsNight()

	ctx.getReturn() = (int32) false;
}

void ScriptFunctions::getIsDawn(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getIsDawn()

	ctx.getReturn() = (int32) false;
}

void ScriptFunctions::getIsDusk(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getIsDusk()

	ctx.getReturn() = (int32) false;
}

void ScriptFunctions::getIsEncounterCreature(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsEncounterCreature");
}

void ScriptFunctions::getLastPlayerDying(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastPlayerDying");
}

void ScriptFunctions::getStartingLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetStartingLocation");
}

void ScriptFunctions::changeToStandardFaction(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ChangeToStandardFaction");
}

void ScriptFunctions::soundObjectPlay(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	warning("TODO: SoundObjectPlay: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::soundObjectStop(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	warning("TODO: SoundObjectStop: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::soundObjectSetVolume(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	int volume = ctx.getParams()[1].getInt();
	warning("TODO: SoundObjectSetVolume: \"%s\" to %d", object->getTag().c_str(), volume);
}

void ScriptFunctions::soundObjectSetPosition(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	float x, y, z;
	ctx.getParams()[1].getVector(x, y, z);

	warning("TODO: SoundObjectSetPosition: \"%s\" to [%f, %f, %f]",
	        object->getTag().c_str(), x, y, z);
}

void ScriptFunctions::speakOneLinerConversation(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &dlg = ctx.getParams()[0].getString();
	Object *tokenTarget = convertObject(ctx.getParams()[1].getObject());

	object->speakOneLiner(dlg, tokenTarget);
}

void ScriptFunctions::getGold(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());

	if (!object)
		return;

	warning("TODO: GetGold: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getLastRespawnButtonPresser(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastRespawnButtonPresser");
}

void ScriptFunctions::getIsDM(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (!creature)
		return;

	warning("TODO: GetIsDM: \"%s\"", creature->getTag().c_str());
}

void ScriptFunctions::playVoiceChat(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	const Aurora::SSFFile *ssf = object->getSSF();
	if (!ssf)
		return;

	object->playSound(ssf->getSound(ctx.getParams()[0].getInt()).fileName, true);
}

void ScriptFunctions::getIsWeaponEffective(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsWeaponEffective");
}

void ScriptFunctions::getLastSpellHarmful(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastSpellHarmful");
}

void ScriptFunctions::eventActivateItem(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EventActivateItem");
}

void ScriptFunctions::musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		area->playAmbientMusic();
}

void ScriptFunctions::musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		area->stopAmbientMusic();
}

void ScriptFunctions::musicBackgroundSetDelay(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: MusicBackgroundSetDelay");
}

void ScriptFunctions::musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		area->setMusicDayTrack(ctx.getParams()[1].getInt());
}

void ScriptFunctions::musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		area->setMusicNightTrack(ctx.getParams()[1].getInt());
}

void ScriptFunctions::musicBattlePlay(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: MusicBattlePlay");
}

void ScriptFunctions::musicBattleStop(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: MusicBattleStop");
}

void ScriptFunctions::musicBattleChange(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: MusicBattleChange");
}

void ScriptFunctions::ambientSoundPlay(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: AmbientSoundPlay");
}

void ScriptFunctions::ambientSoundStop(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: AmbientSoundStop");
}

void ScriptFunctions::ambientSoundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: AmbientSoundChangeDay");
}

void ScriptFunctions::ambientSoundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: AmbientSoundChangeNight");
}

void ScriptFunctions::getLastKiller(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastKiller");
}

void ScriptFunctions::getSpellCastItem(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetSpellCastItem");
}

void ScriptFunctions::getItemActivated(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemActivated");
}

void ScriptFunctions::getItemActivator(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemActivator");
}

void ScriptFunctions::getItemActivatedTargetLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemActivatedTargetLocation");
}

void ScriptFunctions::getItemActivatedTarget(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetItemActivatedTarget");
}

void ScriptFunctions::getIsOpen(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Situated *situated = convertSituated(ctx.getParams()[0].getObject());
	if (!situated)
		return;

	ctx.getReturn() = situated->isOpen();
}

void ScriptFunctions::takeGoldFromCreature(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: TakeGoldFromCreature");
}

void ScriptFunctions::isInConversation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (object)
		ctx.getReturn() = object->getPCSpeaker() != 0;
}

void ScriptFunctions::effectAbilityDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectAbilityDecrease");
}

void ScriptFunctions::effectAttackDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectAttackDecrease");
}

void ScriptFunctions::effectDamageDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDamageDecrease");
}

void ScriptFunctions::effectDamageImmunityDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDamageImmunityDecrease");
}

void ScriptFunctions::effectACDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectACDecrease");
}

void ScriptFunctions::effectMovementSpeedDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectMovementSpeedDecrease");
}

void ScriptFunctions::effectSavingThrowDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSavingThrowDecrease");
}

void ScriptFunctions::effectSkillDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSkillDecrease");
}

void ScriptFunctions::effectSpellResistanceDecrease(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSpellResistanceDecrease");
}

void ScriptFunctions::getPlotFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPlotFlag");
}

void ScriptFunctions::setPlotFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPlotFlag");
}

void ScriptFunctions::effectInvisibility(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectInvisibility");
}

void ScriptFunctions::effectConcealment(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectConcealment");
}

void ScriptFunctions::effectDarkness(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDarkness");
}

void ScriptFunctions::effectDispelMagicAll(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDispelMagicAll");
}

void ScriptFunctions::effectUltravision(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectUltravision");
}

void ScriptFunctions::effectNegativeLevel(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectNegativeLevel");
}

void ScriptFunctions::effectPolymorph(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectPolymorph");
}

void ScriptFunctions::effectSanctuary(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSanctuary");
}

void ScriptFunctions::effectTrueSeeing(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectTrueSeeing");
}

void ScriptFunctions::effectSeeInvisible(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSeeInvisible");
}

void ScriptFunctions::effectTimeStop(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectTimeStop");
}

void ScriptFunctions::effectBlindness(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectBlindness");
}

void ScriptFunctions::getIsReactionTypeFriendly(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsReactionTypeFriendly");
}

void ScriptFunctions::getIsReactionTypeNeutral(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsReactionTypeNeutral");
}

void ScriptFunctions::getIsReactionTypeHostile(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetIsReactionTypeHostile");
}

void ScriptFunctions::effectSpellLevelAbsorption(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectSpellLevelAbsorption");
}

void ScriptFunctions::effectDispelMagicBest(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDispelMagicBest");
}

void ScriptFunctions::activatePortal(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActivatePortal");
}

void ScriptFunctions::getNumStackedItems(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetNumStackedItems");
}

void ScriptFunctions::surrenderToEnemies(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SurrenderToEnemies");
}

void ScriptFunctions::effectMissChance(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectMissChance");
}

void ScriptFunctions::getTurnResistanceHD(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTurnResistanceHD");
}

void ScriptFunctions::getCreatureSize(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetCreatureSize");
}

void ScriptFunctions::effectDisappearAppear(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDisappearAppear");
}

void ScriptFunctions::effectDisappear(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDisappear");
}

void ScriptFunctions::effectAppear(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectAppear");
}

void ScriptFunctions::actionUnlockObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionUnlockObject");
}

void ScriptFunctions::actionLockObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ActionLockObject");
}

void ScriptFunctions::effectModifyAttacks(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectModifyAttacks");
}

void ScriptFunctions::getLastTrapDetected(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetLastTrapDetected");
}

void ScriptFunctions::effectDamageShield(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: EffectDamageShield");
}

void ScriptFunctions::getNearestTrapToObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetNearestTrapToObject");
}

void ScriptFunctions::getDeity(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetDeity");
}

void ScriptFunctions::getSubRace(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetSubRace");
}

void ScriptFunctions::getFortitudeSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFortitudeSavingThrow");
}

void ScriptFunctions::getWillSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetWillSavingThrow");
}

void ScriptFunctions::getReflexSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetReflexSavingThrow");
}

void ScriptFunctions::getChallengeRating(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetChallengeRating");
}

void ScriptFunctions::getAge(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAge");
}

void ScriptFunctions::getMovementRate(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetMovementRate");
}

void ScriptFunctions::getFamiliarCreatureType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFamiliarCreatureType");
}

void ScriptFunctions::getAnimalCompanionCreatureType(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAnimalCompanionCreatureType");
}

void ScriptFunctions::getFamiliarName(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetFamiliarName");
}

} // End of namespace NWN

} // End of namespace Engines
