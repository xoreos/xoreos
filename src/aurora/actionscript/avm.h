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

#include <functional>

#include "src/common/ustring.h"

#include "src/aurora/actionscript/variable.h"
#include "object.h"

namespace Aurora {

namespace ActionScript {

class Stage;

/** Function for receiving fscommand(). */
typedef std::function<Variable(const Common::UString &, const Common::UString &)> FSCommandFunction;

/** Function for registering classes for widgets. */
typedef std::function<void (const Common::UString &, ObjectPtr)> RegisterClassFunction;

/** Prototype for ActionScript static functions. */
typedef std::function<Variable(std::vector<Variable>)> StaticFunction;

/** Function type for ActionScript interval functions. */
typedef std::function<void()> IntervalFunction;

/** An abstract base class for handling external things. */
class ExternalHandler {
public:
	virtual ~ExternalHandler();

	/** Set a specific function to be called in regular intevals
	 *  specified in milliseconds.
	 *
	 *  @param interval The interval in milliseconds between the call of the function
	 *  @param fun The function that should be called in
	 *  @return The id of the interval to have the ability to later stop it
	 */
	virtual uint32_t setInterval(double interval, IntervalFunction fun) = 0;

	/** Stop a running interval function.
	 *
	 *  @param id the id of the function to stop.
	 */
	virtual void clearInterval(uint32_t id) = 0;

	/** Get the elapsed time after the program start.
	 *
	 *  @return the elapsed time since the program started.
	 */
	virtual uint32_t getTime() = 0;
};

/** The Action script virtual machine (AVM). */
class AVM {
public:
	AVM();

	/** Set the stage size. */
	void setStageSize(unsigned int width, unsigned int height);

	/** Set a callback for the Object.registerClass() function. */
	void setRegisterClassFunction(RegisterClassFunction);
	/** Set a callback for the fscommand() function. */
	void setFSCommandCallback(FSCommandFunction);

	/** Set the ExternalHandler for this AVM. */
	void setExternalInterface(ExternalHandler *handler);

	/** fscommand is used for communicating with the host program. It is one of
	 *  Scaleforms ways for sending messages from the actionscript code to the
	 *  c++ program.
	 *
	 *  @param name The name of the fscommand like "PlayMovie".
	 *  @param value The parameters of the fscommand.
	 */
	void fsCommand(const Common::UString &name, const Common::UString &value);

	void pushRegisters(uint8_t n);
	void popRegisters(uint8_t n);

	void storeRegister(Variable value, byte index);
	Variable getRegister(byte index);

	/** Check if a specific variable exists. */
	bool hasVariable(const Common::UString &name);
	/** Set a specific variable. */
	void setVariable(const Common::UString &name, Variable value);
	Variable getVariable(const Common::UString &name);

	/** Call a specific static function. */
	Variable callFunction(const Common::UString &name, const std::vector<Variable> &arguments);

	Variable createNewObject(const Common::UString &name,
	                         std::vector<Variable> arguments = std::vector<Variable>());

	void setStopFlag();
	bool getStopFlag();

	/** Start the timer. */
	void startTime();
	/** Get the current running AVM time. */
	uint32_t getTime();

	void setReturnValue(Variable returnValue = Variable());
	Variable getReturnValue();

private:
	Variable call(AVM &avm);
	Variable registerClass(AVM &avm);
	Variable gotoAndPlay(AVM &avm);

	Variable setInterval(const std::vector<Variable> arguments);
	Variable clearInterval(const std::vector<Variable> arguments);

	RegisterClassFunction _registerClass;

	FSCommandFunction _fscommand;

	ExternalHandler *_handler; ///< A wrapper around common platform functionalities.

	std::vector<std::stack<Variable>> _registers;
	std::map<Common::UString, Variable> _variables;
	std::map<Common::UString, StaticFunction> _functions;

	uint32_t _startTime { 0 }; ///< The elapsed since the start of the avm and the call to startTime()
	bool _stopFlag { false };
	Variable _returnValue;
	Stage *_stage { 0 };
};

} // End of namespace ActionScript

} // End of namespace Aurora

#endif // AURORA_ACTIONSCRIPT_AVM_H
