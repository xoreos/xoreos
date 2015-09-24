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
 *  Context of an NWScript function.
 */

#ifndef AURORA_NWSCRIPT_FUNCTIONCONTEXT_H
#define AURORA_NWSCRIPT_FUNCTIONCONTEXT_H

#include "src/common/ustring.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/variable.h"

namespace Aurora {

namespace NWScript {

class NCSFile;
class VariableContainer;

class FunctionContext {
public:
	FunctionContext(const Common::UString &name = "");
	FunctionContext(const FunctionContext &ctx);
	~FunctionContext();

	FunctionContext &operator=(const FunctionContext &ctx);

	const Common::UString &getName() const;

	void setSignature(const Signature &signature);
	void setDefaults(const Parameters &defaults);

	size_t getParamMin() const;
	size_t getParamMax() const;

	void setParamsSpecified(size_t params);
	size_t getParamsSpecified() const;

	const Signature &getSignature() const;

	Object *getCaller() const;
	void setCaller(Object *obj);

	Object *getTriggerer() const;
	void setTriggerer(Object *obj);

	Variable &getReturn();
	const Variable &getReturn() const;

	Parameters &getParams();
	const Parameters &getParams() const;

	void setCurrentScript(NCSFile *script = 0);
	NCSFile *getCurrentScript() const;
	VariableContainer *getCurrentEnvironment() const;

	const Common::UString &getScriptName() const;

private:
	Common::UString _name;

	Signature _signature; ///< The function's signature.

	Object *_caller;    ///< The calling object.
	Object *_triggerer; ///< The triggering person.

	Variable   _return;     ///< The function's return value.
	Parameters _parameters; ///< The function's parameters.

	NCSFile *_currentScript; ///< The script executing this function.

	size_t _defaultCount;    ///< The number of default values.
	size_t _paramsSpecified; ///< The number of parameters specified (not defaulted).
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_FUNCTIONCONTEXT_H
