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
 *  Buffer for handling actionscript byte code.
 */

#ifndef AURORA_ACTIONSCRIPT_ASBUFFER_H
#define AURORA_ACTIONSCRIPT_ASBUFFER_H

#include <stack>
#include <memory>

#include <boost/any.hpp>

#include "src/common/readstream.h"

#include "src/aurora/actionscript/avm.h"
#include "src/aurora/actionscript/object.h"

namespace Aurora {

namespace ActionScript {

class Variable;

class ASBuffer {
public:
	ASBuffer(Common::SeekableReadStream *as);

	void run(AVM &avm);

	void setConstantPool(std::vector<Common::UString> constantPool);

private:
	void execute(AVM &avm);

	void actionStop(AVM &avm);
	void actionToggleQuality();
	void actionSubtract();
	void actionMultiply();
	void actionDivide();
	void actionAnd();
	void actionOr();
	void actionNot();
	void actionPop();
	void actionGetVariable(AVM &avm);
	void actionSetVariable(AVM &avm);
	void actionTrace();
	void actionGetTime(AVM &avm);
	void actionDefineLocal(AVM &avm);
	void actionCallFunction(AVM &avm);
	void actionReturn(AVM &avm);
	void actionNewObject(AVM &avm);
	void actionInitArray();
	void actionAdd2();
	void actionLess2();
	void actionEquals2();
	void actionToNumber2();
	void actionPushDuplicate();
	void actionGetMember();
	void actionSetMember();
	void actionIncrement();
	void actionCallMethod(AVM &avm);
	void actionEnumerate2();
	void actionGreater();
	void actionExtends();
	void actionGetURL(AVM &avm);
	void actionStoreRegister(AVM &avm);
	void actionConstantPool();
	void actionDefineFunction2();
	void actionPush(AVM &avm);
	void actionJump();
	void actionGetURL2(AVM &avm);
	void actionDefineFunction();
	void actionIf();

	// Utility methods and variables
	Common::UString readString();

	size_t _currentLength;
	ptrdiff_t _seeked;

	// Constant pool
	std::vector<Common::UString> _constants;

	// Execution stack
	std::stack<Variable> _stack;

	// The script data
	Common::SeekableReadStream *_script;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_ASBUFFER_H
