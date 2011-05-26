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

void ScriptFunctions::registerFunctions() {
	Aurora::NWScript::Signature sig;

	FunctionMan.registerFunction("GetIsObjectValid", 42,
			boost::bind(&ScriptFunctions::getObjectIsValid, this, _1),
			createSignature(2, kTypeInt, kTypeObject));

	FunctionMan.registerFunction("IntToString", 92,
			boost::bind(&ScriptFunctions::intToString, this, _1),
			createSignature(2, kTypeString, kTypeInt));

	FunctionMan.registerFunction("GetPCSpeaker", 238,
			boost::bind(&ScriptFunctions::getPCSpeaker, this, _1),
			createSignature(1, kTypeObject));

	FunctionMan.registerFunction("BeginConversation", 255,
			boost::bind(&ScriptFunctions::beginConversation, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject));

	FunctionMan.registerFunction("SendMessageToPC", 374,
			boost::bind(&ScriptFunctions::sendMessageToPC, this, _1),
			createSignature(3, kTypeVoid, kTypeObject, kTypeString));

	FunctionMan.registerFunction("GetFirstPC", 548,
			boost::bind(&ScriptFunctions::getFirstPC, this, _1),
			createSignature(1, kTypeObject));
	FunctionMan.registerFunction("GetNextPC", 549,
			boost::bind(&ScriptFunctions::getNextPC, this, _1),
			createSignature(1, kTypeObject));

}

void ScriptFunctions::getObjectIsValid(Aurora::NWScript::FunctionContext &ctx) {
	const Object *obj = dynamic_cast<const Object *>(ctx.getParams()[0].getObject());

	ctx.getReturn() = (int32) (obj && obj->loaded());
}

void ScriptFunctions::intToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::sprintf("%d", ctx.getParams()[0].getInt());
}

void ScriptFunctions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) getPC();
}

void ScriptFunctions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

void ScriptFunctions::getPCSpeaker(Aurora::NWScript::FunctionContext &ctx) {
	Object *speaker = 0;
	Object *object  = dynamic_cast<Object *>(ctx.getCaller());
	if (object)
		speaker = object->getPCSpeaker();

	ctx.getReturn() = (Aurora::NWScript::Object *) speaker;
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
	Creature *pc     = dynamic_cast<Creature *>(obj2);
	Object   *object = dynamic_cast<Object *>(obj1);

	// Try the other way round, if necessary
	if (!pc || !pc->isPC()) {
		pc     = dynamic_cast<Creature *>(obj1);
		object = dynamic_cast<Object *>(obj2);
	}

	// Fail
	if (!pc || !object || !pc->isPC())
		throw Common::Exception("ScriptFunctions::beginConversation(): "
		                        "Need one PC and one object");

	if (object->getPCSpeaker())
		throw Common::Exception("ScriptFunctions::beginConversation(): "
		                        "Object already in conversation");

	Common::UString conversation = params[0].getString();
	if (conversation.empty())
		conversation = object->getConversation();

	object->setPCSpeaker(pc);
	_module->startConversation(conversation, *pc, *object);
	object->setPCSpeaker(0);
}

void ScriptFunctions::sendMessageToPC(Aurora::NWScript::FunctionContext &ctx) {
	const Aurora::NWScript::Parameters &params = ctx.getParams();

	Creature *pc = dynamic_cast<Creature *>(params[0].getObject());
	if (!pc || !pc->isPC())
		return;

	const Common::UString &msg = params[1].getString();

	warning("Send message to PC \"%s\": \"%s\"", pc->getName().c_str(), msg.c_str());
}

} // End of namespace NWN

} // End of namespace Engines
