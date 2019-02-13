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
 *  Context for executing ActionScript.
 */

#ifndef AURORA_ACTIONSCRIPT_AVM_H
#define AURORA_ACTIONSCRIPT_AVM_H

#include <stack>

#include <boost/function.hpp>

#include "src/common/ustring.h"

#include "src/aurora/actionscript/variable.h"
#include "object.h"

namespace Aurora {

namespace ActionScript {

/** Function for receiving fscommand(). */
typedef boost::function<Variable(const Common::UString &, const Common::UString &)> FSCommandFunction;

/** Function for registering classes for widgets. */
typedef boost::function<void (const Common::UString &, ObjectPtr)> RegisterClassFunction;

/** The Action script virtual machine (AVM). */
class AVM {
public:
	AVM();

	/** Set a callback for the Object.registerClass() function. */
	void setRegisterClassFunction(RegisterClassFunction);
	/** Set a callback for the fscommand() function. */
	void setFSCommandCallback(FSCommandFunction);

	/** fscommand is used for communicating with the host program. It is one of
	 *  Scaleforms ways for sending messages from the actionscript code to the
	 *  c++ program.
	 *
	 *  @param name The name of the fscommand like "PlayMovie".
	 *  @param value The parameters of the fscommand.
	 */
	void fsCommand(const Common::UString &name, const Common::UString &value);

	void pushRegisters(uint8 n);
	void popRegisters(uint8 n);

	void storeRegister(Variable value, byte index);
	Variable getRegister(byte index);

	/** Check if a specific variable exists. */
	bool hasVariable(const Common::UString &name);
	/** Set a specific variable. */
	void setVariable(const Common::UString &name, Variable value);
	Variable getVariable(const Common::UString &name);

	Variable createNewObject(const Common::UString &name,
	                         std::vector<Variable> arguments = std::vector<Variable>());

	void setStopFlag();
	bool getStopFlag();

	void setReturnValue(Variable returnValue = Variable());
	Variable getReturnValue();

private:
	Variable registerClass(AVM &avm);

	RegisterClassFunction _registerClass;
	FSCommandFunction _fscommand;

	std::vector<std::stack<Variable>> _registers;
	std::map<Common::UString, Variable> _variables;

	bool _stopFlag;
	Variable _returnValue;
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_AVM_H
