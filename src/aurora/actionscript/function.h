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

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/asbuffer.h"

namespace Aurora {

namespace ActionScript {

class Function;

typedef boost::shared_ptr<Function> FunctionPtr;

class Function : public Object {
public:
	Function(bool preloadThisFlag, bool preloadSuperFlag);

	bool getPreloadThisFlag();
	bool getPreloadSuperFlag();

	virtual Variable operator()(AVM &avm) = 0;

private:
	bool _preloadThisFlag;
	bool _preloadSuperFlag;
};

class ScriptedFunction : public Function {
public:
	ScriptedFunction(
			Common::SeekableReadStream *as,
			std::vector<Common::UString> constantPool,
			bool preloadThisFlag,
			bool preloadSuperFlag
	);
	~ScriptedFunction();

	Variable operator()(AVM &avm);

private:
	Common::SeekableReadStream *_stream;
	ASBuffer _buffer;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_FUNCTION_H
