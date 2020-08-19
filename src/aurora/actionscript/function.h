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
 *  Function objects for ActionScript.
 */

#ifndef AURORA_ACTIONSCRIPT_FUNCTION_H
#define AURORA_ACTIONSCRIPT_FUNCTION_H

#include <vector>
#include <functional>

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/asbuffer.h"

namespace Aurora {

namespace ActionScript {

class Function;

typedef boost::shared_ptr<Function> FunctionPtr;

class Function : public Object {
public:
	Function(
		std::vector<uint8_t> parameterIds,
		uint8_t numRegisters,
		bool preloadThisFlag,
		bool preloadSuperFlag,
		bool preloadRootFlag,
		bool preloadGlobalFlag
	);

	bool hasRegisterIds();
	uint8_t getRegisterId(size_t n);

	uint8_t getNumRegisters();

	bool getPreloadThisFlag();
	bool getPreloadSuperFlag();
	bool getPreloadRootFlag();
	bool getPreloadGlobalFlag();

	virtual Variable operator()(AVM &avm) = 0;

private:
	std::vector<uint8_t> _parameterIds;

	uint8_t _numRegisters;

	bool _preloadThisFlag;
	bool _preloadSuperFlag;
	bool _preloadRootFlag;
	bool _preloadGlobalFlag;
};

class ScriptedFunction : public Function {
public:
	ScriptedFunction(
			Common::SeekableReadStream *as,
			std::vector<Common::UString> constantPool,
			std::vector<uint8_t> parameterIds,
			uint8_t numRegisters,
			bool preloadThisFlag,
			bool preloadSuperFlag,
			bool preloadRootFlag,
			bool preloadGlobalFlag
	);
	~ScriptedFunction();

	Variable operator()(AVM &avm);

private:
	Common::SeekableReadStream *_stream;
	ASBuffer _buffer;
};

class NativeFunction : public Function {
public:
	NativeFunction(std::function<Variable(AVM &)> function, bool preloadThisFlag, bool preloadSuperFlag, bool preloadRootFlag, bool preloadGlobalFlag);

	Variable operator()(AVM &avm);

private:
	std::function<Variable(AVM &)> _function;
};

class DummyFunction : public Function {
public:
	DummyFunction();

	Variable operator()(AVM &UNUSED(avm)){ return Variable(); };
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_FUNCTION_H
