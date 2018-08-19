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
	kActionExtends         = 0x69,
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

		uint16 length = 0;
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
			case kActionDefineLocal:     actionDefineLocal(); break;
			case kActionCallFunction:    actionCallFunction(); break;
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
			case kActionExtends:         actionExtends(); break;
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

		if (_script->pos() - startPos != length + _seeked)
			throw Common::Exception("Invalid tag");
	} while (opcode != 0 && _script->pos() != _script->size());

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
	_stack.pop();

	debugC(kDebugActionScript, 1, "actionPop");
}

void ASBuffer::actionGetVariable(AVM &avm) {
	Common::UString name = _stack.top().asString();
	_stack.pop();

	_stack.push(avm.getVariable(name));

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

void ASBuffer::actionDefineLocal() {
	// TODO

	debugC(kDebugActionScript, 1, "actionDefineLocal");
}

void ASBuffer::actionCallFunction() {
	// TODO

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

	_stack.push(a + b);

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

	_stack.push(object->getMember(name));

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
	if (name.empty()) {
		function = dynamic_cast<Function *>(object.get());
	} else {
		function = dynamic_cast<Function *>(object->getMember(name).asObject().get());
	}

	if (!function)
		throw Common::Exception("Object is not a function");

	byte counter = 1;

	if (function->getPreloadThisFlag()) {
		if (!name.empty())
			avm.storeRegister(object, counter);
		counter += 1;
	}
	if (function->getPreloadSuperFlag()) {
		// TODO: Add super variable
		counter += 1;
	}

	for (size_t i = 0; i < arguments.size(); ++i) {
		avm.storeRegister(arguments[i], counter);
		counter += 1;
	}

	avm.setReturnValue(Variable());
	_stack.push((*function)(avm));

	debugC(kDebugActionScript, 1, "actionCallMethod");
}

void ASBuffer::actionEnumerate2() {
	// TODO

	debugC(kDebugActionScript, 1, "actionEnumerate2");
}

void ASBuffer::actionExtends() {
	ObjectPtr superClass = _stack.top().asObject();
	_stack.pop();
	ObjectPtr subClass = _stack.top().asObject();
	_stack.pop();

	ObjectPtr scriptObject(new Object(superClass->getMember("prototype").asObject().get()));
	scriptObject->setMember("constructor", superClass);

	subClass->setMember("prototype", scriptObject);

	debugC(kDebugActionScript, 1, "actionExtends");
}

void ASBuffer::actionStoreRegister(AVM &avm) {
	byte registerNumber = _script->readByte();
	avm.storeRegister(_stack.top(), registerNumber);

	debugC(kDebugActionScript, 1, "actionStoreRegister %i", registerNumber);
}

void ASBuffer::actionConstantPool() {
	uint16 count = _script->readUint16LE();
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

	for (int i = 0; i < numParams; ++i) {
		_script->readByte();
		readString();
	}

	unsigned short codeSize = _script->readUint16LE();
	_seeked = codeSize;

	_stack.push(
			ObjectPtr(
					new ScriptedFunction(
							new Common::SeekableSubReadStream(_script, _script->pos(), _script->pos() + codeSize),
							_constants,
							preloadThisFlag,
							preloadSuperFlag
					)
			)
	);

	_script->seek(codeSize, Common::SeekableReadStream::kOriginCurrent);


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
				_stack.push(Variable());
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
				double doubleValue = _script->readIEEEDoubleLE();
				_stack.push(doubleValue);
				debugC(kDebugActionScript, 1, "actionPush %f", doubleValue);
				length -= 8;
				break;
			}
			case 7: {
				unsigned int uintValue = _script->readUint32LE();
				_stack.push(uintValue);
				debugC(kDebugActionScript, 1, "actionPush %u", uintValue);
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
	unsigned short offset = _script->readUint16LE();

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

	uint16 numParams = _script->readUint16LE();
	for (int i = 0; i < numParams; ++i) {
		Common::UString param = readString();
	}

	uint16 codeSize = _script->readUint16LE();
	_script->seek(codeSize, Common::SeekableReadStream::kOriginCurrent);

	ASBuffer buffer(new Common::SeekableSubReadStream(_script, _script->pos(), _script->pos() + codeSize));

	_stack.push(
			ObjectPtr(
					new ScriptedFunction(
							new Common::SeekableSubReadStream(_script, _script->pos(), _script->pos() + codeSize),
							_constants, false, false
					)
			)
	);

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

	uint32 character = _script->readChar();
	while (character != 0) {
		string += character;
		character = _script->readChar();
	}
	return string;
}

} // End of namespace ActionScript

} // End of namespace Aurora
