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

#include <cassert>

#include "src/common/bitstream.h"
#include "src/common/memreadstream.h"
#include "src/common/debug.h"

#include "src/aurora/actionscript/variable.h"
#include "src/aurora/actionscript/asbuffer.h"
#include "src/aurora/actionscript/function.h"
#include "src/aurora/actionscript/array.h"

using Common::kDebugActionScript;

namespace Aurora {

namespace ActionScript {

enum Opcodes {
	kActionNextFrame       = 0x04,
	kActionPreviousFrame   = 0x05,
	kActionStop            = 0x07,
	kActionToggleQuality   = 0x08,
	kActionStopSounds      = 0x09,
	kActionSubtract        = 0x0B,
	kActionMultiply        = 0x0C,
	kActionDivide          = 0x0D,
	kActionEquals          = 0x0E,
	kActionAnd             = 0x10,
	kActionOr              = 0x11,
	kActionNot             = 0x12,
	kActionStringEquals    = 0x13,
	kActionPop             = 0x17,
	kActionGetVariable     = 0x1C,
	kActionSetVariable     = 0x1D,
	kActionTrace           = 0x26,
	kActionGetTime         = 0x34,
	kActionDefineLocal     = 0x3C,
	kActionCallFunction    = 0x3D,
	kActionReturn          = 0x3E,
	kActionNewObject       = 0x40,
	kActionInitArray       = 0x42,
	kActionAdd2            = 0x47,
	kActionLess2           = 0x48,
	kActionEquals2         = 0x49,
	kActionToNumber2       = 0x4A,
	kActionPushDuplicate   = 0x4C,
	kActionGetMember       = 0x4E,
	kActionSetMember       = 0x4F,
	kActionIncrement       = 0x50,
	kActionCallMethod      = 0x52,
	kActionEnumerate2      = 0x55,
	kActionStrictEquals    = 0x66,
	kActionGreater         = 0x67,
	kActionExtends         = 0x69,
	kActionGetURL          = 0x83,
	kActionStoreRegister   = 0x87,
	kActionConstantPool    = 0x88,
	kActionDefineFunction2 = 0x8E,
	kActionPush            = 0x96,
	kActionJump            = 0x99,
	kActionGetURL2         = 0x9A,
	kActionDefineFunction  = 0x9B,
	kActionIf              = 0x9D
};

ASBuffer::ASBuffer(Common::SeekableReadStream *as) :
		_currentLength(0), _seeked(0), _script(as) {

	assert(as);
}

void ASBuffer::run(AVM &avm) {
	_script->seek(0);
	execute(avm);
}

void ASBuffer::setConstantPool(std::vector<Common::UString> constantPool) {
	_constants = constantPool;
}

void ASBuffer::execute(AVM &avm) {
	byte opcode;
	debugC(kDebugActionScript, 1, "--- Start Actionscript ---");
	do {
		_seeked = 0;
		opcode = _script->readByte();

		size_t length = 0;
		if (opcode >= 0x80)
			length = _script->readUint16LE();

		_currentLength = length;

		size_t startPos = _script->pos();

		switch (opcode) {
			case kActionStop:            actionStop(avm); break;
			case kActionToggleQuality:   actionToggleQuality(); break;
			case kActionSubtract:        actionSubtract(); break;
			case kActionMultiply:        actionMultiply(); break;
			case kActionDivide:          actionDivide(); break;
			case kActionAnd:             actionAnd(); break;
			case kActionOr:              actionOr(); break;
			case kActionNot:             actionNot(); break;
			case kActionPop:             actionPop(); break;
			case kActionGetVariable:     actionGetVariable(avm); break;
			case kActionSetVariable:     actionSetVariable(avm); break;
			case kActionTrace:           actionTrace(); break;
			case kActionGetTime:         actionGetTime(avm); break;
			case kActionDefineLocal:     actionDefineLocal(avm); break;
			case kActionCallFunction:    actionCallFunction(avm); break;
			case kActionReturn:          actionReturn(avm); break;
			case kActionNewObject:       actionNewObject(avm); break;
			case kActionInitArray:       actionInitArray(); break;
			case kActionAdd2:            actionAdd2(); break;
			case kActionLess2:           actionLess2(); break;
			case kActionEquals2:         actionEquals2(); break;
			case kActionPushDuplicate:   actionPushDuplicate(); break;
			case kActionToNumber2:       actionToNumber2(); break;
			case kActionGetMember:       actionGetMember(); break;
			case kActionSetMember:       actionSetMember(); break;
			case kActionIncrement:       actionIncrement(); break;
			case kActionCallMethod:      actionCallMethod(avm); break;
			case kActionEnumerate2:      actionEnumerate2(); break;
			case kActionGreater:         actionGreater(); break;
			case kActionExtends:         actionExtends(); break;
			case kActionGetURL:          actionGetURL(avm); break;
			case kActionStoreRegister:   actionStoreRegister(avm); break;
			case kActionDefineFunction2: actionDefineFunction2(); break;
			case kActionConstantPool:    actionConstantPool(); break;
			case kActionPush:            actionPush(avm); break;
			case kActionJump:            actionJump(); break;
			case kActionGetURL2:         actionGetURL2(avm); break;
			case kActionDefineFunction:  actionDefineFunction(); break;
			case kActionIf:              actionIf(); break;
			default:
				_script->seek(length, Common::SeekableReadStream::kOriginCurrent);
				if (opcode != 0)
					warning("Unknown opcode");
		}

		if (_script->pos() - startPos != static_cast<size_t>(length + _seeked))
			throw Common::Exception("Invalid tag");
	} while (opcode != 0 && _script->pos() != _script->size() && avm.getReturnValue().isUndefined());

	debugC(kDebugActionScript, 1, "--- End Actionscript ---");
}

void ASBuffer::actionStop(AVM &avm) {
	avm.setStopFlag();

	debugC(kDebugActionScript, 1, "actionStop");
}

void ASBuffer::actionToggleQuality() {
	debugC(kDebugActionScript, 1, "actionToggleQuality");
}

void ASBuffer::actionSubtract() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(b - a);

	debugC(kDebugActionScript, 1, "actionSubtract");
}

void ASBuffer::actionMultiply() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(a * b);

	debugC(kDebugActionScript, 1, "actionMultiply");
}

void ASBuffer::actionDivide() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(b / a);

	debugC(kDebugActionScript, 1, "actionDivide");
}

void ASBuffer::actionAnd() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(a && b);

	debugC(kDebugActionScript, 1, "actionAnd");
}

void ASBuffer::actionOr() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(a || b);

	debugC(kDebugActionScript, 1, "actionOr");
}

void ASBuffer::actionNot() {
	Variable variable = _stack.top();
	_stack.pop();

	_stack.push(!variable);

	debugC(kDebugActionScript, 1, "actionNot");
}

void ASBuffer::actionPop() {
	if (!_stack.empty())
		_stack.pop();

	debugC(kDebugActionScript, 1, "actionPop");
}

void ASBuffer::actionGetVariable(AVM &avm) {
	Common::UString name = _stack.top().asString();
	_stack.pop();

	std::vector<Common::UString> split;
	Common::UString::split(name, '.', split);

	Variable v = avm.getVariable(split[0]);
	for (size_t i = 1; i < split.size(); ++i) {
		v = v.asObject()->getMember(split[i]);
	}
	_stack.push(v);

	debugC(kDebugActionScript, 1, "actionGetVariable");
}

void ASBuffer::actionSetVariable(AVM &avm) {
	Variable value = _stack.top();
	_stack.pop();
	Common::UString name = _stack.top().asString();
	_stack.pop();

	avm.setVariable(name, value);

	debugC(kDebugActionScript, 1, "actionSetVariable");
}

void ASBuffer::actionTrace() {
	warning("ActionScript: %s", "Trace");

	debugC(kDebugActionScript, 1, "actionTrace");
}

void ASBuffer::actionGetTime(AVM &avm) {
	_stack.push(avm.getTime());

	debugC(kDebugActionScript, 1, "actionGetTime");
}

void ASBuffer::actionDefineLocal(AVM &avm) {
	const Variable value = _stack.top();
	_stack.pop();
	const Variable name = _stack.top();
	_stack.pop();

	if (!name.isString())
		throw Common::Exception("actionDefineLocal: name is not a string!");

	// TODO: There is probably a better way, then defining local variables as global persistent variables.
	avm.setVariable(name.asString(), value);

	debugC(kDebugActionScript, 1, "actionDefineLocal");
}

void ASBuffer::actionCallFunction(AVM &avm) {
	const Variable name = _stack.top();
	_stack.pop();
	const Variable numArgs = _stack.top();
	_stack.pop();

	std::vector<Variable> arguments;
	arguments.resize(numArgs.asNumber());
	for (int i = 0; i < numArgs.asNumber(); ++i) {
		arguments[i] = _stack.top();
		_stack.pop();
	}

	if (!name.isString())
		throw Common::Exception("actionCallFunction: name is not a string!");

	if (!numArgs.isNumber())
		throw Common::Exception("actionCallFunction: numArgs is not a numper!");

	_stack.push(avm.callFunction(name.asString(), arguments));

	debugC(kDebugActionScript, 1, "actionCallFunction");
}

void ASBuffer::actionReturn(AVM &avm) {
	Variable ret = _stack.top();
	_stack.pop();

	avm.setReturnValue(ret);

	debugC(kDebugActionScript, 1, "actionReturn");
}

void ASBuffer::actionInitArray() {
	const Variable nArgs = _stack.top();
	_stack.pop();

	if (!nArgs.isNumber())
		throw Common::Exception("Value is not a number");

	std::list<Variable> values;
	for (int i = 0; i < nArgs.asNumber(); ++i) {
		values.push_back(_stack.top());
		_stack.pop();
	}

	_stack.push(ObjectPtr(new Array(values)));

	debugC(kDebugActionScript, 1, "actionInitArray");
}

void ASBuffer::actionAdd2() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(b + a);

	debugC(kDebugActionScript, 1, "actionAdd2");
}

void ASBuffer::actionNewObject(AVM &avm) {
	Common::UString name = _stack.top().asString();
	_stack.pop();
	double numArgs = _stack.top().asNumber();
	_stack.pop();

	std::vector<Variable> arguments;
	for (int i = 0; i < numArgs; ++i) {
		arguments.push_back(_stack.top());
		_stack.pop();
	}

	_stack.push(avm.createNewObject(name, arguments));

	debugC(kDebugActionScript, 1, "actionNewObject");
}

void ASBuffer::actionLess2() {
	Variable a, b;
	a = _stack.top();
	_stack.pop();
	b = _stack.top();
	_stack.pop();

	_stack.push(b < a);

	debugC(kDebugActionScript, 1, "actionLess2");
}

void ASBuffer::actionEquals2() {
	Variable a = _stack.top();
	_stack.pop();
	Variable b = _stack.top();
	_stack.pop();

	_stack.push(a == b);

	debugC(kDebugActionScript, 1, "actionEquals2");
}

void ASBuffer::actionToNumber2() {
	// TODO

	debugC(kDebugActionScript, 1, "actionToNumber2");
}

void ASBuffer::actionPushDuplicate() {
	_stack.push(_stack.top());

	debugC(kDebugActionScript, 1, "actionPushDuplicate");
}

void ASBuffer::actionGetMember() {
	if (!_stack.top().isString())
		throw Common::Exception("value is not a string");
	Common::UString name = _stack.top().asString();
	_stack.pop();
	if (!_stack.top().isObject())
		throw Common::Exception("value is not an object");
	ObjectPtr object = _stack.top().asObject();
	_stack.pop();

	std::vector<Common::UString> split;
	Common::UString::split(name, '.', split);

	Variable v = object->getMember(split[0]);
	for (size_t i = 1; i < split.size(); ++i) {
		v = v.asObject()->getMember(split[i]);
	}

	_stack.push(v);

	debugC(kDebugActionScript, 1, "actionGetMember");
}

void ASBuffer::actionSetMember() {
	Variable value = _stack.top();
	_stack.pop();
	Common::UString name = _stack.top().asString();
	_stack.pop();
	ObjectPtr object = _stack.top().asObject();
	object->setMember(name, value);
	_stack.pop();

	debugC(kDebugActionScript, 1, "actionSetMember");
}

void ASBuffer::actionIncrement() {
	Variable v = _stack.top();
	_stack.pop();

	_stack.push(++v);

	debugC(kDebugActionScript, 1, "actionIncrement");
}

void ASBuffer::actionCallMethod(AVM &avm) {
	Common::UString name;
	if (_stack.top().isString())
		name = _stack.top().asString();
	_stack.pop();

	ObjectPtr object = _stack.top().asObject();
	_stack.pop();

	unsigned int numArgs = _stack.top().asNumber();
	_stack.pop();

	std::vector<Variable> arguments;
	for (unsigned int i = 0; i < numArgs; ++i) {
		arguments.push_back(_stack.top());
		_stack.pop();
	}

	Function *function;
	bool superFunction = false;
	if (name.empty()) {
		function = dynamic_cast<Function *>(object.get());
	} else {
		function = dynamic_cast<Function *>(object->getMember(name).asObject().get());
		if (!function) {
			function = dynamic_cast<Function *>(object->getMember("prototype").asObject()->getMember(name).asObject().get());

			if (function)
				superFunction = true;
		}
	}

	if (!function)
		throw Common::Exception("Object is not a function");

	byte counter = 1;

	// Since this is the first variable to initialize, it can only be at register 1
	Variable currentThis = avm.getRegister(1);

	avm.pushRegisters(function->getNumRegisters());

	if (function->getPreloadThisFlag()) {
		if (name.empty() || superFunction)
			avm.storeRegister(currentThis, counter);
		else
			avm.storeRegister(object, counter);
		counter += 1;
	}
	if (function->getPreloadRootFlag()) {
		avm.storeRegister(avm.getVariable("_root"), counter);
		counter += 1;
	}
	if (function->getPreloadSuperFlag()) {
		avm.storeRegister(object->getMember("prototype").asObject()->getMember("constructor"), counter);
		counter += 1;
	}
	if (function->getPreloadGlobalFlag()) {
		avm.storeRegister(avm.getVariable("_global"), counter);
		counter += 1;
	}

	if (function->hasRegisterIds()) {
		for (size_t i = 0; i < arguments.size(); ++i) {
			avm.storeRegister(arguments[i], function->getRegisterId(i));
		}
	} else {
		for (size_t i = 0; i < arguments.size(); ++i) {
			avm.storeRegister(arguments[i], counter);
			counter += 1;
		}
	}

	avm.setReturnValue(Variable());
	_stack.push((*function)(avm));
	avm.setReturnValue(Variable());

	if (!currentThis.isUndefined())
		avm.storeRegister(currentThis, 1);

	avm.popRegisters(function->getNumRegisters());

	debugC(kDebugActionScript, 1, "actionCallMethod");
}

void ASBuffer::actionEnumerate2() {
	if (!_stack.top().isObject())
		throw Common::Exception("actionEnumerate2: not an object");

	const ObjectPtr object = _stack.top().asObject();
	_stack.pop();

	// Push null as the end of the enumeration.
	_stack.push(Variable::Null());

	// Go through every slot and push the name to the stack.
	std::vector<Common::UString> slotNames = object->getSlots();
	for (size_t i = 0; i < slotNames.size(); ++i) {
		_stack.push(slotNames[i]);
	}

	debugC(kDebugActionScript, 1, "actionEnumerate2");
}

void ASBuffer::actionGreater() {
	const Variable a = _stack.top();
	_stack.pop();
	const Variable b = _stack.top();
	_stack.pop();

	_stack.push(a < b);

	debugC(kDebugActionScript, 1, "actionGreater");
}

void ASBuffer::actionExtends() {
	ObjectPtr superClass = _stack.top().asObject();
	_stack.pop();
	ObjectPtr subClass = _stack.top().asObject();
	_stack.pop();

	ObjectPtr scriptObject(new Object(superClass->getMember("prototype").asObject().get()));
	scriptObject->setMember("constructor", superClass);
	scriptObject->setMember("proto", superClass->getMember("prototype").asObject());

	subClass->setMember("prototype", scriptObject);

	debugC(kDebugActionScript, 1, "actionExtends");
}

void ASBuffer::actionGetURL(AVM &avm) {
	const Variable url = _stack.top();
	if (!url.isString())
		throw Common::Exception("actionGetURL: url is not a string");
	_stack.pop();
	const Variable target = _stack.top();
	if (!target.isString())
		throw Common::Exception("actionGetURL: target is not a string");
	_stack.pop();

	const Common::UString urlString = url.asString();
	const Common::UString targetString = target.asString();

	avm.fsCommand(urlString, targetString);

	debugC(kDebugActionScript, 1, "actionGetURL \"%s\" \"%s\"", urlString.c_str(), targetString.c_str());
}

void ASBuffer::actionStoreRegister(AVM &avm) {
	byte registerNumber = _script->readByte();
	avm.storeRegister(_stack.top(), registerNumber);

	debugC(kDebugActionScript, 1, "actionStoreRegister %i", registerNumber);
}

void ASBuffer::actionConstantPool() {
	uint16_t count = _script->readUint16LE();
	_constants.resize(count);
	for (int i = 0; i < count; ++i) {
		Common::UString constant = readString();
		_constants[i] = constant;
	}

	debugC(kDebugActionScript, 1, "actionConstantPool");
}

void ASBuffer::actionDefineFunction2() {
	Common::UString functionName = readString();
	int numParams = _script->readUint16LE();
	int registerCount = _script->readByte();

	Common::BitStream8MSB bitstream(_script);

	bool preloadParentFlag = bitstream.getBit() != 0;
	bool preloadRootFlag = bitstream.getBit() != 0;
	bool suppressSuperFlag = bitstream.getBit() != 0;
	bool preloadSuperFlag = bitstream.getBit() != 0;
	bool suppressArgumentsFlag = bitstream.getBit() != 0;
	bool preloadArgumentsFlag = bitstream.getBit() != 0;
	bool suppressThisFlag = bitstream.getBit() != 0;
	bool preloadThisFlag = bitstream.getBit() != 0;

	unsigned int reserved = bitstream.getBits(7);
	assert(reserved == 0);

	bool preloadGlobalFlag = bitstream.getBit() != 0;

	std::vector<uint8_t> parameterIds(numParams);
	for (int i = 0; i < numParams; ++i) {
		parameterIds[i] = _script->readByte();
		readString();
	}

	unsigned short codeSize = _script->readUint16LE();
	_seeked = codeSize;

	_stack.push(
			ObjectPtr(
					new ScriptedFunction(
							_script->readStream(codeSize),
							_constants,
							parameterIds,
							registerCount,
							preloadThisFlag,
							preloadSuperFlag,
							preloadRootFlag,
							preloadGlobalFlag
					)
			)
	);

	debugC(
			kDebugActionScript,
			1,
			"actionDefineFunction2 \"%s\" %d %d %s %s %s %s %s %s %s %s %s",
			functionName.c_str(),
			numParams,
			registerCount,
			preloadParentFlag ? "true" : "false",
			preloadRootFlag ? "true" : "false",
			suppressSuperFlag ? "true" : "false",
			preloadSuperFlag ? "true" : "false",
			suppressArgumentsFlag ? "true" : "false",
			preloadArgumentsFlag ? "true" : "false",
			suppressThisFlag ? "true" : "false",
			preloadThisFlag ? "true" : "false",
			preloadGlobalFlag ? "true" : "false"
	);
}

void ASBuffer::actionPush(AVM &avm) {
	unsigned short length = _currentLength;

	while (length != 0) {
		byte type = _script->readByte();
		length -= 1;

		switch (type) {
			case 0: {
				Common::UString string = readString();
				_stack.push(string);
				debugC(kDebugActionScript, 1, "actionPush \"%s\"", string.c_str());
				length -= string.size() + 1;
				break;
			}
			case 1: {
				float floatValue = _script->readIEEEFloatLE();
				_stack.push(floatValue);
				debugC(kDebugActionScript, 1, "actionPush %f", floatValue);
				length -= 4;
				break;
			}
			case 2: {
				// null
				debugC(kDebugActionScript, 1, "actionPush null");
				_stack.push(Variable::Null());
				break;
			}
			case 3: {
				// undefined
				debugC(kDebugActionScript, 1, "actionPush undefined");
				_stack.push(Variable());
				break;
			}
			case 4: {
				byte registerNumber = _script->readByte();
				debugC(kDebugActionScript, 1, "actionPush register%d", registerNumber);
				_stack.push(avm.getRegister(registerNumber));
				length -= 1;
				break;
			}
			case 5: {
				bool boolValue = (_script->readByte() != 0);
				debugC(kDebugActionScript, 1, "actionPush %s", boolValue ? "true" : "false");
				_stack.push(boolValue);
				length -= 1;
				break;
			}
			case 6: {
				// Double values are weird encoded.
				uint32_t value[2];
				value[1] = _script->readUint32LE();
				value[0] = _script->readUint32LE();

				double doubleValue;
				memcpy(&doubleValue, value, 8);

				_stack.push(doubleValue);
				debugC(kDebugActionScript, 1, "actionPush %f", doubleValue);
				length -= 8;
				break;
			}
			case 7: {
				int intValue = _script->readSint32LE();
				_stack.push(intValue);
				debugC(kDebugActionScript, 1, "actionPush %d", intValue);
				length -= 4;
				break;
			}
				// constant pool index 8bit
			case 8: {
				byte byteValue = _script->readByte();
				_stack.push(_constants[byteValue]);
				debugC(kDebugActionScript, 1, "actionPush \"%s\"", _constants[byteValue].c_str());
				length -= 1;
				break;
			}
				// constant pool index 16bit
			case 9: {
				unsigned short shortValue = _script->readUint16LE();
				_stack.push(_constants[shortValue]);
				debugC(kDebugActionScript, 1, "actionPush \"%s\"", _constants[shortValue].c_str());
				length -= 2;
				break;
			}
			default:
				throw Common::Exception("invalid type byte in actionscript");
		}
	}
}

void ASBuffer::actionJump() {
	int16_t offset = _script->readSint16LE();

	_script->seek(offset, Common::SeekableReadStream::kOriginCurrent);
	_seeked = offset;

	debugC(kDebugActionScript, 1, "actionJump %d", offset);
}

void ASBuffer::actionGetURL2(AVM &avm) {
	Common::BitStream8MSB bitstream(_script);

	byte sendVarsMethodId = bitstream.getBits(2);

	byte reserved = bitstream.getBits(4);
	assert(reserved == 0);

	byte loadTargetFlag = bitstream.getBit();
	byte loadVariablesFlag = bitstream.getBit();

	Common::UString sendVarsMethod;
	switch (sendVarsMethodId) {
		case 1:
			sendVarsMethod = "GET";
			break;
		case 2:
			sendVarsMethod = "POST";
			break;
		default:
			sendVarsMethod = "None";
	}

	Common::UString target = _stack.top().asString();
	_stack.pop();
	Common::UString url = _stack.top().asString();
	_stack.pop();

	avm.fsCommand(target, url);

	debugC(
			kDebugActionScript,
			1,
			"actionGetURL2 %s %i %i",
			sendVarsMethod.c_str(),
			loadTargetFlag,
			loadVariablesFlag
	);
}

void ASBuffer::actionDefineFunction() {
	Common::UString functionName = readString();

	uint16_t numParams = _script->readUint16LE();
	for (int i = 0; i < numParams; ++i) {
		Common::UString param = readString();
	}

	uint16_t codeSize = _script->readUint16LE();
	_stack.push(ObjectPtr(new ScriptedFunction(_script->readStream(codeSize), _constants, std::vector<uint8_t>(), 0, false, false, false, false)));

	_seeked = codeSize;

	debugC(
			kDebugActionScript,
			1,
			"actionDefineFunction %s",
			functionName.c_str()
	);
}

void ASBuffer::actionIf() {
	short branchOffset = _script->readSint16LE();

	Variable variable = _stack.top();
	_stack.pop();

	if (variable.asBoolean()) {
		_script->seek(branchOffset, Common::SeekableReadStream::kOriginCurrent);
		_seeked = branchOffset;
	}

	debugC(kDebugActionScript, 1, "actionIf %i", branchOffset);
}

Common::UString ASBuffer::readString() {
	Common::UString string;

	uint32_t character = _script->readChar();
	while (character != 0) {
		string += character;
		character = _script->readChar();
	}
	return string;
}

} // End of namespace ActionScript

} // End of namespace Aurora
