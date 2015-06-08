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
 *  NWN script functions, 500-599.
 */

#include <boost/bind.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/configman.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"
#include "src/engines/nwn/object.h"

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

// boost-date_time stuff
using boost::posix_time::ptime;
using boost::posix_time::second_clock;

namespace Engines {

namespace NWN {

void ScriptFunctions::registerFunctions500(const Defaults &d) {
	FunctionMan.registerFunction("GetAnimalCompanionName", 500,
			boost::bind(&ScriptFunctions::getAnimalCompanionName, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("ActionCastFakeSpellAtObject", 501,
			boost::bind(&ScriptFunctions::actionCastFakeSpellAtObject, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.intProjPathDefault));
	FunctionMan.registerFunction("ActionCastFakeSpellAtLocation", 502,
			boost::bind(&ScriptFunctions::actionCastFakeSpellAtLocation, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeEngineType, kTypeInt),
			createDefaults(1, d.intProjPathDefault));
	FunctionMan.registerFunction("RemoveSummonedAssociate", 503,
			boost::bind(&ScriptFunctions::removeSummonedAssociate, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCameraMode", 504,
			boost::bind(&ScriptFunctions::setCameraMode, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetIsResting", 505,
			boost::bind(&ScriptFunctions::getIsResting, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLastPCRested", 506,
			boost::bind(&ScriptFunctions::getLastPCRested, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("SetWeather", 507,
			boost::bind(&ScriptFunctions::setWeather, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetLastRestEventType", 508,
			boost::bind(&ScriptFunctions::getLastRestEventType, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("StartNewModule", 509,
			boost::bind(&ScriptFunctions::startNewModule, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("EffectSwarm", 510,
			boost::bind(&ScriptFunctions::effectSwarm, this, _1),
			createSignature(6, kTypeEngineType, kTypeInt, kTypeString, kTypeString, kTypeString, kTypeString),
			createDefaults(3, d.stringEmpty, d.stringEmpty, d.stringEmpty));
	FunctionMan.registerFunction("GetWeaponRanged", 511,
			boost::bind(&ScriptFunctions::getWeaponRanged, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("DoSinglePlayerAutoSave", 512,
			boost::bind(&ScriptFunctions::doSinglePlayerAutoSave, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("GetGameDifficulty", 513,
			boost::bind(&ScriptFunctions::getGameDifficulty, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("SetTileMainLightColor", 514,
			boost::bind(&ScriptFunctions::setTileMainLightColor, this, _1),
			createSignature(4, kTypeVoid, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("SetTileSourceLightColor", 515,
			boost::bind(&ScriptFunctions::setTileSourceLightColor, this, _1),
			createSignature(4, kTypeVoid, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("RecomputeStaticLighting", 516,
			boost::bind(&ScriptFunctions::recomputeStaticLighting, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetTileMainLight1Color", 517,
			boost::bind(&ScriptFunctions::getTileMainLight1Color, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetTileMainLight2Color", 518,
			boost::bind(&ScriptFunctions::getTileMainLight2Color, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetTileSourceLight1Color", 519,
			boost::bind(&ScriptFunctions::getTileSourceLight1Color, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetTileSourceLight2Color", 520,
			boost::bind(&ScriptFunctions::getTileSourceLight2Color, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("SetPanelButtonFlash", 521,
			boost::bind(&ScriptFunctions::setPanelButtonFlash, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetCurrentAction", 522,
			boost::bind(&ScriptFunctions::getCurrentAction, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetStandardFactionReputation", 523,
			boost::bind(&ScriptFunctions::setStandardFactionReputation, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetStandardFactionReputation", 524,
			boost::bind(&ScriptFunctions::getStandardFactionReputation, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("FloatingTextStrRefOnCreature", 525,
			boost::bind(&ScriptFunctions::floatingTextStrRefOnCreature, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("FloatingTextStringOnCreature", 526,
			boost::bind(&ScriptFunctions::floatingTextStringOnCreature, this, _1),
			createSignature(4, kTypeVoid, kTypeString, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetTrapDisarmable", 527,
			boost::bind(&ScriptFunctions::getTrapDisarmable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapDetectable", 528,
			boost::bind(&ScriptFunctions::getTrapDetectable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapDetectedBy", 529,
			boost::bind(&ScriptFunctions::getTrapDetectedBy, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetTrapFlagged", 530,
			boost::bind(&ScriptFunctions::getTrapFlagged, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapBaseType", 531,
			boost::bind(&ScriptFunctions::getTrapBaseType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapOneShot", 532,
			boost::bind(&ScriptFunctions::getTrapOneShot, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapCreator", 533,
			boost::bind(&ScriptFunctions::getTrapCreator, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetTrapKeyTag", 534,
			boost::bind(&ScriptFunctions::getTrapKeyTag, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetTrapDisarmDC", 535,
			boost::bind(&ScriptFunctions::getTrapDisarmDC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetTrapDetectDC", 536,
			boost::bind(&ScriptFunctions::getTrapDetectDC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetLockKeyRequired", 537,
			boost::bind(&ScriptFunctions::getLockKeyRequired, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetLockKeyTag", 538,
			boost::bind(&ScriptFunctions::getLockKeyTag, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetLockLockable", 539,
			boost::bind(&ScriptFunctions::getLockLockable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetLockUnlockDC", 540,
			boost::bind(&ScriptFunctions::getLockUnlockDC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetLockLockDC", 541,
			boost::bind(&ScriptFunctions::getLockLockDC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetPCLevellingUp", 542,
			boost::bind(&ScriptFunctions::getPCLevellingUp, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetHasFeatEffect", 543,
			boost::bind(&ScriptFunctions::getHasFeatEffect, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetPlaceableIllumination", 544,
			boost::bind(&ScriptFunctions::setPlaceableIllumination, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int1));
	FunctionMan.registerFunction("GetPlaceableIllumination", 545,
			boost::bind(&ScriptFunctions::getPlaceableIllumination, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetIsPlaceableObjectActionPossible", 546,
			boost::bind(&ScriptFunctions::getIsPlaceableObjectActionPossible, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("DoPlaceableObjectAction", 547,
			boost::bind(&ScriptFunctions::doPlaceableObjectAction, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetFirstPC", 548,
			boost::bind(&ScriptFunctions::getFirstPC, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetNextPC", 549,
			boost::bind(&ScriptFunctions::getNextPC, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("SetTrapDetectedBy", 550,
			boost::bind(&ScriptFunctions::setTrapDetectedBy, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetIsTrapped", 551,
			boost::bind(&ScriptFunctions::getIsTrapped, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("EffectTurnResistanceDecrease", 552,
			boost::bind(&ScriptFunctions::effectTurnResistanceDecrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectTurnResistanceIncrease", 553,
			boost::bind(&ScriptFunctions::effectTurnResistanceIncrease, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("PopUpDeathGUIPanel", 554,
			boost::bind(&ScriptFunctions::popUpDeathGUIPanel, this, _1),
			createSignature(6, kTypeVoid, kTypeObject, kTypeInt, kTypeInt, kTypeInt, kTypeString),
			createDefaults(4, d.int1, d.int1, d.int0, d.stringEmpty));
	FunctionMan.registerFunction("SetTrapDisabled", 555,
			boost::bind(&ScriptFunctions::setTrapDisabled, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetLastHostileActor", 556,
			boost::bind(&ScriptFunctions::getLastHostileActor, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("ExportAllCharacters", 557,
			boost::bind(&ScriptFunctions::exportAllCharacters, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("MusicBackgroundGetDayTrack", 558,
			boost::bind(&ScriptFunctions::musicBackgroundGetDayTrack, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("MusicBackgroundGetNightTrack", 559,
			boost::bind(&ScriptFunctions::musicBackgroundGetNightTrack, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("WriteTimestampedLogEntry", 560,
			boost::bind(&ScriptFunctions::writeTimestampedLogEntry, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("GetModuleName", 561,
			boost::bind(&ScriptFunctions::getModuleName, this, _1),
			createSignature(1, kTypeString));
	FunctionMan.registerFunction("GetFactionLeader", 562,
			boost::bind(&ScriptFunctions::getFactionLeader, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("SendMessageToAllDMs", 563,
			boost::bind(&ScriptFunctions::sendMessageToAllDMs, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("EndGame", 564,
			boost::bind(&ScriptFunctions::endGame, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("BootPC", 565,
			boost::bind(&ScriptFunctions::bootPC, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ActionCounterSpell", 566,
			boost::bind(&ScriptFunctions::actionCounterSpell, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("AmbientSoundSetDayVolume", 567,
			boost::bind(&ScriptFunctions::ambientSoundSetDayVolume, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("AmbientSoundSetNightVolume", 568,
			boost::bind(&ScriptFunctions::ambientSoundSetNightVolume, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("MusicBackgroundGetBattleTrack", 569,
			boost::bind(&ScriptFunctions::musicBackgroundGetBattleTrack, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetHasInventory", 570,
			boost::bind(&ScriptFunctions::getHasInventory, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetStrRefSoundDuration", 571,
			boost::bind(&ScriptFunctions::getStrRefSoundDuration, this, _1),
			createSignature(2, kTypeFloat, kTypeInt));
	FunctionMan.registerFunction("AddToParty", 572,
			boost::bind(&ScriptFunctions::addToParty, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("RemoveFromParty", 573,
			boost::bind(&ScriptFunctions::removeFromParty, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetStealthMode", 574,
			boost::bind(&ScriptFunctions::getStealthMode, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetDetectMode", 575,
			boost::bind(&ScriptFunctions::getDetectMode, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetDefensiveCastingMode", 576,
			boost::bind(&ScriptFunctions::getDefensiveCastingMode, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetAppearanceType", 577,
			boost::bind(&ScriptFunctions::getAppearanceType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SpawnScriptDebugger", 578,
			boost::bind(&ScriptFunctions::spawnScriptDebugger, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("GetModuleItemAcquiredStackSize", 579,
			boost::bind(&ScriptFunctions::getModuleItemAcquiredStackSize, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("DecrementRemainingFeatUses", 580,
			boost::bind(&ScriptFunctions::decrementRemainingFeatUses, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("DecrementRemainingSpellUses", 581,
			boost::bind(&ScriptFunctions::decrementRemainingSpellUses, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetResRef", 582,
			boost::bind(&ScriptFunctions::getResRef, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("EffectPetrify", 583,
			boost::bind(&ScriptFunctions::effectPetrify, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("CopyItem", 584,
			boost::bind(&ScriptFunctions::copyItem, this, _1),
			createSignature(4, kTypeObject, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, d.object0, d.int0));
	FunctionMan.registerFunction("EffectCutsceneParalyze", 585,
			boost::bind(&ScriptFunctions::effectCutsceneParalyze, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("GetDroppableFlag", 586,
			boost::bind(&ScriptFunctions::getDroppableFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetUseableFlag", 587,
			boost::bind(&ScriptFunctions::getUseableFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetStolenFlag", 588,
			boost::bind(&ScriptFunctions::getStolenFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetCampaignFloat", 589,
			boost::bind(&ScriptFunctions::setCampaignFloat, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeString, kTypeFloat, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCampaignInt", 590,
			boost::bind(&ScriptFunctions::setCampaignInt, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeString, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCampaignVector", 591,
			boost::bind(&ScriptFunctions::setCampaignVector, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeString, kTypeVector, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCampaignLocation", 592,
			boost::bind(&ScriptFunctions::setCampaignLocation, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeString, kTypeEngineType, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetCampaignString", 593,
			boost::bind(&ScriptFunctions::setCampaignString, this, _1),
			createSignature(5, kTypeVoid, kTypeString, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("DestroyCampaignDatabase", 594,
			boost::bind(&ScriptFunctions::destroyCampaignDatabase, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("GetCampaignFloat", 595,
			boost::bind(&ScriptFunctions::getCampaignFloat, this, _1),
			createSignature(4, kTypeFloat, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCampaignInt", 596,
			boost::bind(&ScriptFunctions::getCampaignInt, this, _1),
			createSignature(4, kTypeInt, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCampaignVector", 597,
			boost::bind(&ScriptFunctions::getCampaignVector, this, _1),
			createSignature(4, kTypeVector, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCampaignLocation", 598,
			boost::bind(&ScriptFunctions::getCampaignLocation, this, _1),
			createSignature(4, kTypeEngineType, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetCampaignString", 599,
			boost::bind(&ScriptFunctions::getCampaignString, this, _1),
			createSignature(4, kTypeString, kTypeString, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
}

void ScriptFunctions::getAnimalCompanionName(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetAnimalCompanionName");
}

void ScriptFunctions::actionCastFakeSpellAtObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionCastFakeSpellAtObject");
}

void ScriptFunctions::actionCastFakeSpellAtLocation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionCastFakeSpellAtLocation");
}

void ScriptFunctions::removeSummonedAssociate(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RemoveSummonedAssociate");
}

void ScriptFunctions::setCameraMode(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetCameraMode");
}

void ScriptFunctions::getIsResting(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsResting");
}

void ScriptFunctions::getLastPCRested(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastPCRested");
}

void ScriptFunctions::setWeather(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetWeather");
}

void ScriptFunctions::getLastRestEventType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastRestEventType");
}

void ScriptFunctions::startNewModule(Aurora::NWScript::FunctionContext &ctx) {
	Module *module = _engine->getModule();
	if (!module)
		return;

	Common::UString mod = ctx.getParams()[0].getString();

	if (!NWNEngine::hasModule(mod)) {
		warning("Can't start module \"%s\": No such module", mod.c_str());
		return;
	}

	module->load(mod);
}

void ScriptFunctions::effectSwarm(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSwarm");
}

void ScriptFunctions::getWeaponRanged(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetWeaponRanged");
}

void ScriptFunctions::doSinglePlayerAutoSave(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DoSinglePlayerAutoSave");
}

void ScriptFunctions::getGameDifficulty(Aurora::NWScript::FunctionContext &ctx) {
	// The scripts have another difficulty "Very Easy", which we don't recognize.
	ctx.getReturn() = (int32) ((GameDifficulty) (ConfigMan.getInt("difficulty") + 1));
}

void ScriptFunctions::setTileMainLightColor(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetTileMainLightColor");
}

void ScriptFunctions::setTileSourceLightColor(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetTileSourceLightColor");
}

void ScriptFunctions::recomputeStaticLighting(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RecomputeStaticLighting");
}

void ScriptFunctions::getTileMainLight1Color(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTileMainLight1Color");
}

void ScriptFunctions::getTileMainLight2Color(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTileMainLight2Color");
}

void ScriptFunctions::getTileSourceLight1Color(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTileSourceLight1Color");
}

void ScriptFunctions::getTileSourceLight2Color(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTileSourceLight2Color");
}

void ScriptFunctions::setPanelButtonFlash(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetPanelButtonFlash");
}

void ScriptFunctions::getCurrentAction(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kActionInvalid;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	warning("TODO: GetCurrentAction: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::setStandardFactionReputation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetStandardFactionReputation");
}

void ScriptFunctions::getStandardFactionReputation(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStandardFactionReputation");
}

void ScriptFunctions::floatingTextStrRefOnCreature(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FloatingTextStrRefOnCreature");
}

void ScriptFunctions::floatingTextStringOnCreature(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FloatingTextStringOnCreature");
}

void ScriptFunctions::getTrapDisarmable(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapDisarmable");
}

void ScriptFunctions::getTrapDetectable(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapDetectable");
}

void ScriptFunctions::getTrapDetectedBy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapDetectedBy");
}

void ScriptFunctions::getTrapFlagged(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapFlagged");
}

void ScriptFunctions::getTrapBaseType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapBaseType");
}

void ScriptFunctions::getTrapOneShot(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapOneShot");
}

void ScriptFunctions::getTrapCreator(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapCreator");
}

void ScriptFunctions::getTrapKeyTag(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapKeyTag");
}

void ScriptFunctions::getTrapDisarmDC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapDisarmDC");
}

void ScriptFunctions::getTrapDetectDC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTrapDetectDC");
}

void ScriptFunctions::getLockKeyRequired(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLockKeyRequired");
}

void ScriptFunctions::getLockKeyTag(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLockKeyTag");
}

void ScriptFunctions::getLockLockable(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLockLockable");
}

void ScriptFunctions::getLockUnlockDC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLockUnlockDC");
}

void ScriptFunctions::getLockLockDC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLockLockDC");
}

void ScriptFunctions::getPCLevellingUp(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetPCLevellingUp");
}

void ScriptFunctions::getHasFeatEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetHasFeatEffect");
}

void ScriptFunctions::setPlaceableIllumination(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetPlaceableIllumination");
}

void ScriptFunctions::getPlaceableIllumination(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetPlaceableIllumination");
}

void ScriptFunctions::getIsPlaceableObjectActionPossible(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsPlaceableObjectActionPossible");
}

void ScriptFunctions::doPlaceableObjectAction(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DoPlaceableObjectAction");
}

void ScriptFunctions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) getPC();
}

void ScriptFunctions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

void ScriptFunctions::setTrapDetectedBy(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetTrapDetectedBy");
}

void ScriptFunctions::getIsTrapped(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetIsTrapped");
}

void ScriptFunctions::effectTurnResistanceDecrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectTurnResistanceDecrease");
}

void ScriptFunctions::effectTurnResistanceIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectTurnResistanceIncrease");
}

void ScriptFunctions::popUpDeathGUIPanel(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: PopUpDeathGUIPanel");
}

void ScriptFunctions::setTrapDisabled(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetTrapDisabled");
}

void ScriptFunctions::getLastHostileActor(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastHostileActor");
}

void ScriptFunctions::exportAllCharacters(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ExportAllCharacters");
}

void ScriptFunctions::musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		ctx.getReturn() = (int32) area->getMusicDayTrack();
}

void ScriptFunctions::musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Area *area = convertArea(ctx.getParams()[0].getObject());
	if (area)
		ctx.getReturn() = (int32) area->getMusicNightTrack();
}

void ScriptFunctions::writeTimestampedLogEntry(Aurora::NWScript::FunctionContext &ctx) {
	ptime t(second_clock::universal_time());
	const Common::UString tstamp = Common::UString::format("%04d-%02d-%02dT%02d:%02d:%02d",
		(int) t.date().year(), (int) t.date().month(), (int) t.date().day(),
		(int) t.time_of_day().hours(), (int) t.time_of_day().minutes(),
		(int) t.time_of_day().seconds());

	status("NWN: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
}

void ScriptFunctions::getModuleName(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleName");
}

void ScriptFunctions::getFactionLeader(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFactionLeader");
}

void ScriptFunctions::sendMessageToAllDMs(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SendMessageToAllDMs");
}

void ScriptFunctions::endGame(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EndGame");
}

void ScriptFunctions::bootPC(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: BootPC");
}

void ScriptFunctions::actionCounterSpell(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionCounterSpell");
}

void ScriptFunctions::ambientSoundSetDayVolume(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AmbientSoundSetDayVolume");
}

void ScriptFunctions::ambientSoundSetNightVolume(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AmbientSoundSetNightVolume");
}

void ScriptFunctions::musicBackgroundGetBattleTrack(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: MusicBackgroundGetBattleTrack");
}

void ScriptFunctions::getHasInventory(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetHasInventory");
}

void ScriptFunctions::getStrRefSoundDuration(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStrRefSoundDuration");
}

void ScriptFunctions::addToParty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: AddToParty");
}

void ScriptFunctions::removeFromParty(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RemoveFromParty");
}

void ScriptFunctions::getStealthMode(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStealthMode");
}

void ScriptFunctions::getDetectMode(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDetectMode");
}

void ScriptFunctions::getDefensiveCastingMode(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDefensiveCastingMode");
}

void ScriptFunctions::getAppearanceType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetAppearanceType");
}

void ScriptFunctions::spawnScriptDebugger(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SpawnScriptDebugger");
}

void ScriptFunctions::getModuleItemAcquiredStackSize(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetModuleItemAcquiredStackSize");
}

void ScriptFunctions::decrementRemainingFeatUses(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DecrementRemainingFeatUses");
}

void ScriptFunctions::decrementRemainingSpellUses(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: DecrementRemainingSpellUses");
}

void ScriptFunctions::getResRef(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetResRef");
}

void ScriptFunctions::effectPetrify(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectPetrify");
}

void ScriptFunctions::copyItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: CopyItem");
}

void ScriptFunctions::effectCutsceneParalyze(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectCutsceneParalyze");
}

void ScriptFunctions::getDroppableFlag(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetDroppableFlag");
}

void ScriptFunctions::getUseableFlag(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetUseableFlag");
}

void ScriptFunctions::getStolenFlag(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStolenFlag");
}

void ScriptFunctions::setCampaignFloat(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	float value = ctx.getParams()[2].getFloat();

	warning("TODO: SetCampaignFloat: \"%s\":\"%s\" to %f",
			dbName.c_str(), varName.c_str(), value);
}

void ScriptFunctions::setCampaignInt(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	int32 value = ctx.getParams()[2].getInt();

	warning("TODO: SetCampaignInt: \"%s\":\"%s\" to %d",
			dbName.c_str(), varName.c_str(), value);
}

void ScriptFunctions::setCampaignVector(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	float x, y, z;
	ctx.getParams()[2].getVector(x, y, z);

	warning("TODO: SetCampaignVector: \"%s\":\"%s\" to %f, %f, %f",
			dbName.c_str(), varName.c_str(), x, y, z);
}

void ScriptFunctions::setCampaignLocation(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	// Location *loc = convertLocation(ctx.getParams()[2].getEngineType());

	warning("TODO: SetCampaignLocation: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::setCampaignString(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	const Common::UString &value = ctx.getParams()[2].getString();

	warning("TODO: SetCampaignString: \"%s\":\"%s\" to \"%s\"",
			dbName.c_str(), varName.c_str(), value.c_str());
}

void ScriptFunctions::destroyCampaignDatabase(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &dbName = ctx.getParams()[0].getString();

	warning("TODO: DestroyCampaignDatabase: \"%s\"", dbName.c_str());
}

void ScriptFunctions::getCampaignFloat(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0.0f;

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: GetCampaignFloat: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::getCampaignInt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: GetCampaignInt: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::getCampaignVector(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: GetCampaignVector: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::getCampaignLocation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::EngineType *) 0;

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: GetCampaignLocation: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

void ScriptFunctions::getCampaignString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &dbName  = ctx.getParams()[0].getString();
	const Common::UString &varName = ctx.getParams()[1].getString();

	warning("TODO: GetCampaignString: \"%s\":\"%s\"", dbName.c_str(), varName.c_str());
}

} // End of namespace NWN

} // End of namespace Engines
