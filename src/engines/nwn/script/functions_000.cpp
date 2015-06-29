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
 *  NWN script functions, 000-099.
 */

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"

#include "src/aurora/resman.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/ncsfile.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/door.h"
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

void ScriptFunctions::registerFunctions000(const Defaults &d) {
	FunctionMan.registerFunction("Random", 0,
			boost::bind(&ScriptFunctions::random, this, _1),
			createSignature(2, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("PrintString", 1,
			boost::bind(&ScriptFunctions::printString, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("PrintFloat", 2,
			boost::bind(&ScriptFunctions::printFloat, this, _1),
			createSignature(4, kTypeVoid, kTypeFloat, kTypeInt, kTypeInt),
			createDefaults(2, d.int18, d.int9));
	FunctionMan.registerFunction("FloatToString", 3,
			boost::bind(&ScriptFunctions::floatToString, this, _1),
			createSignature(4, kTypeString, kTypeFloat, kTypeInt, kTypeInt),
			createDefaults(2, d.int18, d.int9));
	FunctionMan.registerFunction("PrintInteger", 4,
			boost::bind(&ScriptFunctions::printInteger, this, _1),
			createSignature(2, kTypeVoid, kTypeInt));
	FunctionMan.registerFunction("PrintObject", 5,
			boost::bind(&ScriptFunctions::printObject, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("AssignCommand", 6,
			boost::bind(&ScriptFunctions::assignCommand, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeScriptState));
	FunctionMan.registerFunction("DelayCommand", 7,
			boost::bind(&ScriptFunctions::delayCommand, this, _1),
			createSignature(3, kTypeVoid, kTypeFloat, kTypeScriptState));
	FunctionMan.registerFunction("ExecuteScript", 8,
			boost::bind(&ScriptFunctions::executeScript, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject));
	FunctionMan.registerFunction("ClearAllActions", 9,
			boost::bind(&ScriptFunctions::clearAllActions, this, _1),
			createSignature(2, kTypeVoid, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("SetFacing", 10,
			boost::bind(&ScriptFunctions::setFacing, this, _1),
			createSignature(2, kTypeVoid, kTypeFloat));
	FunctionMan.registerFunction("SetCalendar", 11,
			boost::bind(&ScriptFunctions::setCalendar, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("SetTime", 12,
			boost::bind(&ScriptFunctions::setTime, this, _1),
			createSignature(5, kTypeVoid, kTypeInt, kTypeInt, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("GetCalendarYear", 13,
			boost::bind(&ScriptFunctions::getCalendarYear, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetCalendarMonth", 14,
			boost::bind(&ScriptFunctions::getCalendarMonth, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetCalendarDay", 15,
			boost::bind(&ScriptFunctions::getCalendarDay, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetTimeHour", 16,
			boost::bind(&ScriptFunctions::getTimeHour, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetTimeMinute", 17,
			boost::bind(&ScriptFunctions::getTimeMinute, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetTimeSecond", 18,
			boost::bind(&ScriptFunctions::getTimeSecond, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("GetTimeMillisecond", 19,
			boost::bind(&ScriptFunctions::getTimeMillisecond, this, _1),
			createSignature(1, kTypeInt));
	FunctionMan.registerFunction("ActionRandomWalk", 20,
			boost::bind(&ScriptFunctions::actionRandomWalk, this, _1),
			createSignature(1, kTypeVoid));
	FunctionMan.registerFunction("ActionMoveToLocation", 21,
			boost::bind(&ScriptFunctions::actionMoveToLocation, this, _1),
			createSignature(3, kTypeVoid, kTypeEngineType, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("ActionMoveToObject", 22,
			boost::bind(&ScriptFunctions::actionMoveToObject, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(2, d.int0, d.float1_0));
	FunctionMan.registerFunction("ActionMoveAwayFromObject", 23,
			boost::bind(&ScriptFunctions::actionMoveAwayFromObject, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(2, d.int0, d.float40_0));
	FunctionMan.registerFunction("GetArea", 24,
			boost::bind(&ScriptFunctions::getArea, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetEnteringObject", 25,
			boost::bind(&ScriptFunctions::getEnteringObject, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetExitingObject", 26,
			boost::bind(&ScriptFunctions::getExitingObject, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetPosition", 27,
			boost::bind(&ScriptFunctions::getPosition, this, _1),
			createSignature(2, kTypeVector, kTypeObject));
	FunctionMan.registerFunction("GetFacing", 28,
			boost::bind(&ScriptFunctions::getFacing, this, _1),
			createSignature(2, kTypeFloat, kTypeObject));
	FunctionMan.registerFunction("GetItemPossessor", 29,
			boost::bind(&ScriptFunctions::getItemPossessor, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetItemPossessedBy", 30,
			boost::bind(&ScriptFunctions::getItemPossessedBy, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeString));
	FunctionMan.registerFunction("CreateItemOnObject", 31,
			boost::bind(&ScriptFunctions::createItemOnObject, this, _1),
			createSignature(5, kTypeObject, kTypeString, kTypeObject, kTypeInt, kTypeString),
			createDefaults(3, d.object0, d.int1, d.stringEmpty));
	FunctionMan.registerFunction("ActionEquipItem", 32,
			boost::bind(&ScriptFunctions::actionEquipItem, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("ActionUnequipItem", 33,
			boost::bind(&ScriptFunctions::actionUnequipItem, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ActionPickUpItem", 34,
			boost::bind(&ScriptFunctions::actionPickUpItem, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ActionPutDownItem", 35,
			boost::bind(&ScriptFunctions::actionPutDownItem, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("GetLastAttacker", 36,
			boost::bind(&ScriptFunctions::getLastAttacker, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("ActionAttack", 37,
			boost::bind(&ScriptFunctions::actionAttack, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("GetNearestCreature", 38,
			boost::bind(&ScriptFunctions::getNearestCreature, this, _1),
			createSignature(9, kTypeObject, kTypeInt, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(6, d.object0, d.int1, d.intm1, d.intm1, d.intm1, d.intm1));
	FunctionMan.registerFunction("ActionSpeakString", 39,
			boost::bind(&ScriptFunctions::actionSpeakString, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeInt),
			createDefaults(1, d.intTalk));
	FunctionMan.registerFunction("ActionPlayAnimation", 40,
			boost::bind(&ScriptFunctions::actionPlayAnimation, this, _1),
			createSignature(4, kTypeVoid, kTypeInt, kTypeFloat, kTypeFloat),
			createDefaults(2, d.float1_0, d.float0_0));
	FunctionMan.registerFunction("GetDistanceToObject", 41,
			boost::bind(&ScriptFunctions::getDistanceToObject, this, _1),
			createSignature(2, kTypeFloat, kTypeObject));
	FunctionMan.registerFunction("GetIsObjectValid", 42,
			boost::bind(&ScriptFunctions::getIsObjectValid, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("ActionOpenDoor", 43,
			boost::bind(&ScriptFunctions::actionOpenDoor, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("ActionCloseDoor", 44,
			boost::bind(&ScriptFunctions::actionCloseDoor, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("SetCameraFacing", 45,
			boost::bind(&ScriptFunctions::setCameraFacing, this, _1),
			createSignature(5, kTypeVoid, kTypeFloat, kTypeFloat, kTypeFloat, kTypeInt),
			createDefaults(3, d.floatm1_0, d.floatm1_0, d.intCameraSnap));
	FunctionMan.registerFunction("PlaySound", 46,
			boost::bind(&ScriptFunctions::playSound, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("GetSpellTargetObject", 47,
			boost::bind(&ScriptFunctions::getSpellTargetObject, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("ActionCastSpellAtObject", 48,
			boost::bind(&ScriptFunctions::actionCastSpellAtObject, this, _1),
			createSignature(8, kTypeVoid, kTypeInt, kTypeObject, kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(5, d.intMetaMagicAny, d.int0, d.int0, d.intProjPathDefault, d.int0));
	FunctionMan.registerFunction("GetCurrentHitPoints", 49,
			boost::bind(&ScriptFunctions::getCurrentHitPoints, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetMaxHitPoints", 50,
			boost::bind(&ScriptFunctions::getMaxHitPoints, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetLocalInt", 51,
			boost::bind(&ScriptFunctions::getLocalInt, this, _1),
			createSignature(3, kTypeInt, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetLocalFloat", 52,
			boost::bind(&ScriptFunctions::getLocalFloat, this, _1),
			createSignature(3, kTypeFloat, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetLocalString", 53,
			boost::bind(&ScriptFunctions::getLocalString, this, _1),
			createSignature(3, kTypeString, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetLocalObject", 54,
			boost::bind(&ScriptFunctions::getLocalObject, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeString));
	FunctionMan.registerFunction("SetLocalInt", 55,
			boost::bind(&ScriptFunctions::setLocalInt, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeInt));
	FunctionMan.registerFunction("SetLocalFloat", 56,
			boost::bind(&ScriptFunctions::setLocalFloat, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeFloat));
	FunctionMan.registerFunction("SetLocalString", 57,
			boost::bind(&ScriptFunctions::setLocalString, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeString));
	FunctionMan.registerFunction("SetLocalObject", 58,
			boost::bind(&ScriptFunctions::setLocalObject, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeString, kTypeObject));
	FunctionMan.registerFunction("GetStringLength", 59,
			boost::bind(&ScriptFunctions::getStringLength, this, _1),
			createSignature(2, kTypeInt, kTypeString));
	FunctionMan.registerFunction("GetStringUpperCase", 60,
			boost::bind(&ScriptFunctions::getStringUpperCase, this, _1),
			createSignature(2, kTypeString, kTypeString));
	FunctionMan.registerFunction("GetStringLowerCase", 61,
			boost::bind(&ScriptFunctions::getStringLowerCase, this, _1),
			createSignature(2, kTypeString, kTypeString));
	FunctionMan.registerFunction("GetStringRight", 62,
			boost::bind(&ScriptFunctions::getStringRight, this, _1),
			createSignature(3, kTypeString, kTypeString, kTypeInt));
	FunctionMan.registerFunction("GetStringLeft", 63,
			boost::bind(&ScriptFunctions::getStringLeft, this, _1),
			createSignature(3, kTypeString, kTypeString, kTypeInt));
	FunctionMan.registerFunction("InsertString", 64,
			boost::bind(&ScriptFunctions::insertString, this, _1),
			createSignature(4, kTypeString, kTypeString, kTypeString, kTypeInt));
	FunctionMan.registerFunction("GetSubString", 65,
			boost::bind(&ScriptFunctions::getSubString, this, _1),
			createSignature(4, kTypeString, kTypeString, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("FindSubString", 66,
			boost::bind(&ScriptFunctions::findSubString, this, _1),
			createSignature(4, kTypeInt, kTypeString, kTypeString, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("fabs", 67,
			boost::bind(&ScriptFunctions::fabs, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("cos", 68,
			boost::bind(&ScriptFunctions::cos, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("sin", 69,
			boost::bind(&ScriptFunctions::sin, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("tan", 70,
			boost::bind(&ScriptFunctions::tan, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("acos", 71,
			boost::bind(&ScriptFunctions::acos, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("asin", 72,
			boost::bind(&ScriptFunctions::asin, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("atan", 73,
			boost::bind(&ScriptFunctions::atan, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("log", 74,
			boost::bind(&ScriptFunctions::log, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("pow", 75,
			boost::bind(&ScriptFunctions::pow, this, _1),
			createSignature(3, kTypeFloat, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("sqrt", 76,
			boost::bind(&ScriptFunctions::sqrt, this, _1),
			createSignature(2, kTypeFloat, kTypeFloat));
	FunctionMan.registerFunction("abs", 77,
			boost::bind(&ScriptFunctions::abs, this, _1),
			createSignature(2, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectHeal", 78,
			boost::bind(&ScriptFunctions::effectHeal, this, _1),
			createSignature(2, kTypeEngineType, kTypeInt));
	FunctionMan.registerFunction("EffectDamage", 79,
			boost::bind(&ScriptFunctions::effectDamage, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(2, d.intDamageMagical, d.intDamageNormal));
	FunctionMan.registerFunction("EffectAbilityIncrease", 80,
			boost::bind(&ScriptFunctions::effectAbilityIncrease, this, _1),
			createSignature(3, kTypeEngineType, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("EffectDamageResistance", 81,
			boost::bind(&ScriptFunctions::effectDamageResistance, this, _1),
			createSignature(4, kTypeEngineType, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, d.int0));
	FunctionMan.registerFunction("EffectResurrection", 82,
			boost::bind(&ScriptFunctions::effectResurrection, this, _1),
			createSignature(1, kTypeEngineType));
	FunctionMan.registerFunction("EffectSummonCreature", 83,
			boost::bind(&ScriptFunctions::effectSummonCreature, this, _1),
			createSignature(5, kTypeEngineType, kTypeString, kTypeInt, kTypeFloat, kTypeInt),
			createDefaults(3, d.intVFXNone, d.float0_0, d.int0));
	FunctionMan.registerFunction("GetCasterLevel", 84,
			boost::bind(&ScriptFunctions::getCasterLevel, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("GetFirstEffect", 85,
			boost::bind(&ScriptFunctions::getFirstEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("GetNextEffect", 86,
			boost::bind(&ScriptFunctions::getNextEffect, this, _1),
			createSignature(2, kTypeEngineType, kTypeObject));
	FunctionMan.registerFunction("RemoveEffect", 87,
			boost::bind(&ScriptFunctions::removeEffect, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeEngineType));
	FunctionMan.registerFunction("GetIsEffectValid", 88,
			boost::bind(&ScriptFunctions::getIsEffectValid, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetEffectDurationType", 89,
			boost::bind(&ScriptFunctions::getEffectDurationType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetEffectSubType", 90,
			boost::bind(&ScriptFunctions::getEffectSubType, this, _1),
			createSignature(2, kTypeInt, kTypeEngineType));
	FunctionMan.registerFunction("GetEffectCreator", 91,
			boost::bind(&ScriptFunctions::getEffectCreator, this, _1),
			createSignature(2, kTypeObject, kTypeEngineType));
	FunctionMan.registerFunction("IntToString", 92,
			boost::bind(&ScriptFunctions::intToString, this, _1),
			createSignature(2, kTypeString, kTypeInt));
	FunctionMan.registerFunction("GetFirstObjectInArea", 93,
			boost::bind(&ScriptFunctions::getFirstObjectInArea, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("GetNextObjectInArea", 94,
			boost::bind(&ScriptFunctions::getNextObjectInArea, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, d.object0));
	FunctionMan.registerFunction("d2", 95,
			boost::bind(&ScriptFunctions::d2, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d3", 96,
			boost::bind(&ScriptFunctions::d3, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d4", 97,
			boost::bind(&ScriptFunctions::d4, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d6", 98,
			boost::bind(&ScriptFunctions::d6, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
	FunctionMan.registerFunction("d8", 99,
			boost::bind(&ScriptFunctions::d8, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, d.int1));
}

void ScriptFunctions::random(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(0, ctx.getParams()[0].getInt() - 1);
}

void ScriptFunctions::printString(Aurora::NWScript::FunctionContext &ctx) {
	status("NWN: %s", ctx.getParams()[0].getString().c_str());
}

void ScriptFunctions::printFloat(Aurora::NWScript::FunctionContext &ctx) {
	float value    = ctx.getParams()[0].getFloat();
	int   width    = ctx.getParams()[1].getInt();
	int   decimals = ctx.getParams()[2].getInt();

	status("NWN: %s", floatToString(value, width, decimals).c_str());
}

void ScriptFunctions::floatToString(Aurora::NWScript::FunctionContext &ctx) {
	float value    = ctx.getParams()[0].getFloat();
	int   width    = ctx.getParams()[1].getInt();
	int   decimals = ctx.getParams()[2].getInt();

	ctx.getReturn() = floatToString(value, width, decimals);
}

void ScriptFunctions::printInteger(Aurora::NWScript::FunctionContext &ctx) {
	status("NWN: %d", ctx.getParams()[0].getInt());
}

void ScriptFunctions::printObject(Aurora::NWScript::FunctionContext &ctx) {
	status("NWN: %p", (void *) ctx.getParams()[0].getObject());
}

void ScriptFunctions::assignCommand(Aurora::NWScript::FunctionContext &ctx) {
	Module *module = _engine->getModule();
	if (!module)
		return;

	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("ScriptFunctions::assignCommand(): Script needed");

	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		object = ctx.getCaller();

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	module->delayScript(script, state, object, ctx.getTriggerer(), 0);
}

void ScriptFunctions::delayCommand(Aurora::NWScript::FunctionContext &ctx) {
	Module *module = _engine->getModule();
	if (!module)
		return;

	Common::UString script = ctx.getScriptName();
	if (script.empty())
		throw Common::Exception("ScriptFunctions::assignCommand(): Script needed");

	uint32 delay = ctx.getParams()[0].getFloat() * 1000;

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	module->delayScript(script, state, ctx.getCaller(), ctx.getTriggerer(), delay);
}

void ScriptFunctions::executeScript(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getParams()[0].getString();

	// Max resource name length is 16, and ExecuteScript should truncate accordingly
	script.truncate(16);

	if (!ResMan.hasResource(script, Aurora::kFileTypeNCS))
		return;

	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();
	try {
		Aurora::NWScript::NCSFile ncs(script);

		ncs.run(object);
	} catch (Common::Exception &e) {
		e.add("Failed ExecuteScript(\"%s\", %s)", script.c_str(), gTag(object).c_str());

		Common::printException(e, "WARNING: ");
	}
}

void ScriptFunctions::clearAllActions(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ClearAllActions");
}

void ScriptFunctions::setFacing(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetFacing");
}

void ScriptFunctions::setCalendar(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetCalendar");
}

void ScriptFunctions::setTime(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetTime");
}

void ScriptFunctions::getCalendarYear(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetCalendarYear");
}

void ScriptFunctions::getCalendarMonth(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetCalendarMonth");
}

void ScriptFunctions::getCalendarDay(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetCalendarDay");
}

void ScriptFunctions::getTimeHour(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTimeHour");
}

void ScriptFunctions::getTimeMinute(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTimeMinute");
}

void ScriptFunctions::getTimeSecond(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTimeSecond");
}

void ScriptFunctions::getTimeMillisecond(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetTimeMillisecond");
}

void ScriptFunctions::actionRandomWalk(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionRandomWalk");
}

void ScriptFunctions::actionMoveToLocation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionMoveToLocation(): /Action/

	Object   *object = convertObject(ctx.getCaller());
	Location *moveTo = convertLocation(ctx.getParams()[0].getEngineType());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);

	bool run = ctx.getParams()[1].getInt() != 0;

	warning("TODO: ActionMoveToLocation: \"%s\" (%d)", object->getTag().c_str(), run);
}

void ScriptFunctions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionMoveToObject(): /Action/

	Object *object = convertObject(ctx.getCaller());
	Object *moveTo = convertObject(ctx.getParams()[0].getObject());

	if (!object || !moveTo)
		return;

	float x, y, z;
	moveTo->getPosition(x, y, z);

	jumpTo(object, moveTo->getArea(), x, y, z);

	bool  run   = ctx.getParams()[1].getInt() != 0;
	float range = ctx.getParams()[2].getFloat();

	warning("TODO: ActionMoveToObject: \"%s\" to \"%s\" (%d, %f)",
			object->getTag().c_str(), moveTo->getTag().c_str(), run, range);
}

void ScriptFunctions::actionMoveAwayFromObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionMoveAwayFromObject");
}

void ScriptFunctions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (object)
		ctx.getReturn() = (Aurora::NWScript::Object *) object->getArea();
}

void ScriptFunctions::getEnteringObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void ScriptFunctions::getExitingObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void ScriptFunctions::getPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	float x, y, z;
	object->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

void ScriptFunctions::getFacing(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0.0f;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetFacing: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getItemPossessor(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetItemPossessor: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getItemPossessedBy(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (!object)
		return;

	const Common::UString &tag = params[1].getString();

	warning("TODO: GetItemPossessedBy: \"%s\" by \"%s\"",
	        tag.c_str(), object->getTag().c_str());
}

void ScriptFunctions::createItemOnObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *target = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		target = convertObject(ctx.getCaller());
	if (!target)
		return;

	const Common::UString &templ  = ctx.getParams()[0].getString();
	const Common::UString &newTag = ctx.getParams()[3].getString();

	int stackSize = ctx.getParams()[2].getInt();

	warning("TODO: CreateItemOnObject: %dx \"%s\" on \"%s\" (\"%s\")", stackSize,
	        templ.c_str(), target->getTag().c_str(), newTag.c_str());
}

void ScriptFunctions::actionEquipItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionEquipItem");
}

void ScriptFunctions::actionUnequipItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionUnequipItem");
}

void ScriptFunctions::actionPickUpItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionPickUpItem");
}

void ScriptFunctions::actionPutDownItem(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionPutDownItem");
}

void ScriptFunctions::getLastAttacker(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetLastAttacker");
}

void ScriptFunctions::actionAttack(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionAttack");
}

void ScriptFunctions::getNearestCreature(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Module *module = _engine->getModule();
	if (!module)
		return;

	Object *target = convertObject(ctx.getParams()[2].getObject());
	if (ctx.getParamsSpecified() < 3)
		target = convertObject(ctx.getCaller());

	if (!target)
		return;

	int nth = ctx.getParams()[3].getInt() - 1;

	// TODO: ScriptFunctions::getNearestCreature(): Critia
	/*
	int crit1Type  = ctx.getParams()[0].getInt();
	int crit1Value = ctx.getParams()[1].getInt();
	int crit2Type  = ctx.getParams()[4].getInt();
	int crit2Value = ctx.getParams()[5].getInt();
	int crit3Type  = ctx.getParams()[6].getInt();
	int crit3Value = ctx.getParams()[7].getInt();
	*/

	Aurora::NWScript::ObjectSearch *search = module->findObjects();
	Aurora::NWScript::Object       *object = 0;

	std::list<Object *> creatures;
	while ((object = search->next())) {
		Creature *creature = convertCreature(object);

		if (creature && (creature != target) && (creature->getArea() == target->getArea()))
			creatures.push_back(creature);
	}

	delete search;

	creatures.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = creatures.begin();
	for (int n = 0; (n < nth) && (it != creatures.end()); ++n)
		++it;

	if (it != creatures.end())
		ctx.getReturn() = *it;
}

void ScriptFunctions::actionSpeakString(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionSpeakString(): /Action/

	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	const Common::UString &str = ctx.getParams()[0].getString();

	// TODO: ScriptFunctions::speakString(): Volume
	uint32 volume = (uint32) ctx.getParams()[1].getInt();

	object->speakString(str, volume);
}

void ScriptFunctions::actionPlayAnimation(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionPlayAnimation(): /Action/

	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	Animation animation = (Animation) ctx.getParams()[0].getInt();

	// TODO: ScriptFunctions::actionPlayAnimation(): speed, second
	// float speed   = ctx.getParams()[1].getFloat();
	// float seconds = ctx.getParams()[2].getFloat();

	object->playAnimation(animation);
}

#define SQR(x) ((x) * (x))
void ScriptFunctions::getDistanceToObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1.0f;

	Object *object1 = convertObject(ctx.getParams()[0].getObject());
	Object *object2 = convertObject(ctx.getCaller());
	if (!object1 || !object2)
		return;

	float x1, y1, z1;
	object1->getPosition(x1, y1, z1);

	float x2, y2, z2;
	object2->getPosition(x2, y2, z2);

	ctx.getReturn() = sqrtf(SQR(x1 - x2) + SQR(y1 - y2) + SQR(z1 - z2));
}
#undef SQR

void ScriptFunctions::getIsObjectValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = convertObject(ctx.getParams()[0].getObject()) != 0;
}

void ScriptFunctions::actionOpenDoor(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionOpenDoor(): /Action/

	Door *door = convertDoor(ctx.getParams()[0].getObject());
	if (!door)
		return;

	door->open(convertObject(ctx.getCaller()));
}

void ScriptFunctions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::actionCloseDoor(): /Action/

	Door *door = convertDoor(ctx.getParams()[0].getObject());
	if (!door)
		return;

	door->close(convertObject(ctx.getCaller()));
}

void ScriptFunctions::setCameraFacing(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: SetCameraFacing");
}

void ScriptFunctions::playSound(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	if (!object)
		return;

	object->playSound(ctx.getParams()[0].getString());
}

void ScriptFunctions::getSpellTargetObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSpellTargetObject");
}

void ScriptFunctions::actionCastSpellAtObject(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: ActionCastSpellAtObject");
}

void ScriptFunctions::getCurrentHitPoints(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetCurrentHitPoints");
}

void ScriptFunctions::getMaxHitPoints(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetMaxHitPoints");
}

void ScriptFunctions::getLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeInt).getInt();
}

void ScriptFunctions::getLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeFloat).getFloat();
}

void ScriptFunctions::getLocalString(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeString).getString();
}

void ScriptFunctions::getLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeObject).getObject();
}

void ScriptFunctions::setLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		object->setVariable(params[1].getString(), params[2].getInt());
}

void ScriptFunctions::setLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		object->setVariable(params[1].getString(), params[2].getFloat());
}

void ScriptFunctions::setLocalString(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		object->setVariable(params[1].getString(), params[2].getString());
}

void ScriptFunctions::setLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Aurora::NWScript::Object *object = params[0].getObject();
	if (object)
		object->setVariable(params[1].getString(), params[2].getObject());
}

void ScriptFunctions::getStringLength(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) ctx.getParams()[0].getString().size();
}

void ScriptFunctions::getStringUpperCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toUpper();
}

void ScriptFunctions::getStringLowerCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toLower();
}

void ScriptFunctions::getStringRight(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStringRight");
}

void ScriptFunctions::getStringLeft(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetStringLeft");
}

void ScriptFunctions::insertString(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: InsertString");
}

void ScriptFunctions::getSubString(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetSubString");
}

void ScriptFunctions::findSubString(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: FindSubString");
}

void ScriptFunctions::fabs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::cos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = cosf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::sin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = sinf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::tan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = tanf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::acos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = acosf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::asin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = asinf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::atan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = atanf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::log(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = logf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::pow(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = powf(ctx.getParams()[0].getFloat(), ctx.getParams()[1].getFloat());
}

void ScriptFunctions::sqrt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = sqrtf(ctx.getParams()[0].getFloat());
}

void ScriptFunctions::abs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getInt());
}

void ScriptFunctions::effectHeal(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectHeal");
}

void ScriptFunctions::effectDamage(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDamage");
}

void ScriptFunctions::effectAbilityIncrease(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectAbilityIncrease");
}

void ScriptFunctions::effectDamageResistance(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectDamageResistance");
}

void ScriptFunctions::effectResurrection(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectResurrection");
}

void ScriptFunctions::effectSummonCreature(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: EffectSummonCreature");
}

void ScriptFunctions::getCasterLevel(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetCasterLevel");
}

void ScriptFunctions::getFirstEffect(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::EngineType *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetFirstEffect: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getNextEffect(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::EngineType *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetNextEffect: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::removeEffect(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: RemoveEffect");
}

void ScriptFunctions::getIsEffectValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getEngineType() != 0;
}

void ScriptFunctions::getEffectDurationType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEffectDurationType");
}

void ScriptFunctions::getEffectSubType(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEffectSubType");
}

void ScriptFunctions::getEffectCreator(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetEffectCreator");
}

void ScriptFunctions::intToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::format("%d", ctx.getParams()[0].getInt());
}

void ScriptFunctions::getFirstObjectInArea(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetFirstObjectInArea");
}

void ScriptFunctions::getNextObjectInArea(Aurora::NWScript::FunctionContext &UNUSED(ctx)) {
	warning("TODO: GetNextObjectInArea");
}

void ScriptFunctions::d2(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 2, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d3(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 3, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d4(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 4, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d6(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 6, ctx.getParams()[0].getInt());
}

void ScriptFunctions::d8(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = random(1, 8, ctx.getParams()[0].getInt());
}

} // End of namespace NWN

} // End of namespace Engines
