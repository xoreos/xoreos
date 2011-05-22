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

/** @file aurora/nwscript/functionman.h
 *  The NWScript function manager.
 */

#ifndef AURORA_NWSCRIPT_FUNCTIONMAN_H
#define AURORA_NWSCRIPT_FUNCTIONMAN_H

#include <map>

#include "common/ustring.h"
#include "common/singleton.h"

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/functioncontext.h"

namespace Aurora {

namespace NWScript {

class FunctionManager : public Common::Singleton<FunctionManager> {
public:

	FunctionManager();
	~FunctionManager();

	void clear();

	void registerFunction(const Common::UString &name, const Function &func,
	                      const Signature &signature);

	FunctionContext createContext(const Common::UString &function) const;
	void call(const Common::UString &function, FunctionContext &ctx) const;

private:
	struct FunctionEntry {
		Function func;
		FunctionContext ctx;
	};

	typedef std::map<Common::UString, FunctionEntry> FunctionMap;

	FunctionMap _functions;
};

} // End of namespace NWScript

} // End of namespace Aurora

#define FunctionMan Aurora::NWScript::FunctionManager::instance()

#endif // AURORA_NWSCRIPT_FUNCTIONMAN_H
