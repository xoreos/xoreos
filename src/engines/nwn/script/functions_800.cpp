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

/** @file engines/nwn/script/functions_800.cpp
 *  NWN script functions, 800-847.
 */

#include <boost/bind.hpp>

#include "common/util.h"
#include "common/error.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

#include "engines/nwn/types.h"

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

void ScriptFunctions::registerFunctions800(const Defaults &d) {
	FunctionMan.registerFunction("SetLockLockable", 800,
			boost::bind(&ScriptFunctions::setLockLockable, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetLockUnlockDC", 801,
			boost::bind(&ScriptFunctions::setLockUnlockDC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetLockLockDC", 802,
			boost::bind(&ScriptFunctions::setLockLockDC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetTrapDisarmable", 803,
			boost::bind(&ScriptFunctions::setTrapDisarmable, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetTrapDetectable", 804,
			boost::bind(&ScriptFunctions::setTrapDetectable, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetTrapOneShot", 805,
			boost::bind(&ScriptFunctions::setTrapOneShot, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("SetTrapKeyTag", 806,
			boost::bind(&ScriptFunctions::setTrapKeyTag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("SetTrapDisarmDC", 807,
			boost::bind(&ScriptFunctions::setTrapDisarmDC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetTrapDetectDC", 808,
			boost::bind(&ScriptFunctions::setTrapDetectDC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("CreateTrapAtLocation", 809,
			boost::bind(&ScriptFunctions::createTrapAtLocation, this, _1),
			createSignature(8, kTypeObject, kTypeInt, kTypeEngineType, kTypeFloat, kTypeString, kTypeInt, kTypeString, kTypeString),
			createDefaults(5, d.float2_0, d.stringEmpty, d.intFactionHostile, d.stringEmpty, d.stringEmpty));
	FunctionMan.registerFunction("CreateTrapOnObject", 810,
			boost::bind(&ScriptFunctions::createTrapOnObject, this, _1),
			createSignature(6, kTypeVoid, kTypeInt, kTypeObject, kTypeInt, kTypeString, kTypeString),
			createDefaults(3, d.intFactionHostile, d.stringEmpty, d.stringEmpty));
	FunctionMan.registerFunction("SetWillSavingThrow", 811,
			boost::bind(&ScriptFunctions::setWillSavingThrow, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetReflexSavingThrow", 812,
			boost::bind(&ScriptFunctions::setReflexSavingThrow, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetFortitudeSavingThrow", 813,
			boost::bind(&ScriptFunctions::setFortitudeSavingThrow, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetTilesetResRef", 814,
			boost::bind(&ScriptFunctions::getTilesetResRef, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetTrapRecoverable", 815,
			boost::bind(&ScriptFunctions::getTrapRecoverable, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetTrapRecoverable", 816,
			boost::bind(&ScriptFunctions::setTrapRecoverable, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetModuleXPScale", 817,
			boost::bind(&ScriptFunctions::getModuleXPScale, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("SetModuleXPScale", 818,
			boost::bind(&ScriptFunctions::setModuleXPScale, this, _1),
			createSignature(2, kTypeVoid, kTypeInt));
	FunctionMan.registerFunction("GetKeyRequiredFeedback", 819,
			boost::bind(&ScriptFunctions::getKeyRequiredFeedback, this, _1),
			createSignature(2, kTypeString, kTypeObject));
	FunctionMan.registerFunction("SetKeyRequiredFeedback", 820,
			boost::bind(&ScriptFunctions::setKeyRequiredFeedback, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetTrapActive", 821,
			boost::bind(&ScriptFunctions::getTrapActive, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetTrapActive", 822,
			boost::bind(&ScriptFunctions::setTrapActive, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("LockCameraPitch", 823,
			boost::bind(&ScriptFunctions::lockCameraPitch, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("LockCameraDistance", 824,
			boost::bind(&ScriptFunctions::lockCameraDistance, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("LockCameraDirection", 825,
			boost::bind(&ScriptFunctions::lockCameraDirection, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetPlaceableLastClickedBy", 826,
			boost::bind(&ScriptFunctions::getPlaceableLastClickedBy, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetInfiniteFlag", 827,
			boost::bind(&ScriptFunctions::getInfiniteFlag, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("SetInfiniteFlag", 828,
			boost::bind(&ScriptFunctions::setInfiniteFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("GetAreaSize", 829,
			boost::bind(&ScriptFunctions::getAreaSize, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetName", 830,
			boost::bind(&ScriptFunctions::setName, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString),
			createDefaults(1, d.stringEmpty));
	FunctionMan.registerFunction("GetPortraitId", 831,
			boost::bind(&ScriptFunctions::getPortraitId, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetPortraitId", 832,
			boost::bind(&ScriptFunctions::setPortraitId, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetPortraitResRef", 833,
			boost::bind(&ScriptFunctions::getPortraitResRef, this, _1),
			createSignature(2, kTypeString, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("SetPortraitResRef", 834,
			boost::bind(&ScriptFunctions::setPortraitResRef, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("SetUseableFlag", 835,
			boost::bind(&ScriptFunctions::setUseableFlag, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetDescription", 836,
			boost::bind(&ScriptFunctions::getDescription, this, _1),
			createSignature(4, kTypeString, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(2, d.int0, d.int1));
	FunctionMan.registerFunction("SetDescription", 837,
			boost::bind(&ScriptFunctions::setDescription, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeInt),
			createDefaults(2, d.stringEmpty, d.int1));
	FunctionMan.registerFunction("GetPCChatSpeaker", 838,
			boost::bind(&ScriptFunctions::getPCChatSpeaker, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetPCChatMessage", 839,
			boost::bind(&ScriptFunctions::getPCChatMessage, this, _1),
			createSignature(1, kTypeString));
	FunctionMan.registerFunction("GetPCChatVolume", 840,
			boost::bind(&ScriptFunctions::getPCChatVolume, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("SetPCChatMessage", 841,
			boost::bind(&ScriptFunctions::setPCChatMessage, this, _1),
			createSignature(2, kTypeVoid, kTypeString),
			createDefaults(1, d.stringEmpty));
	FunctionMan.registerFunction("SetPCChatVolume", 842,
			boost::bind(&ScriptFunctions::setPCChatVolume, this, _1),
			createSignature(2, kTypeVoid, kTypeInt),
			createDefaults(1, d.intTalk));
	FunctionMan.registerFunction("GetColor", 843,
			boost::bind(&ScriptFunctions::getColor, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("SetColor", 844,
			boost::bind(&ScriptFunctions::setColor, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyMaterial", 845,
			boost::bind(&ScriptFunctions::itemPropertyMaterial, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyQuality", 846,
			boost::bind(&ScriptFunctions::itemPropertyQuality, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("ItemPropertyAdditional", 847,
			boost::bind(&ScriptFunctions::itemPropertyAdditional, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
}

void ScriptFunctions::setLockLockable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLockLockable");
}

void ScriptFunctions::setLockUnlockDC(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLockUnlockDC");
}

void ScriptFunctions::setLockLockDC(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetLockLockDC");
}

void ScriptFunctions::setTrapDisarmable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapDisarmable");
}

void ScriptFunctions::setTrapDetectable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapDetectable");
}

void ScriptFunctions::setTrapOneShot(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapOneShot");
}

void ScriptFunctions::setTrapKeyTag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapKeyTag");
}

void ScriptFunctions::setTrapDisarmDC(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapDisarmDC");
}

void ScriptFunctions::setTrapDetectDC(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapDetectDC");
}

void ScriptFunctions::createTrapAtLocation(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: CreateTrapAtLocation");
}

void ScriptFunctions::createTrapOnObject(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: CreateTrapOnObject");
}

void ScriptFunctions::setWillSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetWillSavingThrow");
}

void ScriptFunctions::setReflexSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetReflexSavingThrow");
}

void ScriptFunctions::setFortitudeSavingThrow(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetFortitudeSavingThrow");
}

void ScriptFunctions::getTilesetResRef(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTilesetResRef");
}

void ScriptFunctions::getTrapRecoverable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTrapRecoverable");
}

void ScriptFunctions::setTrapRecoverable(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapRecoverable");
}

void ScriptFunctions::getModuleXPScale(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetModuleXPScale");
}

void ScriptFunctions::setModuleXPScale(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetModuleXPScale");
}

void ScriptFunctions::getKeyRequiredFeedback(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetKeyRequiredFeedback");
}

void ScriptFunctions::setKeyRequiredFeedback(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetKeyRequiredFeedback");
}

void ScriptFunctions::getTrapActive(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetTrapActive");
}

void ScriptFunctions::setTrapActive(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetTrapActive");
}

void ScriptFunctions::lockCameraPitch(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LockCameraPitch");
}

void ScriptFunctions::lockCameraDistance(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LockCameraDistance");
}

void ScriptFunctions::lockCameraDirection(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: LockCameraDirection");
}

void ScriptFunctions::getPlaceableLastClickedBy(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPlaceableLastClickedBy");
}

void ScriptFunctions::getInfiniteFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetInfiniteFlag");
}

void ScriptFunctions::setInfiniteFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetInfiniteFlag");
}

void ScriptFunctions::getAreaSize(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetAreaSize");
}

void ScriptFunctions::setName(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetName");
}

void ScriptFunctions::getPortraitId(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPortraitId");
}

void ScriptFunctions::setPortraitId(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPortraitId");
}

void ScriptFunctions::getPortraitResRef(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPortraitResRef");
}

void ScriptFunctions::setPortraitResRef(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPortraitResRef");
}

void ScriptFunctions::setUseableFlag(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetUseableFlag");
}

void ScriptFunctions::getDescription(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetDescription");
}

void ScriptFunctions::setDescription(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetDescription");
}

void ScriptFunctions::getPCChatSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCChatSpeaker");
}

void ScriptFunctions::getPCChatMessage(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCChatMessage");
}

void ScriptFunctions::getPCChatVolume(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetPCChatVolume");
}

void ScriptFunctions::setPCChatMessage(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPCChatMessage");
}

void ScriptFunctions::setPCChatVolume(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetPCChatVolume");
}

void ScriptFunctions::getColor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetColor");
}

void ScriptFunctions::setColor(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: SetColor");
}

void ScriptFunctions::itemPropertyMaterial(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyMaterial");
}

void ScriptFunctions::itemPropertyQuality(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyQuality");
}

void ScriptFunctions::itemPropertyAdditional(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ItemPropertyAdditional");
}

} // End of namespace NWN

} // End of namespace Engines
