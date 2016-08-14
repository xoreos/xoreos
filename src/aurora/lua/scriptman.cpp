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
 *  Lua script manager.
 */

#include <boost/scoped_ptr.hpp>

extern "C" {

#include "lua/lualib.h"

#include "toluapp/tolua++.h"

}

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/resman.h"
#include "src/aurora/util.h"

#include "src/aurora/lua/scriptman.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/variable.h"

DECLARE_SINGLETON(Aurora::Lua::ScriptManager)

namespace Aurora {

namespace Lua {

ScriptManager::ScriptManager() : _luaState(0), _regNestingLevel(0) {

}

ScriptManager::~ScriptManager() {
	closeLuaState();
}

void ScriptManager::init() {
	assert(!ready());

	openLuaState();
	registerDefaultBindings();
}

void ScriptManager::deinit() {
	assert(ready());

	closeLuaState();
}

bool ScriptManager::ready() const {
	return _luaState != 0;
}

void ScriptManager::executeFile(const Common::UString &path) {
	assert(_luaState && _regNestingLevel == 0);

	if (isIgnoredFile(path)) {
		return;
	}

	boost::scoped_ptr<Common::SeekableReadStream> stream(ResMan.getResource(path, kFileTypeLUC));
	if (!stream) {
		const Common::UString fileName = TypeMan.setFileType(path, kFileTypeLUC);
		throw Common::Exception("No such LUC \"%s\"", fileName.c_str());
	}

	boost::scoped_ptr<Common::MemoryReadStream> memStream(stream->readStream(stream->size()));
	const char *data = reinterpret_cast<const char *>(memStream->getData());
	const int dataSize = memStream->size();

	const int execResult = lua_dobuffer(_luaState, data, dataSize, path.c_str());
	if (execResult != 0) {
		const Common::UString fileName = TypeMan.setFileType(path, kFileTypeLUC);
		throw Common::Exception("Failed to execute Lua file: %s", fileName.c_str());
	}
}

void ScriptManager::executeString(const Common::UString &code) {
	assert(_luaState && _regNestingLevel == 0);

	const int execResult = lua_dostring(_luaState, code.c_str());
	if (execResult != 0) {
		throw Common::Exception("Failed to execute Lua code: %s", code.c_str());
	}
}

Variables ScriptManager::callFunction(const Common::UString &name, const Variables &params) {
	assert(!name.empty());
	assert(_luaState && _regNestingLevel == 0);

	std::vector<Common::UString> parts;
	Common::UString::split(name, '.', parts);
	if (parts.empty()) {
		error("Lua call \"%s\" failed: bad name", name.c_str());
		return Variables();
	}

	const Common::UString funcName = parts.back();
	parts.pop_back();

	// Retrieve the global table
	if (!parts.empty()) {
		lua_getglobal(_luaState, parts[0].c_str());
		if (!lua_istable(_luaState, -1)) {
			const char *msg = "Lua call \"%s\" failed: \"%s\" is not a table";
			error(msg, name.c_str(), parts[0].c_str());
			return Variables();
		}
	}

	// Retrieve inner tables
	for (uint32 i = 1; i < parts.size(); ++i) {
		lua_pushstring(_luaState, parts[i].c_str());
		lua_gettable(_luaState, -2);
		if (!lua_istable(_luaState, -1)) {
			const char *msg = "Lua call \"%s\" failed: \"%s\" is not a table";
			error(msg, name.c_str(), parts[i].c_str());
			return Variables();
		}
	}

	// Retrieve the function
	const int savedTop = lua_gettop(_luaState);
	if (parts.empty()) {
		lua_getglobal(_luaState, funcName.c_str());
	} else {
		lua_pushstring(_luaState, funcName.c_str());
		lua_gettable(_luaState, -2);
	}
	if (!lua_isfunction(_luaState, -1)) {
		const char *msg = "Lua call \"%s\" failed: \"%s\" is not a function";
		error(msg, name.c_str(), funcName.c_str());
		return Variables();
	}

	// Perform the actual call
	Stack stack(*_luaState);
	stack.pushVariables(params);

	if (lua_pcall(&stack.getLuaState(), params.size(), LUA_MULTRET, 0) != 0) {
		const char *msg = "Lua call \"%s\" failed: %s";
		const Common::UString luaError = stack.getStringAt(-1);
		error(msg, name.c_str(), luaError.c_str());
		return Variables();
	}

	const int retsCount = stack.getSize() - savedTop;
	const Variables rets = stack.getVariablesFromTop(retsCount);
	return rets;
}

void ScriptManager::addIgnoredFile(const Common::UString &path) {
	_ignoredFiles.insert(path);
}

void ScriptManager::removeIgnoredFile(const Common::UString &path) {
	_ignoredFiles.erase(path);
}

bool ScriptManager::isIgnoredFile(const Common::UString &path) const {
	return _ignoredFiles.find(path) != _ignoredFiles.end();
}

void ScriptManager::declareClass(const Common::UString &name) {
	assert(!name.empty());
	assert(_luaState && _regNestingLevel == 0);

	tolua_usertype(_luaState, name.c_str());
}

void ScriptManager::beginRegister() {
	assert(_luaState && _regNestingLevel == 0);

	++_regNestingLevel;

	tolua_module(_luaState, 0, 1);
	tolua_beginmodule(_luaState, 0);
}

void ScriptManager::endRegister() {
	assert(_luaState && _regNestingLevel == 1);

	--_regNestingLevel;

	tolua_endmodule(_luaState);
}

void ScriptManager::beginRegisterNamespace(const Common::UString &name) {
	assert(!name.empty());
	assert(_luaState && _regNestingLevel > 0);

	++_regNestingLevel;

	tolua_module(_luaState, name.c_str(), 1);
	tolua_beginmodule(_luaState, name.c_str());
}

void ScriptManager::endRegisterNamespace() {
	assert(_luaState && _regNestingLevel > 1);

	--_regNestingLevel;

	tolua_endmodule(_luaState);
}

void ScriptManager::beginRegisterClass(const Common::UString &name, const Common::UString &baseName) {
	assert(!name.empty());
	assert(_luaState && _regNestingLevel > 0);

	++_regNestingLevel;

	requireDeclaredClass(name);
	requireDeclaredClass(baseName);

	tolua_cclass(_luaState, name.c_str(), name.c_str(), baseName.c_str(), 0);
	tolua_beginmodule(_luaState, name.c_str());
}

void ScriptManager::endRegisterClass() {
	assert(_luaState && _regNestingLevel > 1);

	--_regNestingLevel;

	tolua_endmodule(_luaState);
}

void ScriptManager::registerConstant(const Common::UString &name, float value) {
	assert(!name.empty());
	assert(_luaState &&_regNestingLevel > 0);

	tolua_constant(_luaState, name.c_str(), value);
}

void ScriptManager::registerVariable(const Common::UString &name, lua_CFunction getter, lua_CFunction setter) {
	assert(!name.empty() && getter);
	assert(_luaState && _regNestingLevel > 0);

	tolua_variable(_luaState, name.c_str(), getter, setter);
}

void ScriptManager::registerFunction(const Common::UString &name, lua_CFunction func) {
	assert(!name.empty() && func);
	assert(_luaState && _regNestingLevel > 0);

	tolua_function(_luaState, name.c_str(), func);
}

int ScriptManager::getUsedMemoryAmount() const {
	return lua_getgccount(_luaState);
}

void ScriptManager::openLuaState() {
	_luaState = lua_open();
	if (!_luaState) {
		throw Common::Exception("Failed to open Lua state");
	}

	luaopen_base(_luaState);
	luaopen_io(_luaState);
	luaopen_math(_luaState);
	luaopen_string(_luaState);
	luaopen_table(_luaState);
	luaopen_loadlib(_luaState);
	luaopen_debug(_luaState);

	tolua_open(_luaState);

	lua_atpanic(_luaState, &ScriptManager::atPanic);
}

void ScriptManager::closeLuaState() {
	if (_luaState) {
		lua_close(_luaState);
		_luaState = 0;
	}
	_regNestingLevel = 0;
}

void ScriptManager::requireDeclaredClass(const Common::UString &name) const {
	assert(_luaState);

	if (name.empty()) {
		return;
	}

	luaL_getmetatable(_luaState, name.c_str());
	if (lua_isnil(_luaState, -1)) {
		lua_pop(_luaState, 1);
		throw Common::Exception("Class \"%s\" wasn't declared for Lua", name.c_str());
	}
	lua_pop(_luaState, 1);
}

void ScriptManager::registerDefaultBindings() {
	declareClass("ScriptManager");

	beginRegister();

	registerFunction("getLua", &ScriptManager::getLua);

	beginRegisterClass("ScriptManager");
	registerFunction("PlayFile", &ScriptManager::playFile);
	registerFunction("SetGCInterval", &ScriptManager::setGCInterval);
	endRegisterClass();

	endRegister();
}

int ScriptManager::atPanic(lua_State *state) {
	const char *message = luaL_checkstring(state, -1);
	error("Lua has panicked: %s", message);
	lua_pop(state, 1);
	return 0;
}

int ScriptManager::getLua(lua_State *state) {
	assert(state);

	Stack stack(*state);
	stack.pushUserType<ScriptManager>(LuaScriptMan, "ScriptManager");
	return 1;
}

int ScriptManager::playFile(lua_State *state) {
	assert(state);

	Stack stack(*state);
	assert(stack.getSize() > 1);

	const Common::UString scriptFile = stack.getStringAt(2);
	LuaScriptMan.executeFile(scriptFile);
	return 0;
}

int ScriptManager::setGCInterval(lua_State *UNUSED(state)) {
	// TODO
	return 0;
}

} // End of namespace Lua

} // End of namespace Aurora

