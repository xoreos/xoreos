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

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/actionscript/avm.h"
#include "src/aurora/actionscript/object.h"
#include "src/aurora/actionscript/function.h"
#include "src/aurora/actionscript/array.h"
#include "src/aurora/actionscript/string.h"
#include "src/aurora/actionscript/movieclip.h"
#include "src/aurora/actionscript/textfield.h"

namespace Aurora {

namespace ActionScript {

AVM::AVM() {
	_registers.resize(256);
	_stopFlag = false;

	_variables["_global"] = ObjectPtr(new Object());
	_variables["Object"] = ObjectPtr(new DummyFunction());
	_variables["Object"].asObject()->setMember("prototype", ObjectPtr(new Object()));
	_variables["Array"] = ObjectPtr(new DummyFunction());
	_variables["Array"].asObject()->setMember("prototype", ObjectPtr(new Array()));
	_variables["String"] = ObjectPtr(new DummyFunction());
	_variables["String"].asObject()->setMember("prototype", ObjectPtr(new String()));
	_variables["MovieClip"] = ObjectPtr(new DummyFunction());
	_variables["MovieClip"].asObject()->setMember("prototype", ObjectPtr(new MovieClip()));
	_variables["TextField"] = ObjectPtr(new DummyFunction());
	_variables["TextField"].asObject()->setMember("prototype", ObjectPtr(new TextField()));
}

void AVM::setFSCommandCallback(FSCommandFunction fscommand) {
	_fscommand = fscommand;
}

void AVM::storeRegister(Variable value, byte index) {
	_registers[index] = value;
}

Variable AVM::getRegister(byte index) {
	return _registers[index];
}

void AVM::fsCommand(const Common::UString &name, const Common::UString &value) {
	info("fsCommand(\"%s\", \"%s\")", name.c_str(), value.c_str());
	if (_fscommand)
		_fscommand(name, value);
}

bool AVM::hasVariable(const Common::UString &name) {
	if (_variables.find(name) != _variables.end())
		return true;
	if (_variables["_global"].asObject()->hasMember(name))
		return true;

	return false;
}

void AVM::setVariable(const Common::UString &name, Variable value) {
	_variables[name] = value;
}

Variable AVM::getVariable(const Common::UString &name) {
	if (_variables.find(name) != _variables.end()) {
		return _variables[name];
	} else if (_variables["_global"].asObject()->hasMember(name)) {
		return _variables["_global"].asObject()->getMember(name);
	} else {
		_variables["_global"].asObject()->setMember(name, Variable());
		return _variables["_global"].asObject()->getMember(name);
	}
}

Variable AVM::createNewObject(const Common::UString &name, std::vector<Variable> arguments) {
	Variable variable = getVariable(name);

	if (!variable.isFunction())
		throw Common::Exception("Object is not a function");

	if (!variable.asObject()->hasMember("prototype"))
		throw Common::Exception("Object has no prototype");

	ObjectPtr newObject(new Object(variable.asObject()->getMember("prototype").asObject().get()));
	Function *constructor = dynamic_cast<Function *>(variable.asObject().get());

	if (!constructor)
		throw Common::Exception("Constructor is not a function");

	byte counter = 1;
	if (constructor->getPreloadThisFlag()) {
		storeRegister(Variable(newObject), counter);
		counter += 1;
	}
	if (constructor->getPreloadSuperFlag()) {
		storeRegister(Variable(newObject->getMember("constructor")), counter);
		counter += 1;
	}

	for (size_t i = 0; i < arguments.size(); ++i) {
		storeRegister(arguments[i], counter);
		counter += 1;
	}

	(*constructor)(*this);

	return Variable(newObject);
}

void AVM::setStopFlag() {
	_stopFlag = true;
}

bool AVM::getStopFlag() {
	bool tmp = _stopFlag;
	_stopFlag = false;
	return tmp;
}

void AVM::setReturnValue(Variable returnValue) {
	_returnValue = returnValue;
}

Variable AVM::getReturnValue() {
	return _returnValue;
}

} // End of namespace ActionScript

} // End of namespace Aurora
