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

FunctionManager::FunctionManager() {
}

FunctionManager::~FunctionManager() {
}

void FunctionManager::clear() {
	_functions.clear();
}

void FunctionManager::registerFunction(const Common::UString &name,
                                       const Function &func, const Signature &signature) {

	std::pair<FunctionMap::iterator, bool> result;

	result = _functions.insert(std::make_pair(name, FunctionEntry()));
	if (!result.second)
		throw Common::Exception("Failed to register NWScript function \"%s\"", name.c_str());

	FunctionEntry &f = result.first->second;

	f.func = func;
	f.ctx.setSignature(signature);
}

FunctionContext FunctionManager::createContext(const Common::UString &function) const {
	FunctionMap::const_iterator f = _functions.find(function);
	if (f == _functions.end())
		throw Common::Exception("No such NWScript function \"%s\"", function.c_str());

	return f->second.ctx;
}

void FunctionManager::call(const Common::UString &function, FunctionContext &ctx) const {
	FunctionMap::const_iterator f = _functions.find(function);
	if (f == _functions.end())
		throw Common::Exception("No such NWScript function \"%s\"", function.c_str());

	f->second.func(ctx);
}

} // End of namespace NWScript

} // End of namespace Aurora
