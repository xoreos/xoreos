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

/** @file engines/nwn/scriptfuncs.h
 *  NWN script functions.
 */

#ifndef ENGINES_NWN_SCRIPTFUNCS_H
#define ENGINES_NWN_SCRIPTFUNCS_H

namespace Aurora {
	namespace NWScript {
		class Object;
		class FunctionContext;
	}
}

namespace Engines {

namespace NWN {

class Module;

class Object;
class Creature;

class ScriptFunctions {
public:
	ScriptFunctions();
	~ScriptFunctions();

	void setModule(Module *module = 0);

private:
	Module *_module;

	void registerFunctions();

	Aurora::NWScript::Object *getPC();

	Object   *convertObject  (Aurora::NWScript::Object *o);
	Creature *convertCreature(Aurora::NWScript::Object *o);
	Creature *convertPC      (Aurora::NWScript::Object *o);

	void getObjectIsValid(Aurora::NWScript::FunctionContext &ctx);

	void getLocalInt(Aurora::NWScript::FunctionContext &ctx);
	void getLocalFloat(Aurora::NWScript::FunctionContext &ctx);
	void getLocalString(Aurora::NWScript::FunctionContext &ctx);
	void getLocalObject(Aurora::NWScript::FunctionContext &ctx);
	void setLocalInt(Aurora::NWScript::FunctionContext &ctx);
	void setLocalFloat(Aurora::NWScript::FunctionContext &ctx);
	void setLocalString(Aurora::NWScript::FunctionContext &ctx);
	void setLocalObject(Aurora::NWScript::FunctionContext &ctx);

	void intToString(Aurora::NWScript::FunctionContext &ctx);

	void getTag(Aurora::NWScript::FunctionContext &ctx);

	void getPCSpeaker(Aurora::NWScript::FunctionContext &ctx);

	void getModule(Aurora::NWScript::FunctionContext &ctx);

	void beginConversation(Aurora::NWScript::FunctionContext &ctx);

	void sendMessageToPC(Aurora::NWScript::FunctionContext &ctx);

	void getFirstPC(Aurora::NWScript::FunctionContext &ctx);
	void getNextPC(Aurora::NWScript::FunctionContext &ctx);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SCRIPTFUNCS_H
