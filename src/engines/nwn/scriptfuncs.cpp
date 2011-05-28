/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file engines/nwn/scriptfuncs.cpp
 *  NWN script functions.
 */

#include "boost/bind.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/configman.h"

#include "aurora/talkman.h"
#include "aurora/ssffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"
#include "aurora/nwscript/ncsfile.h"

#include "engines/aurora/tokenman.h"

#include "engines/nwn/scriptfuncs.h"
#include "engines/nwn/object.h"
#include "engines/nwn/creature.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

using Aurora::kObjectIDInvalid;

// NWScript
using Aurora::NWScript::kTypeVoid;
using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeVector;
using Aurora::NWScript::kTypeScriptState;
using Aurora::NWScript::createSignature;
using Aurora::NWScript::createDefaults;

// boost-date_time stuff
using boost::posix_time::ptime;
using boost::posix_time::second_clock;

namespace Engines {

namespace NWN {

class ObjectDistanceSort {
private:
	float xt, yt, zt;

	float getDistance(Object &a) {
		float x, y, z;
		a.getPosition(x, y, z);

		return ABS(x - xt) + ABS(y - yt) + ABS(z - zt);
	}

public:
	ObjectDistanceSort(const Object &target) {
		target.getPosition(xt, yt, zt);
	}

	bool operator()(Object *a, Object *b) {
		return getDistance(*a) < getDistance(*b);
	}
};

ScriptFunctions::ScriptFunctions() {
	registerFunctions();
}

ScriptFunctions::~ScriptFunctions() {
	FunctionMan.clear();
}

void ScriptFunctions::setModule(Module *module) {
	_module = module;
}

Common::UString ScriptFunctions::floatToString(float f, int width, int decimals) {
	return Common::UString::sprintf("%*.*f", width, decimals, f);
}

int32 ScriptFunctions::random(int min, int max, int32 n) {
	if (n < 1)
		n = 1;

	int32 r = 0;

	while (n-- > 0)
		r += std::rand() % (max - min + 1) + min;

	return r;
}

Aurora::NWScript::Object *ScriptFunctions::getPC() {
	if (!_module)
		return 0;

	return (Aurora::NWScript::Object *) _module->getPC();
}

Common::UString ScriptFunctions::gTag(const Aurora::NWScript::Object *o) {
	if (o)
		return Common::UString::sprintf("\"%s\"", o->getTag().c_str());

	return "0";
}

Object *ScriptFunctions::convertObject(Aurora::NWScript::Object *o) {
	Object *object = dynamic_cast<Object *>(o);
	if (!object || !object->loaded() || (object->getID() == kObjectIDInvalid))
		return 0;

	return object;
}

Creature *ScriptFunctions::convertCreature(Aurora::NWScript::Object *o) {
	Creature *creature = dynamic_cast<Creature *>(o);
	if (!creature || !creature->loaded() || (creature->getID() == kObjectIDInvalid))
		return 0;

	return creature;
}

Creature *ScriptFunctions::convertPC(Aurora::NWScript::Object *o) {
	Creature *pc = dynamic_cast<Creature *>(o);
	if (!pc || !pc->loaded() || (pc->getID() == kObjectIDInvalid) || !pc->isPC())
		return 0;

	return pc;
}

Area *ScriptFunctions::convertArea(Aurora::NWScript::Object *o) {
	return dynamic_cast<Area *>(o);
}

Module *ScriptFunctions::convertModule(Aurora::NWScript::Object *o) {
	return dynamic_cast<Module *>(o);
}

void ScriptFunctions::registerFunctions() {
	Aurora::NWScript::Signature sig;

	Aurora::NWScript::Variable defaultIntm1(-1);
	Aurora::NWScript::Variable defaultInt0(0);
	Aurora::NWScript::Variable defaultInt1(1);
	Aurora::NWScript::Variable defaultInt9(9);
	Aurora::NWScript::Variable defaultInt18(18);
	Aurora::NWScript::Variable defaultIntMale(Aurora::kGenderMale);
	Aurora::NWScript::Variable defaultFloat1_0(1.0f);
	Aurora::NWScript::Variable defaultStringEmpty("");
	Aurora::NWScript::Variable defaultObject0((Aurora::NWScript::Object *) 0);

	FunctionMan.registerFunction("Random", 0,
			boost::bind(&ScriptFunctions::random, this, _1),
			createSignature(2, kTypeInt, kTypeInt));
	FunctionMan.registerFunction("PrintString", 1,
			boost::bind(&ScriptFunctions::printString, this, _1),
			createSignature(2, kTypeVoid, kTypeString));
	FunctionMan.registerFunction("PrintFloat", 2,
			boost::bind(&ScriptFunctions::printFloat, this, _1),
			createSignature(4, kTypeString, kTypeFloat, kTypeInt, kTypeInt),
			createDefaults(2, &defaultInt18, &defaultInt9));
	FunctionMan.registerFunction("FloatToString", 3,
			boost::bind(&ScriptFunctions::floatToString, this, _1),
			createSignature(4, kTypeString, kTypeFloat, kTypeInt, kTypeInt),
			createDefaults(2, &defaultInt18, &defaultInt9));
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
			createDefaults(1, &defaultInt0));

	FunctionMan.registerFunction("ActionMoveToObject", 22,
			boost::bind(&ScriptFunctions::actionMoveToObject, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(2, &defaultInt0, &defaultFloat1_0));

	FunctionMan.registerFunction("GetArea", 24,
			boost::bind(&ScriptFunctions::getArea, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetEnteringObject", 25,
			boost::bind(&ScriptFunctions::getEnteringObject, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetExitingObject", 26,
			boost::bind(&ScriptFunctions::getExitingObject, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("GetItemPossessor", 29,
			boost::bind(&ScriptFunctions::getItemPossessor, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetItemPossessedBy", 30,
			boost::bind(&ScriptFunctions::getItemPossessedBy, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeString));
	FunctionMan.registerFunction("CreateItemOnObject", 31,
			boost::bind(&ScriptFunctions::createItemOnObject, this, _1),
			createSignature(5, kTypeObject, kTypeString, kTypeObject, kTypeInt, kTypeString),
			createDefaults(3, &defaultObject0, &defaultInt1, &defaultStringEmpty));

	FunctionMan.registerFunction("GetNearestCreature", 38,
			boost::bind(&ScriptFunctions::getNearestCreature, this, _1),
			createSignature(9, kTypeObject, kTypeInt, kTypeInt, kTypeObject,
			                   kTypeInt, kTypeInt, kTypeInt, kTypeInt, kTypeInt),
			createDefaults(6, &defaultObject0, &defaultInt1, &defaultIntm1, &defaultIntm1,
			                  &defaultIntm1, &defaultIntm1));

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

	FunctionMan.registerFunction("IntToString", 92,
			boost::bind(&ScriptFunctions::intToString, this, _1),
			createSignature(2, kTypeString, kTypeInt));

	FunctionMan.registerFunction("d2", 95,
			boost::bind(&ScriptFunctions::d2, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d3", 96,
			boost::bind(&ScriptFunctions::d3, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d4", 97,
			boost::bind(&ScriptFunctions::d4, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d6", 98,
			boost::bind(&ScriptFunctions::d6, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d8", 99,
			boost::bind(&ScriptFunctions::d8, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d10", 100,
			boost::bind(&ScriptFunctions::d10, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d12", 101,
			boost::bind(&ScriptFunctions::d12, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d20", 102,
			boost::bind(&ScriptFunctions::d20, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));
	FunctionMan.registerFunction("d100", 103,
			boost::bind(&ScriptFunctions::d100, this, _1),
			createSignature(2, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1));

	FunctionMan.registerFunction("GetRacialType", 107,
			boost::bind(&ScriptFunctions::getRacialType, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

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

	FunctionMan.registerFunction("GetAbilityScore", 139,
			boost::bind(&ScriptFunctions::getAbilityScore, this, _1),
			createSignature(4, kTypeInt, kTypeObject, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt0));

	FunctionMan.registerFunction("GetHitDice", 166,
			boost::bind(&ScriptFunctions::getHitDice, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("GetTag", 168,
			boost::bind(&ScriptFunctions::getTag, this, _1),
			createSignature(2, kTypeString, kTypeObject));

	FunctionMan.registerFunction("GetWaypointByTag", 197,
			boost::bind(&ScriptFunctions::getWaypointByTag, this, _1),
			createSignature(2, kTypeObject, kTypeString));

	FunctionMan.registerFunction("GetObjectByTag", 200,
			boost::bind(&ScriptFunctions::getObjectByTag, this, _1),
			createSignature(3, kTypeObject, kTypeString, kTypeInt),
			createDefaults(1, &defaultInt0));

	FunctionMan.registerFunction("GetIsPC", 217,
			boost::bind(&ScriptFunctions::getIsPC, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("GetNearestObjectByTag", 229,
			boost::bind(&ScriptFunctions::getNearestObjectByTag, this, _1),
			createSignature(4, kTypeObject, kTypeString, kTypeObject, kTypeInt),
			createDefaults(2, &defaultObject0, &defaultInt1));
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

	FunctionMan.registerFunction("GetPCSpeaker", 238,
			boost::bind(&ScriptFunctions::getPCSpeaker, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetStringByStrRef", 239,
			boost::bind(&ScriptFunctions::getStringByStrRef, this, _1),
			createSignature(3, kTypeString, kTypeInt, kTypeInt),
			createDefaults(1, &defaultIntMale));

	FunctionMan.registerFunction("GetModule", 242,
			boost::bind(&ScriptFunctions::getModule, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("GetName", 253,
			boost::bind(&ScriptFunctions::getName, this, _1),
			createSignature(3, kTypeString, kTypeObject, kTypeInt),
			createDefaults(1, &defaultInt0));

	FunctionMan.registerFunction("BeginConversation", 255,
			boost::bind(&ScriptFunctions::beginConversation, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject),
			createDefaults(2, &defaultStringEmpty, &defaultObject0));

	FunctionMan.registerFunction("ObjectToString", 272,
			boost::bind(&ScriptFunctions::objectToString, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeString));

	FunctionMan.registerFunction("SetCustomToken", 284,
			boost::bind(&ScriptFunctions::setCustomToken, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeString));

	FunctionMan.registerFunction("GetSkillRank", 315,
			boost::bind(&ScriptFunctions::getSkillRank, this, _1),
			createSignature(4, kTypeInt, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(2, &defaultObject0, &defaultInt0));

	FunctionMan.registerFunction("GetMaster", 319,
			boost::bind(&ScriptFunctions::getMaster, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("SetLocked", 324,
			boost::bind(&ScriptFunctions::setLocked, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("GetLocked", 325,
			boost::bind(&ScriptFunctions::getLocked, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("SetAssociateListenPatterns", 327,
			boost::bind(&ScriptFunctions::setAssociateListenPatterns, this, _1),
			createSignature(2, kTypeVoid, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("GetClassByPosition", 341,
			boost::bind(&ScriptFunctions::getClassByPosition, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));
	FunctionMan.registerFunction("GetLevelByPosition", 342,
			boost::bind(&ScriptFunctions::getLevelByPosition, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));
	FunctionMan.registerFunction("GetLevelByClass", 343,
			boost::bind(&ScriptFunctions::getLevelByClass, this, _1),
			createSignature(3, kTypeInt, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("GetHenchman", 354,
			boost::bind(&ScriptFunctions::getHenchman, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeInt),
			createDefaults(2, &defaultObject0, &defaultInt1));

	FunctionMan.registerFunction("GetGender", 358,
			boost::bind(&ScriptFunctions::getGender, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("GetAttemptedAttackTarget", 361,
			boost::bind(&ScriptFunctions::getAttemptedAttackTarget, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("GetAssociate", 364,
			boost::bind(&ScriptFunctions::getAssociate, this, _1),
			createSignature(4, kTypeObject, kTypeInt, kTypeObject, kTypeInt),
			createDefaults(2, &defaultObject0, &defaultInt1));
	FunctionMan.registerFunction("AddHenchman", 365,
			boost::bind(&ScriptFunctions::addHenchman, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, &defaultObject0));
	FunctionMan.registerFunction("RemoveHenchman", 366,
			boost::bind(&ScriptFunctions::removeHenchman, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("AddJournalQuestEntry", 367,
			boost::bind(&ScriptFunctions::addJournalQuestEntry, this, _1),
			createSignature(7, kTypeVoid, kTypeString, kTypeInt, kTypeObject,
			                   kTypeInt, kTypeInt, kTypeInt),
			createDefaults(1, &defaultInt1, &defaultInt0, &defaultInt0));

	FunctionMan.registerFunction("SendMessageToPC", 374,
			boost::bind(&ScriptFunctions::sendMessageToPC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));
	FunctionMan.registerFunction("GetAttemptedSpellTarget", 375,
			boost::bind(&ScriptFunctions::getAttemptedSpellTarget, this, _1),
			createSignature(1, kTypeObject));

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

	FunctionMan.registerFunction("GetGold", 418,
			boost::bind(&ScriptFunctions::getGold, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("PlayVoiceChat", 421,
			boost::bind(&ScriptFunctions::playVoiceChat, this, _1),
			createSignature(3, kTypeVoid, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("MusicBackgroundPlay", 425,
			boost::bind(&ScriptFunctions::musicBackgroundPlay, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));
	FunctionMan.registerFunction("MusicBackgroundStop", 426,
			boost::bind(&ScriptFunctions::musicBackgroundStop, this, _1),
			createSignature(2, kTypeVoid, kTypeObject));

	FunctionMan.registerFunction("MusicBackgroundChangeDay", 428,
			boost::bind(&ScriptFunctions::musicBackgroundChangeDay, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));
	FunctionMan.registerFunction("MusicBackgroundChangeNight", 429,
			boost::bind(&ScriptFunctions::musicBackgroundChangeNight, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeInt));

	FunctionMan.registerFunction("IsInConversation", 445,
			boost::bind(&ScriptFunctions::isInConversation, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("GetGameDifficulty", 513,
			boost::bind(&ScriptFunctions::getGameDifficulty, this, _1),
			createSignature(1, kTypeInt));

	FunctionMan.registerFunction("GetCurrentAction", 522,
			boost::bind(&ScriptFunctions::getCurrentAction, this, _1),
			createSignature(2, kTypeInt, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("GetFirstPC", 548,
			boost::bind(&ScriptFunctions::getFirstPC, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetNextPC", 549,
			boost::bind(&ScriptFunctions::getNextPC, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("MusicBackgroundGetDayTrack", 558,
			boost::bind(&ScriptFunctions::musicBackgroundGetDayTrack, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("MusicBackgroundGetNightTrack", 559,
			boost::bind(&ScriptFunctions::musicBackgroundGetNightTrack, this, _1),
			createSignature(2, kTypeInt, kTypeObject));
	FunctionMan.registerFunction("WriteTimestampedLogEntry", 560,
			boost::bind(&ScriptFunctions::writeTimestampedLogEntry, this, _1),
			createSignature(2, kTypeVoid, kTypeString));

	FunctionMan.registerFunction("Get2DAString", 710,
			boost::bind(&ScriptFunctions::get2DAString, this, _1),
			createSignature(4, kTypeString, kTypeString, kTypeString, kTypeInt));

	FunctionMan.registerFunction("SetMaxHenchmen", 746,
			boost::bind(&ScriptFunctions::setMaxHenchmen, this, _1),
			createSignature(2, kTypeVoid, kTypeInt));
	FunctionMan.registerFunction("GetMaxHenchmen", 747,
			boost::bind(&ScriptFunctions::getMaxHenchmen, this, _1),
			createSignature(1, kTypeInt));
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
	Aurora::NWScript::NCSFile *script = ctx.getCurrentScript();
	if (!script)
		throw Common::Exception("ScriptFunctions::assignCommand(): Script needed");

	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		object = ctx.getCaller();

	const Aurora::NWScript::ScriptState &state = ctx.getParams()[1].getScriptState();

	script->assign(state, object, ctx.getTriggerer());
}

void ScriptFunctions::delayCommand(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: DelayCommand: %f", ctx.getParams()[0].getFloat());
}

void ScriptFunctions::executeScript(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString script = ctx.getParams()[0].getString();

	// Max resource name length is 16, and ExecuteScript should truncate accordingly
	script.truncate(16);

	if (!ResMan.hasResource(script, Aurora::kFileTypeNCS))
		return;

	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();
	try {
		Aurora::NWScript::NCSFile ncs(script, object);

		ncs.run();
	} catch (Common::Exception &e) {
		e.add("Failed ExecuteScript(\"%s\", %s)", script.c_str(), gTag(object).c_str());

		Common::printException(e, "WARNING: ");
	}
}

void ScriptFunctions::clearAllActions(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: ClearAllActions");
}

void ScriptFunctions::actionMoveToObject(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getCaller());
	Object *moveTo = convertObject(ctx.getParams()[0].getObject());

	if (!object || !moveTo)
		return;

	warning("TODO: ActionMoveToObject: \"%s\" to \"%s\"",
			object->getTag().c_str(), moveTo->getTag().c_str());
}

void ScriptFunctions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (object)
		ctx.getReturn() = object->getArea();
}

void ScriptFunctions::getEnteringObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void ScriptFunctions::getExitingObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
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

void ScriptFunctions::getNearestCreature(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

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

	std::list<Object *> objects;

	Object *object = 0;
	if (_module->findFirstObject(_objSearchContext)) {
		if ((object = convertObject(_objSearchContext.getObject())) && (object != target))
			objects.push_back(object);

		while (_module->findNextObject(_objSearchContext))
			if ((object = convertObject(_objSearchContext.getObject())) && (object != target))
				objects.push_back(object);
	}

	objects.sort(ObjectDistanceSort(*target));

	std::list<Object *>::iterator it = objects.begin();
	for (int n = 0; (n < nth) && (it != objects.end()); ++n)
		++it;

	if (it != objects.end())
		ctx.getReturn() = *it;
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
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	warning("TODO: ActionOpenDoor: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::actionCloseDoor(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (!object)
		return;

	warning("TODO: ActionCloseDoor: \"%s\"", object->getTag().c_str());
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
	ctx.getReturn() = ctx.getParams()[0].getString();
	ctx.getReturn().getString().toupper();
}

void ScriptFunctions::getStringLowerCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString();
	ctx.getReturn().getString().tolower();
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

void ScriptFunctions::intToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::sprintf("%d", ctx.getParams()[0].getInt());
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

void ScriptFunctions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kRaceInvalid;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getRace();
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

void ScriptFunctions::getAbilityScore(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getAbilityScore(): nBaseAbilityScore

	ctx.getReturn() = 0;

	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Creature *creature = convertCreature(params[0].getObject());
	Ability   ability  = (Ability) params[1].getInt();

	if (creature)
		ctx.getReturn() = (int32) creature->getAbility(ability);
}

void ScriptFunctions::getHitDice(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = creature->getHitDice();
}

void ScriptFunctions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	if (object)
		ctx.getReturn() = object->getTag();
}

void ScriptFunctions::getWaypointByTag(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &tag = ctx.getParams()[0].getString();

	warning("TODO: GetWaypointByTag: \"%s\"", tag.c_str());
}

void ScriptFunctions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Object *) 0;
	if (!_module)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	int nth = ctx.getParams()[1].getInt();

	if (!_module->findFirstObject(tag, _objSearchContext))
		return;

	while (nth-- > 0)
		_module->findNextObject(tag, _objSearchContext);

	ctx.getReturn() = _objSearchContext.getObject();
}

void ScriptFunctions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = convertPC(ctx.getParams()[0].getObject()) != 0;
}

void ScriptFunctions::getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Object *) 0;
	if (!_module)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	Object *target = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		target = convertObject(ctx.getCaller());
	if (!target)
		return;

	std::list<Object *> objects;

	// TODO: ScriptFunctions::getNearestObjectByTag(): Only consider objects in the same area
	Object *object = 0;
	if (_module->findFirstObject(tag, _objSearchContext)) {
		if ((object = convertObject(_objSearchContext.getObject())) && (object != target))
			objects.push_back(object);

		while (_module->findNextObject(tag, _objSearchContext))
			if ((object = convertObject(_objSearchContext.getObject())) && (object != target))
				objects.push_back(object);
	}

	objects.sort(ObjectDistanceSort(*target));

	int nth = ctx.getParams()[2].getInt() - 1;

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
	sscanf(ctx.getParams()[0].getString().c_str(), "%d", &i);

	ctx.getReturn() = (int32) i;
}

void ScriptFunctions::stringToFloat(Aurora::NWScript::FunctionContext &ctx) {
	float f;
	sscanf(ctx.getParams()[0].getString().c_str(), "%f", &f);

	ctx.getReturn() = f;
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
	const Aurora::Gender gender = (Aurora::Gender) ctx.getParams()[1].getInt();

	ctx.getReturn() = TalkMan.getString(strRef, gender);
}

void ScriptFunctions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _module;
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

void ScriptFunctions::beginConversation(Aurora::NWScript::FunctionContext &ctx) {
	if (!_module)
		throw Common::Exception("ScriptFunctions::beginConversation(): Module needed");

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
		throw Common::Exception("ScriptFunctions::beginConversation(): "
		                        "Need one PC and one object");

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

	_module->startConversation(conversation, *pc, *object);
}

void ScriptFunctions::objectToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::sprintf("%p", (void *) ctx.getParams()[0].getObject());
}

void ScriptFunctions::setCustomToken(Aurora::NWScript::FunctionContext &ctx) {
	int32 tokenNumber = ctx.getParams()[0].getInt();
	const Common::UString &tokenValue = ctx.getParams()[1].getString();

	const Common::UString tokenName = Common::UString::sprintf("<CUSTOM%d>", tokenNumber);

	TokenMan.set(tokenName, tokenValue);
}

void ScriptFunctions::getSkillRank(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1;

	Object *object = convertObject(ctx.getParams()[1].getObject());
	if (ctx.getParamsSpecified() < 2)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	int  skill = ctx.getParams()[0].getInt();
	bool base  = ctx.getParams()[2].getInt() != 0;

	warning("TODO: GetSkillRank: \"%s\", %d, %d", object->getTag().c_str(), skill, base);
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

void ScriptFunctions::setLocked(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	bool locked = ctx.getParams()[1].getInt() != 0;
	warning("TODO: SetLocked: \"%s\" to %d", object->getTag().c_str(), locked);
}

void ScriptFunctions::getLocked(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (!object)
		return;

	warning("TODO: GetLocked: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::setAssociateListenPatterns(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());
	if (!object)
		return;

	warning("TODO: SetAssociateListenPatterns: \"%s\"", object->getTag().c_str());
}

void ScriptFunctions::getClassByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kClassInvalid;

	uint32 position = (uint32) ctx.getParams()[0].getInt();

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

	uint32 position = (uint32) ctx.getParams()[0].getInt();

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

void ScriptFunctions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kGenderNone;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getGender();
}

void ScriptFunctions::getAttemptedAttackTarget(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = convertCreature(ctx.getCaller());
	if (!creature)
		return;

	warning("TODO: GetAttemptedAttackTarget: \"%s\"", creature->getTag().c_str());
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

void ScriptFunctions::getGold(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());

	if (!object)
		return;

	warning("TODO: GetGold: \"%s\"", object->getTag().c_str());
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

void ScriptFunctions::isInConversation(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (object)
		ctx.getReturn() = object->getPCSpeaker() != 0;
}

void ScriptFunctions::getGameDifficulty(Aurora::NWScript::FunctionContext &ctx) {
	// The scripts have another difficulty "Very Easy", which we don't recognize.
	ctx.getReturn() = (int32) ((GameDifficulty) (ConfigMan.getInt("difficulty") + 1));
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

void ScriptFunctions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) getPC();
}

void ScriptFunctions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
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
	const Common::UString tstamp = Common::UString::sprintf("%04d-%02d-%02dT%02d:%02d:%02d",
		(int) t.date().year(), (int) t.date().month(), (int) t.date().day(),
		(int) t.time_of_day().hours(), (int) t.time_of_day().minutes(),
		(int) t.time_of_day().seconds());

	status("NWN: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
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

void ScriptFunctions::setMaxHenchmen(Aurora::NWScript::FunctionContext &ctx) {
	int n = ctx.getParams()[0].getInt();

	warning("TODO: SetMaxHenchmen: %d", n);
}

void ScriptFunctions::getMaxHenchmen(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: GetMaxHenchmen");
	ctx.getReturn() = 0;
}

} // End of namespace NWN

} // End of namespace Engines
