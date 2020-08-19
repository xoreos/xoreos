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

#ifndef AURORA_NWSCRIPT_FUNCTIONMAN_H
#define AURORA_NWSCRIPT_FUNCTIONMAN_H

#include <vector>
#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/functioncontext.h"

namespace Aurora {

namespace NWScript {

class FunctionManager : public Common::Singleton<FunctionManager> {
public:

	FunctionManager();
	~FunctionManager();

	void clear();

	void registerFunction(const Common::UString &name, uint32_t id, const Function &func,
	                      const Signature &signature);
	void registerFunction(const Common::UString &name, uint32_t id, const Function &func,
	                      const Signature &signature, const Parameters &defaults);

	FunctionContext createContext(const Common::UString &function) const;
	void call(const Common::UString &function, FunctionContext &ctx) const;

	FunctionContext createContext(uint32_t function) const;
	void call(uint32_t function, FunctionContext &ctx) const;

private:
	struct FunctionEntry {
		bool empty;

		Function func;
		FunctionContext ctx;

		FunctionEntry(const Common::UString &name = "");
	};

	typedef std::map<Common::UString, FunctionEntry> FunctionMap;
	typedef std::vector<FunctionEntry> FunctionArray;

	FunctionMap _functionMap;
	FunctionArray _functionArray;

	const FunctionEntry &find(const Common::UString &function) const;
	const FunctionEntry &find(uint32_t function) const;
};

} // End of namespace NWScript

} // End of namespace Aurora

#define FunctionMan Aurora::NWScript::FunctionManager::instance()

#endif // AURORA_NWSCRIPT_FUNCTIONMAN_H
