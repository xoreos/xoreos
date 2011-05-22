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

	FunctionMan.registerFunction("BeginConversation",
			boost::bind(&ScriptFunctions::beginConversation, this, _1),
			createSignature(3, kTypeVoid, kTypeString, kTypeObject));
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

	Common::UString conversation = params[0].getString();
	if (conversation.empty())
		conversation = object->getConversation();

	_module->startConversation(conversation, *pc, *object);
}

} // End of namespace NWN

} // End of namespace Engines
