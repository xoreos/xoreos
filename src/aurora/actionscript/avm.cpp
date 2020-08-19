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

#include <functional>

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/actionscript/avm.h"
#include "src/aurora/actionscript/object.h"
#include "src/aurora/actionscript/function.h"
#include "src/aurora/actionscript/array.h"
#include "src/aurora/actionscript/string.h"
#include "src/aurora/actionscript/movieclip.h"
#include "src/aurora/actionscript/textfield.h"
#include "src/aurora/actionscript/stage.h"

namespace Aurora {

namespace ActionScript {

AVM::AVM() : _handler(0) {
	_registers.resize(256);
	for (size_t i = 0; i < _registers.size(); ++i)
		_registers[i].push(Variable());

	_variables["_global"] = ObjectPtr(new Object());
	_variables["_root"] = ObjectPtr(new Object());
	_variables["Object"] = ObjectPtr(new DummyFunction());
	_variables["Object"].asObject()->setMember("registerClass", new NativeFunction(std::bind(&AVM::registerClass, this, std::placeholders::_1), false, false, false, false));
	_variables["Object"].asObject()->setMember("prototype", ObjectPtr(new Object()));
	_variables["Array"] = ObjectPtr(new DummyFunction());
	_variables["Array"].asObject()->setMember("prototype", ObjectPtr(new Array()));
	_variables["String"] = ObjectPtr(new DummyFunction());
	_variables["String"].asObject()->setMember("prototype", ObjectPtr(new String()));
	_variables["MovieClip"] = ObjectPtr(new DummyFunction());
	_variables["MovieClip"].asObject()->setMember("prototype", ObjectPtr(new MovieClip()));
	_variables["TextField"] = ObjectPtr(new DummyFunction());
	_variables["TextField"].asObject()->setMember("prototype", ObjectPtr(new TextField()));

	_stage = new Stage();
	_variables["Stage"] = ObjectPtr(_stage);


	_variables["_root"].asObject()->setMember("gotoAndPlay", new NativeFunction(std::bind(&AVM::gotoAndPlay, this, std::placeholders::_1), false, false, false, false));

	_variables["flash"] = ObjectPtr(new Object());
	_variables["flash"].asObject()->setMember("external", ObjectPtr(new Object()));
	_variables["flash"].asObject()->getMember("external").asObject()->setMember("ExternalInterface", ObjectPtr(new Object()));
	_variables["flash"].asObject()
		->getMember("external").asObject()->getMember("ExternalInterface").asObject()
		->setMember("call", new NativeFunction(std::bind(&AVM::call, this, std::placeholders::_1), false, false, false, false));

	_functions["setInterval"] = std::bind(&AVM::setInterval, this, std::placeholders::_1);
	_functions["clearInterval"] = std::bind(&AVM::clearInterval, this, std::placeholders::_1);
}

void AVM::setStageSize(unsigned int width, unsigned int height) {
	_stage->setSize(width, height);
}

void AVM::setRegisterClassFunction(RegisterClassFunction registerClass) {
	_registerClass = registerClass;
}

void AVM::setFSCommandCallback(FSCommandFunction fscommand) {
	_fscommand = fscommand;
}

void AVM::setExternalInterface(ExternalHandler *handler) {
	_handler = handler;
}

void AVM::pushRegisters(uint8_t n) {
	for (unsigned int i = 1; i < n; ++i) {
		_registers[i].push(Variable());
	}
}

void AVM::popRegisters(uint8_t n) {
	for (unsigned int i = 1; i < n; ++i) {
		_registers[i].pop();
	}
}

void AVM::storeRegister(Variable value, byte index) {
	_registers[index].top() = value;
}

Variable AVM::getRegister(byte index) {
	return _registers[index].top();
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

Variable AVM::callFunction(const Common::UString &name, const std::vector<Variable> &arguments) {
	std::map<Common::UString, StaticFunction>::const_iterator iter = _functions.find(name);

	if (iter == _functions.end()) {
		warning("Function %s does not exist", name.c_str());
		return Variable();
	}

	return (*iter).second(arguments);
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

	pushRegisters(constructor->getNumRegisters());

	byte counter = 1;
	if (constructor->getPreloadRootFlag()) {
		storeRegister(_variables["_root"], counter);
		counter += 1;
	}
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

	popRegisters(constructor->getNumRegisters());

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

void AVM::startTime() {
	if (!_handler)
		return;

	_startTime = _handler->getTime();
}

uint32_t AVM::getTime() {
	if (!_handler)
		return 0;

	return _handler->getTime() - _startTime;
}

void AVM::setReturnValue(Variable returnValue) {
	_returnValue = returnValue;
}

Variable AVM::getReturnValue() {
	return _returnValue;
}

Variable AVM::call(AVM &avm) {
	Variable name = avm.getRegister(1);
	if (!name.isString())
		throw Common::Exception("AVM::registerClass(): name is not a string");

	info("TODO: ExternalInterface.call(%s,...)", name.asString().c_str());

	return Variable();
}

Variable AVM::registerClass(AVM &avm) {
	Variable name = avm.getRegister(1);
	Variable object = avm.getRegister(2);

	if (!name.isString())
		throw Common::Exception("AVM::registerClass(): name is not a string");
	if (!object.isObject())
		throw Common::Exception("AVM::registerClass(): value is not an object");

	if (_registerClass)
		_registerClass(name.asString(), object.asObject());

	return Variable();
}

Variable AVM::gotoAndPlay(AVM &avm) {
	Variable frame = avm.getRegister(1);

	if (!frame.isNumber() && !frame.isString())
		throw Common::Exception("AVM::gotoAndPlay(): parameter is neither a string nor a number");

	info("TODO: gotoAndPlay(%s)", frame.asString().c_str());

	return Variable();
}

ExternalHandler::~ExternalHandler() {

}
} // End of namespace ActionScript

} // End of namespace Aurora
