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
 *  The NWScript function manager.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/debug.h"

#include "src/aurora/nwscript/functionman.h"

DECLARE_SINGLETON(Aurora::NWScript::FunctionManager)

namespace Aurora {

namespace NWScript {

FunctionManager::FunctionEntry::FunctionEntry(const Common::UString &name) :
	empty(true), ctx(name) {
}


FunctionManager::FunctionManager() {
}

FunctionManager::~FunctionManager() {
}

void FunctionManager::clear() {
	_functionMap.clear();
	_functionArray.clear();
}

void FunctionManager::registerFunction(const Common::UString &name, uint32_t id,
                                       const Function &func, const Signature &signature) {
	Parameters defaults;

	registerFunction(name, id, func, signature, defaults);
}

void FunctionManager::registerFunction(const Common::UString &name, uint32_t id,
                                       const Function &func, const Signature &signature,
                                       const Parameters &defaults) {

	std::pair<FunctionMap::iterator, bool> result;

	result = _functionMap.insert(std::make_pair(name, FunctionEntry(name)));
	if (!result.second)
		throw Common::Exception("Failed to register NWScript function \"%s\"", name.c_str());

	FunctionEntry &f = result.first->second;

	f.func = func;
	f.ctx.setSignature(signature);
	f.ctx.setDefaults(defaults);
	f.empty = false;

	if (_functionArray.size() <= id)
		_functionArray.resize(id + 1);

	_functionArray[id] = f;
}

FunctionContext FunctionManager::createContext(const Common::UString &function) const {
	return find(function).ctx;
}

void FunctionManager::call(const Common::UString &function, FunctionContext &ctx) const {
	debugCN(Common::kDebugEngineScripts, 5, "%s %s(%s)", formatType(ctx.getReturn().getType()).c_str(),
	        ctx.getName().c_str(), formatParams(ctx).c_str());

	find(function).func(ctx);

	const Common::UString r = formatReturn(ctx);
	debugC(Common::kDebugEngineScripts, 5, "%s%s", r.empty() ? "" : " => ", r.c_str());

	if (DebugMan.getVerbosityLevel(Common::kDebugEngineScripts) < 5)
		debugC(Common::kDebugEngineScripts, 2, "%s %s(%s)%s%s", formatType(ctx.getReturn().getType()).c_str(),
		       ctx.getName().c_str(), formatParams(ctx).c_str(), r.empty() ? "" : " => ", r.c_str());
}

FunctionContext FunctionManager::createContext(uint32_t function) const {
	return find(function).ctx;
}

void FunctionManager::call(uint32_t function, FunctionContext &ctx) const {
	debugCN(Common::kDebugEngineScripts, 5, "%s %s(%s)", formatType(ctx.getReturn().getType()).c_str(),
	        ctx.getName().c_str(), formatParams(ctx).c_str());

	find(function).func(ctx);

	const Common::UString r = formatReturn(ctx);
	debugC(Common::kDebugEngineScripts, 5, "%s%s", r.empty() ? "" : " => ", r.c_str());

	if (DebugMan.getVerbosityLevel(Common::kDebugEngineScripts) < 5)
		debugC(Common::kDebugEngineScripts, 2, "%s %s(%s)%s%s", formatType(ctx.getReturn().getType()).c_str(),
		       ctx.getName().c_str(), formatParams(ctx).c_str(), r.empty() ? "" : " => ", r.c_str());
}

const FunctionManager::FunctionEntry &FunctionManager::find(const Common::UString &function) const {
	FunctionMap::const_iterator f = _functionMap.find(function);
	if ((f == _functionMap.end()) || f->second.empty)
		throw Common::Exception("No such NWScript function \"%s\"", function.c_str());

	return f->second;
}

const FunctionManager::FunctionEntry &FunctionManager::find(uint32_t function) const {
	if ((function >= _functionArray.size()) || _functionArray[function].empty)
		throw Common::Exception("No such NWScript function %d", function);

	return _functionArray[function];
}

} // End of namespace NWScript

} // End of namespace Aurora
