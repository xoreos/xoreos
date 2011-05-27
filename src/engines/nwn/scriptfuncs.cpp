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

#include "common/util.h"
#include "common/error.h"

#include "engines/nwn/scriptfuncs.h"
#include "engines/nwn/object.h"
#include "engines/nwn/creature.h"
#include "engines/nwn/module.h"
#include "engines/nwn/area.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/functioncontext.h"
#include "aurora/nwscript/functionman.h"

using Aurora::NWScript::kTypeVoid;
using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::createSignature;

namespace Engines {

namespace NWN {

ScriptFunctions::ScriptFunctions() {
	registerFunctions();
}

ScriptFunctions::~ScriptFunctions() {
	FunctionMan.clear();
}

void ScriptFunctions::setModule(Module *module) {
	_module = module;
}

Aurora::NWScript::Object *ScriptFunctions::getPC() {
	if (!_module)
		return 0;

	return (Aurora::NWScript::Object *) _module->getPC();
}

Object *ScriptFunctions::convertObject(Aurora::NWScript::Object *o) {
	Object *object = dynamic_cast<Object *>(o);
	if (!object || !object->loaded())
		return 0;

	return object;
}

Creature *ScriptFunctions::convertCreature(Aurora::NWScript::Object *o) {
	Creature *creature = dynamic_cast<Creature *>(o);
	if (!creature || !creature->loaded())
		return 0;

	return creature;
}

Creature *ScriptFunctions::convertPC(Aurora::NWScript::Object *o) {
	Creature *pc = dynamic_cast<Creature *>(o);
	if (!pc || !pc->loaded() || !pc->isPC())
		return 0;

	return pc;
}

Area *ScriptFunctions::convertArea(Aurora::NWScript::Object *o) {
	return dynamic_cast<Area *>(o);
}

void ScriptFunctions::registerFunctions() {
	Aurora::NWScript::Signature sig;

	Aurora::NWScript::Variable defaultInt0(0);
	Aurora::NWScript::Variable defaultInt1(1);
	Aurora::NWScript::Variable defaultFloat1_0(1.0f);
	Aurora::NWScript::Variable defaultStringEmpty("");
	Aurora::NWScript::Variable defaultObject0((Aurora::NWScript::Object *) 0);

	FunctionMan.registerFunction("Random", 0,
			boost::bind(&ScriptFunctions::random, this, _1),
			createSignature(2, kTypeInt, kTypeInt));

	FunctionMan.registerFunction("ActionMoveToObject", 22,
			boost::bind(&ScriptFunctions::actionMoveToObject, this, _1),
			createSignature(4, kTypeVoid, kTypeObject, kTypeInt, kTypeFloat),
			createDefaults(2, &defaultInt0, &defaultFloat1_0));

	FunctionMan.registerFunction("GetArea", 24,
			boost::bind(&ScriptFunctions::getArea, this, _1),
			createSignature(2, kTypeObject, kTypeObject));

	FunctionMan.registerFunction("GetItemPossessor", 29,
			boost::bind(&ScriptFunctions::getItemPossessor, this, _1),
			createSignature(2, kTypeObject, kTypeObject));
	FunctionMan.registerFunction("GetItemPossessedBy", 30,
			boost::bind(&ScriptFunctions::getItemPossessedBy, this, _1),
			createSignature(3, kTypeObject, kTypeObject, kTypeString));

	FunctionMan.registerFunction("GetIsObjectValid", 42,
			boost::bind(&ScriptFunctions::getObjectIsValid, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

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

	FunctionMan.registerFunction("IntToString", 92,
			boost::bind(&ScriptFunctions::intToString, this, _1),
			createSignature(2, kTypeString, kTypeInt));

	FunctionMan.registerFunction("GetRacialType", 107,
			boost::bind(&ScriptFunctions::getRacialType, this, _1),
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

	FunctionMan.registerFunction("GetPCSpeaker", 238,
			boost::bind(&ScriptFunctions::getPCSpeaker, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("GetModule", 242,
			boost::bind(&ScriptFunctions::getModule, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("BeginConversation", 255,
			boost::bind(&ScriptFunctions::beginConversation, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject),
			createDefaults(2, &defaultStringEmpty, &defaultObject0));

	FunctionMan.registerFunction("GetMaster", 319,
			boost::bind(&ScriptFunctions::getMaster, this, _1),
			createSignature(2, kTypeObject, kTypeObject),
			createDefaults(1, &defaultObject0));

	FunctionMan.registerFunction("GetGender", 358,
			boost::bind(&ScriptFunctions::getGender, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("SendMessageToPC", 374,
			boost::bind(&ScriptFunctions::sendMessageToPC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));

	FunctionMan.registerFunction("GetGold", 418,
			boost::bind(&ScriptFunctions::getGold, this, _1),
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

}

void ScriptFunctions::random(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = std::rand() %  ctx.getParams()[0].getInt();
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

void ScriptFunctions::getObjectIsValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = convertObject(ctx.getParams()[0].getObject()) != 0;
}

void ScriptFunctions::getLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeInt).getInt();
}

void ScriptFunctions::getLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeFloat).getFloat();
}

void ScriptFunctions::getLocalString(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeString).getString();
}

void ScriptFunctions::getLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		ctx.getReturn() = object->getVariable(params[1].getString(), kTypeObject).getObject();
}

void ScriptFunctions::setLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		object->setVariable(params[1].getString(), params[2].getInt());
}

void ScriptFunctions::setLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		object->setVariable(params[1].getString(), params[2].getFloat());
}

void ScriptFunctions::setLocalString(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		object->setVariable(params[1].getString(), params[2].getString());
}

void ScriptFunctions::setLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Object *object = convertObject(params[0].getObject());
	if (object)
		object->setVariable(params[1].getString(), params[2].getObject());
}

void ScriptFunctions::intToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::sprintf("%d", ctx.getParams()[0].getInt());
}

void ScriptFunctions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kRaceInvalid;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getRace();
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

	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (object)
		ctx.getReturn() = object->getTag();
}

void ScriptFunctions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getObjectByTag(): nNth

	ctx.getReturn() = (Object *) 0;

	const Common::UString &tag = ctx.getParams()[0].getString();

	if (_module)
		ctx.getReturn() = _module->findObject(tag);
}

void ScriptFunctions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = convertPC(ctx.getParams()[0].getObject()) != 0;
}

void ScriptFunctions::getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: ScriptFunctions::getNearestObjectByTag(): /Nearest/ Object

	ctx.getReturn() = (Object *) 0;

	const Common::UString &tag = ctx.getParams()[0].getString();

	if (_module)
		ctx.getReturn() = _module->findObject(tag);
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

void ScriptFunctions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) getPC();
}

void ScriptFunctions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

void ScriptFunctions::getPCSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *speaker = 0;
	Object *object = convertObject(ctx.getCaller());
	if (object)
		speaker = object->getPCSpeaker();

	ctx.getReturn() = speaker;
}

void ScriptFunctions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _module;
}

void ScriptFunctions::getMaster(Aurora::NWScript::FunctionContext &ctx) {
	Object *object = convertObject(ctx.getParams()[0].getObject());
	if (ctx.getParamsSpecified() < 1)
		object = convertObject(ctx.getCaller());

	ctx.getReturn() = object;
}

void ScriptFunctions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kGenderNone;

	Creature *creature = convertCreature(ctx.getParams()[0].getObject());
	if (creature)
		ctx.getReturn() = (int32) creature->getGender();
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

void ScriptFunctions::sendMessageToPC(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Creature *pc = convertPC(params[0].getObject());
	if (!pc)
		return;

	const Common::UString &msg = params[1].getString();

	warning("Send message to PC \"%s\": \"%s\"", pc->getName().c_str(), msg.c_str());
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

} // End of namespace NWN

} // End of namespace Engines
