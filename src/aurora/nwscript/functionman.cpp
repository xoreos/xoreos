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

/** @file aurora/nwscript/functionman.cpp
 *  The NWScript function manager.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/nwscript/functionman.h"

DECLARE_SINGLETON(Aurora::NWScript::FunctionManager)

namespace Aurora {

namespace NWScript {

FunctionManager::FunctionEntry::FunctionEntry() : empty(true) {
}


FunctionManager::FunctionManager() {
}

FunctionManager::~FunctionManager() {
}

void FunctionManager::clear() {
	_functionMap.clear();
	_functionArray.clear();
}

void FunctionManager::registerFunction(const Common::UString &name, uint32 id,
                                       const Function &func, const Signature &signature) {

	std::pair<FunctionMap::iterator, bool> result;

	result = _functionMap.insert(std::make_pair(name, FunctionEntry()));
	if (!result.second)
		throw Common::Exception("Failed to register NWScript function \"%s\"", name.c_str());

	FunctionEntry &f = result.first->second;

	f.func = func;
	f.ctx.setSignature(signature);
	f.empty = false;

	if (_functionArray.size() <= id)
		_functionArray.resize(id + 1);

	_functionArray[id] = f;
}

FunctionContext FunctionManager::createContext(const Common::UString &function) const {
	return find(function).ctx;
}

void FunctionManager::call(const Common::UString &function, FunctionContext &ctx) const {
	find(function).func(ctx);
}

FunctionContext FunctionManager::createContext(uint32 function) const {
	return find(function).ctx;
}

void FunctionManager::call(uint32 function, FunctionContext &ctx) const {
	find(function).func(ctx);
}

const FunctionManager::FunctionEntry &FunctionManager::find(const Common::UString &function) const {
	FunctionMap::const_iterator f = _functionMap.find(function);
	if ((f == _functionMap.end()) || f->second.empty)
		throw Common::Exception("No such NWScript function \"%s\"", function.c_str());

	return f->second;
}

const FunctionManager::FunctionEntry &FunctionManager::find(uint32 function) const {
	if ((function >= _functionArray.size()) || _functionArray[function].empty)
		throw Common::Exception("No such NWScript function %d", function);

	return _functionArray[function];
}

} // End of namespace NWScript

} // End of namespace Aurora
