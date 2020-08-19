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

#include <cassert>

#include <memory>

#include "external/lua/lualib.h"

#include "external/toluapp/tolua++.h"

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/resman.h"
#include "src/aurora/util.h"

#include "src/aurora/lua/scriptman.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/stackguard.h"
#include "src/aurora/lua/variable.h"
#include "src/aurora/lua/table.h"
#include "src/aurora/lua/function.h"

DECLARE_SINGLETON(Aurora::Lua::ScriptManager)

namespace Aurora {

namespace Lua {

ScriptManager::ScriptManager() : _luaState(0), _regNestingLevel(0) {

}

ScriptManager::~ScriptManager() {
	try {
		deinit();
	} catch (...) {
	}
}

void ScriptManager::init() {
	assert(!ready());

	openLuaState();
	registerDefaultBindings();
	executeDefaultCode();
}

void ScriptManager::deinit() {
	if (!_objectLuaInstances.empty()) {
		warning("Lua instances were not freed properly");

		_objectLuaInstances.clear();
	}

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

	std::unique_ptr<Common::SeekableReadStream> stream(ResMan.getResource(path, kFileTypeLUC));
	if (!stream) {
		const Common::UString fileName = TypeMan.setFileType(path, kFileTypeLUC);
		throw Common::Exception("No such LUC \"%s\"", fileName.c_str());
	}

	std::unique_ptr<Common::MemoryReadStream> memStream(stream->readStream(stream->size()));
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

	if (parts.empty()) {
		return getGlobalFunction(funcName).call(params);
	}

	TableRef table = getGlobalTable(parts[0]);
	for (uint32_t i = 1; i < parts.size(); ++i) {
		table = table.getTableAt(parts[i]);
	}
	return table.getFunctionAt(funcName).call(params);
}

Variables ScriptManager::callFunction(const Common::UString &name) {
	return callFunction(name, Variables());
}

Variable ScriptManager::getGlobalVariable(const Common::UString &name) const {
	StackGuard guard(*_luaState);

	lua_getglobal(_luaState, name.c_str());

	Stack stack(*_luaState);
	return stack.getVariableAt(-1);
}

TableRef ScriptManager::getGlobalTable(const Common::UString &name) const {
	return getGlobalVariable(name).getTable();
}

FunctionRef ScriptManager::getGlobalFunction(const Common::UString &name) const {
	return getGlobalVariable(name).getFunction();
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

void ScriptManager::beginRegisterClass(const Common::UString &name, const Common::UString &baseName,
	                                   lua_CFunction deleter) {

	assert(!name.empty());
	assert(_luaState && _regNestingLevel > 0);

	++_regNestingLevel;

	requireDeclaredClass(name);
	requireDeclaredClass(baseName);

	tolua_cclass(_luaState, name.c_str(), name.c_str(), baseName.c_str(), deleter);
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

void ScriptManager::setLuaInstanceForObject(void *object, const TableRef &luaInstance) {
	assert(object);
	// TODO: Commented out to make stubs work
	// assert(_objectLuaInstances.find(object) == _objectLuaInstances.end());

	_objectLuaInstances[object] = luaInstance;
}

void ScriptManager::unsetLuaInstanceForObject(void *object) {
	assert(object);

	_objectLuaInstances.erase(object);
}

const TableRef &ScriptManager::getLuaInstanceForObject(void *object) const {
	assert(object);

	static const TableRef invalidInstance;

	ObjectLuaInstanceMap::const_iterator found = _objectLuaInstances.find(object);
	if (found == _objectLuaInstances.end())
	{
		return invalidInstance;
	}
	return found->second;
}

void ScriptManager::injectNewIndexMetaEventIntoTable(const TableRef &table) {
	const FunctionRef fn = getGlobalFunction("mt_new_index");
	TableRef metaTable = table.getMetaTable();
	metaTable.setFunctionAt("__newindex", fn);
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

	registerFunction("getLua", &ScriptManager::luaGetLua);

	beginRegisterClass("ScriptManager");
	registerFunction("PlayFile", &ScriptManager::luaPlayFile);
	registerFunction("SetGCInterval", &ScriptManager::luaSetGCInterval);
	registerFunction("RegisterSubst", &ScriptManager::luaRegisterSubst);
	registerFunction("UnregisterSubst", &ScriptManager::luaUnregisterSubst);
	registerFunction("RegisterHandler", &ScriptManager::luaRegisterHandler);
	endRegisterClass();

	endRegister();
}

void ScriptManager::executeDefaultCode() {
	static const Common::UString fnNewIndexSource =
	    "function mt_new_index(table, key, value)"
	    "    local objClass = getmetatable(table).__objectClass"
	    "    if objClass ~= nil then"
	    "        local cppInstance = rawget(table, \"CPP_instance\")"
	    "        if cppInstance[key] ~= nil then"
	    "            cppInstance[key] = value"
	    "        end"
	    "        return"
	    "    end"
	    "    rawset(table, key, value)"
	    "end";

	executeString(fnNewIndexSource);
}

int ScriptManager::atPanic(lua_State *state) {
	const char *message = luaL_checkstring(state, -1);
	error("Lua has panicked: %s", message);
	lua_pop(state, 1);
	return 0;
}

int ScriptManager::luaGetLua(lua_State *state) {
	assert(state);

	Stack stack(*state);
	stack.pushUserType<ScriptManager>(LuaScriptMan, "ScriptManager");
	return 1;
}

int ScriptManager::luaPlayFile(lua_State *state) {
	assert(state);

	Stack stack(*state);
	assert(stack.getSize() > 1);

	const Common::UString scriptFile = stack.getStringAt(2);
	LuaScriptMan.executeFile(scriptFile);
	return 0;
}

int ScriptManager::luaSetGCInterval(lua_State *UNUSED(state)) {
	// TODO
	return 0;
}

int ScriptManager::luaRegisterSubst(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 3);

	ScriptManager* scriptMan = stack.getUserTypeAt<ScriptManager>(1);
	assert(scriptMan == &LuaScriptMan);
	void* object = stack.getRawUserTypeAt(2);
	assert(object);
	const TableRef instance = stack.getTableAt(3);

	scriptMan->setLuaInstanceForObject(object, instance);
	return 0;
}

int ScriptManager::luaUnregisterSubst(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	ScriptManager* scriptMan = stack.getUserTypeAt<ScriptManager>(1);
	assert(scriptMan == &LuaScriptMan);
	void* object = stack.getRawUserTypeAt(2);
	assert(object);

	scriptMan->unsetLuaInstanceForObject(object);
	return 0;
}

int ScriptManager::luaRegisterHandler(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getTypeAt(1) == Aurora::Lua::kTypeUserType);
	assert(stack.getTypeAt(2) == Aurora::Lua::kTypeUserType);
	assert(stack.getTypeAt(3) == Aurora::Lua::kTypeString);

	// TODO
	return 0;
}

} // End of namespace Lua

} // End of namespace Aurora

