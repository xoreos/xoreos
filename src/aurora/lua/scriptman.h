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

#ifndef ENGINES_AURORA_LUA_SCRIPTMAN_H
#define ENGINES_AURORA_LUA_SCRIPTMAN_H

#include <cassert>

#include <set>
#include <map>

#include "src/common/singleton.h"
#include "src/common/ustring.h"

#include "src/aurora/lua/types.h"

namespace Aurora {

namespace Lua {

/** Lua script manager. */
class ScriptManager : public Common::Singleton<ScriptManager> {
public:
	ScriptManager();
	~ScriptManager();

	/** Initialize the script subsystem. */
	void init();
	/** Deinitialize the script subsystem. */
	void deinit();

	/** Was the script subsystem successfully initialized? */
	bool ready() const;

	/** Execute a script file. */
	void executeFile(const Common::UString &path);
	/** Execute a script string. */
	void executeString(const Common::UString &code);

	/** Call a Lua function.
	 *  A "dot" syntax is used to call class methods or table functions.
	 *  For example, callFunction("module.Class.method", params).
	 */
	Variables callFunction(const Common::UString &name, const Variables &params);
	Variables callFunction(const Common::UString &name);

	Variable getGlobalVariable(const Common::UString &name) const;
	TableRef getGlobalTable(const Common::UString &name) const;
	FunctionRef getGlobalFunction(const Common::UString &name) const;

	/** Add a file to the ignore list. */
	void addIgnoredFile(const Common::UString &path);
	/** Remove a file from the ignore list. */
	void removeIgnoredFile(const Common::UString &path);
	/** Is this file in the ignore list? */
	bool isIgnoredFile(const Common::UString &path) const;

	/** Declare a class with the given name. Must be called before the registration of entities. */
	void declareClass(const Common::UString &name);

	/** Begin registration of the entities. */
	void beginRegister();
	/** End registration of the entities. */
	void endRegister();

	/** Begin registration of a namespace. */
	void beginRegisterNamespace(const Common::UString &name);
	/** End registration of the current namespace. */
	void endRegisterNamespace();

	/** Begin registration of a class. */
	void beginRegisterClass(const Common::UString &name, const Common::UString &baseName = "",
	                        lua_CFunction deleter = 0);

	/** End registration of the current class. */
	void endRegisterClass();

	/** Register a constant. */
	void registerConstant(const Common::UString &name, float value);
	/** Register a variable. */
	void registerVariable(const Common::UString &name, lua_CFunction getter, lua_CFunction setter = 0);
	/** Register a function. */
	void registerFunction(const Common::UString &name, lua_CFunction func);

	/** Return the amount of memory in use by Lua (in Kbytes). */
	int getUsedMemoryAmount() const;

	void setLuaInstanceForObject(void *object, const TableRef& luaInstance);
	void unsetLuaInstanceForObject(void *object);
	const TableRef &getLuaInstanceForObject(void *object) const;

	void injectNewIndexMetaEventIntoTable(const TableRef& table);

private:
	typedef std::map<void *, TableRef> ObjectLuaInstanceMap;

	/** The Lua state. */
	lua_State *_luaState;
	/** The current nesting level of the registration process. */
	int _regNestingLevel;
	/** A list of files that the script subsystem ignores. */
	std::set<Common::UString> _ignoredFiles;

	ObjectLuaInstanceMap _objectLuaInstances;

	/** Open and setup a new Lua state. */
	void openLuaState();
	/** Close the current Lua state. */
	void closeLuaState();

	/** Check whether a class with the given name was declared.
	 *  Throw an exception if the check failed.
	 */
	void requireDeclaredClass(const Common::UString &name) const;

	void registerDefaultBindings();
	void executeDefaultCode();

	/** Handler of the Lua panic situations. */
	static int atPanic(lua_State *state);

	/** Lua bindings */
	static int luaGetLua(lua_State *state);
	static int luaPlayFile(lua_State *state);
	static int luaSetGCInterval(lua_State *state);
	static int luaRegisterSubst(lua_State *state);
	static int luaUnregisterSubst(lua_State *state);
	static int luaRegisterHandler(lua_State *state);
};

} // End of namespace Lua

} // End of namespace Aurora

/** Shortcut for accessing the script engine. */
#define LuaScriptMan ::Aurora::Lua::ScriptManager::instance()

#endif // ENGINES_AURORA_LUA_SCRIPTMAN_H
