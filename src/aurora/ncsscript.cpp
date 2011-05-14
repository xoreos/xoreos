/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/ncsscript.cpp
 *  Handling BioWare's NWN Compiled Scripts.
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/ncsscript.h"
#include "aurora/error.h"

namespace Aurora {

StackObject::StackObject(StackObjectType type) {
	_type = type;
}

StackObject::StackObject(int32 val, StackObjectType type) {
	_intVal = val;
	_type = type;
}

StackObject::StackObject(float val) {
	_floatVal = val;
	_type = kStackObjectFloat;
}

StackObject::StackObject(const Common::UString &val) {
	_stringVal = val;
	_type = kStackObjectString;
}

int32 StackObject::getInt() const {
	if (_type != kStackObjectInt)
		throw Common::Exception("Trying to get int from non-int StackObject");
	return (int32)_intVal;
}

float StackObject::getFloat() const {
	if (_type != kStackObjectFloat)
		throw Common::Exception("Trying to get float from non-float StackObject");
	return _floatVal;
}

const Common::UString &StackObject::getString() const {
	if (_type != kStackObjectString)
		throw Common::Exception("Trying to get string from non-string StackObject");
	return _stringVal;
}

uint32 StackObject::getObject() const {
	if (_type != kStackObjectObject)
		throw Common::Exception("Trying to get object from non-object StackObject");
	return _intVal;
}

bool StackObject::operator==(StackObject &obj) {
	if (_type != obj._type)
		return false;

	switch (_type) {
	case kStackObjectNone:
		return true;
	case kStackObjectInt:
	case kStackObjectObject:
		return _intVal == obj._intVal;
	case kStackObjectFloat:
		return _floatVal == obj._floatVal;
	case kStackObjectString:
		return _stringVal == obj._stringVal;
	default:
		warning("Unhandled operator== for StackObject type %d", _type);
	}

	return false;
}

NCSStack::NCSStack() {
	_stackPtr = -1;
	_basePtr = -1;
}

NCSStack::~NCSStack() {
}

StackObject NCSStack::top() {
	return at(_stackPtr);
}

StackObject NCSStack::pop() {
	if (_stackPtr == 0)
		throw Common::Exception("NCSStack: Stack underflow");

	return at(_stackPtr--);
}

void NCSStack::push(StackObject obj) {
	if (_stackPtr == 0x7FFFFFFF) // Like this will ever happen :P
		throw Common::Exception("NCSStack: Stack overflow");

	if (_stackPtr == (int32)size() - 1)
		push_back(obj);
	else
		at(_stackPtr + 1) = obj;

	_stackPtr++;
}

int32 NCSStack::getStackPtr() {
	return (_stackPtr + 1) * -4;
}

void NCSStack::setStackPtr(int32 pos) {
	if (pos >= -4 || (pos % 4) != 0)
		throw Common::Exception("NCSStack::setStackPtr(): Illegal position %d", pos);

	_stackPtr = (pos / -4) - 1;
}

int32 NCSStack::getBasePtr() {
	return (_basePtr + 1) * -4;
}

void NCSStack::setBasePtr(int32 pos) {
	if (pos >= -4 || (pos % 4) != 0)
		throw Common::Exception("NCSStack::setBasePtr(): Illegal position %d", pos);

	_basePtr = (pos / -4) - 1;
}

#define OPCODE(x) { &NCSScript::x, #x }

void NCSScript::setupOpcodes() {
	static const Opcode opcodes[] = {
		// 0x00
		OPCODE(o_nop), // Doesn't exist
		OPCODE(o_cpdownsp),
		OPCODE(o_rsadd),
		OPCODE(o_cptopsp),
		// 0x04
		OPCODE(o_const),
		OPCODE(o_action),
		OPCODE(o_logand),
		OPCODE(o_logor),
		// 0x08
		OPCODE(o_incor),
		OPCODE(o_excor),
		OPCODE(o_booland),
		OPCODE(o_eq),
		// 0x0C
		OPCODE(o_neq),
		OPCODE(o_geq),
		OPCODE(o_gt),
		OPCODE(o_lt),
		// 0x10
		OPCODE(o_leq),
		OPCODE(o_shleft),
		OPCODE(o_shright),
		OPCODE(o_ushright),
		// 0x14
		OPCODE(o_add),
		OPCODE(o_sub),
		OPCODE(o_mul),
		OPCODE(o_div),
		// 0x18
		OPCODE(o_mod),
		OPCODE(o_neg),
		OPCODE(o_comp),
		OPCODE(o_movsp),
		// 0x1C
		OPCODE(o_storestateall),
		OPCODE(o_jmp),
		OPCODE(o_jsr),
		OPCODE(o_jz),
		// 0x20
		OPCODE(o_retn),
		OPCODE(o_destruct),
		OPCODE(o_not),
		OPCODE(o_decsp),
		// 0x24
		OPCODE(o_incsp),
		OPCODE(o_jnz),
		OPCODE(o_cpdownbp),
		OPCODE(o_cptopbp),
		// 0x28
		OPCODE(o_decbp),
		OPCODE(o_incbp),
		OPCODE(o_savebp),
		OPCODE(o_restorebp),
		// 0x2C
		OPCODE(o_storestate),
		OPCODE(o_nop)
	};

	_opcodes = opcodes;
	_opcodeListSize = ARRAYSIZE(opcodes);
}

#undef OPCODE

static const uint32 kNCSTag = MKID_BE('NCS ');
static const uint32 kVersion10 = MKID_BE('V1.0');

NCSScript::NCSScript() {
	setupOpcodes();
	_savedBasePtr = -4;
}

void NCSScript::load(Common::SeekableReadStream &ncs) {
	readHeader(ncs);

	if (_id != kNCSTag)
		throw Common::Exception("Try to load non-NCS file");

	if (_version != kVersion10)
		throw Common::Exception("Unsupported NCS file version %08X", _version);

	_script = &ncs;
	_script->skip(5); // Program Size
}

void NCSScript::executeStep() {
	byte opcode = _script->readByte();
	InstructionType type = (InstructionType)_script->readByte();

	if (opcode >= _opcodeListSize || opcode == 0)
		throw Common::Exception("Illegal instruction 0x%02x", opcode);

	try {
		(this->*(_opcodes[opcode].proc))(type);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::decompile() {
	uint32 oldScriptPos = _script->pos();
	_script->seek(13); // 8 byte header + 5 byte program size dummy op

	// TODO

	_script->seek(oldScriptPos);
}

// OPCODES!

void NCSScript::o_rsadd(InstructionType type) {
	switch (type) {
	case kInstTypeInt:
		_stack.push(StackObject(StackObject::kStackObjectInt));
		break;
	case kInstTypeFloat:
		_stack.push(StackObject(StackObject::kStackObjectFloat));
		break;
	case kInstTypeString:
		_stack.push(StackObject(StackObject::kStackObjectString));
		break;
	case kInstTypeObject:
		_stack.push(StackObject(StackObject::kStackObjectObject));
		break;
	default:
		throw Common::Exception("o_rsadd: Illegal type %d", type);
	}
}

void NCSScript::o_const(InstructionType type) {
	switch (type) {
	case kInstTypeInt:
		_stack.push(_script->readSint32LE());
		break;
	case kInstTypeFloat:
		_stack.push(_script->readIEEEFloatLE());
		break;
	case kInstTypeString: {
		Common::UString string;
		string.readFixedASCII(*_script, _script->readUint16LE());
		_stack.push(string);
		break;
	}
	case kInstTypeObject:
		_stack.push(StackObject(_script->readUint32LE(), StackObject::kStackObjectObject));
		break;
	default:
		throw Common::Exception("o_const: Illegal type %d", type);
	}
}

void NCSScript::o_action(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_action: Illegal type %d", type);

	uint16 routineNumber = _script->readUint16LE();
	byte argCount = _script->readByte();

	warning("STUB: o_action: Run routine %d with %d args", routineNumber, argCount);

	// For now, pop off the args and put a 0 on the stack :P

	for (byte i = 0; i < argCount; i++)
		_stack.pop();
	_stack.push(0);
}

void NCSScript::o_logand(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_logand: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_logor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_logor: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 || arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_incor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_incor: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 | arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_excor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_excor: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 ^ arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_booland(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_booland: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_eq(InstructionType type) {
	if (type == kInstTypeStructStruct) // TODO!
		_script->readUint16LE();

	StackObject arg1 = _stack.pop();
	StackObject arg2 = _stack.pop();

	_stack.push(arg1 == arg2);
}

void NCSScript::o_neq(InstructionType type) {
	if (type == kInstTypeStructStruct) // TODO!
		_script->readUint16LE();

	StackObject arg1 = _stack.pop();
	StackObject arg2 = _stack.pop();

	_stack.push(arg1 != arg2);
}

void NCSScript::o_geq(InstructionType type) {
	switch (type) {
	case kInstTypeIntInt:
		try {
			int32 arg1 = _stack.pop().getInt();
			int32 arg2 = _stack.pop().getInt();
			_stack.push(arg2 >= arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	case kInstTypeFloatFloat:
		try {
			float arg1 = _stack.pop().getFloat();
			float arg2 = _stack.pop().getFloat();
			_stack.push(arg2 >= arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	default:
		throw Common::Exception("o_geq: Illegal type %d", type);
	}
}

void NCSScript::o_gt(InstructionType type) {
	switch (type) {
	case kInstTypeIntInt:
		try {
			int32 arg1 = _stack.pop().getInt();
			int32 arg2 = _stack.pop().getInt();
			_stack.push(arg2 > arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	case kInstTypeFloatFloat:
		try {
			float arg1 = _stack.pop().getFloat();
			float arg2 = _stack.pop().getFloat();
			_stack.push(arg2 > arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	default:
		throw Common::Exception("o_gt: Illegal type %d", type);
	}
}

void NCSScript::o_lt(InstructionType type) {
	switch (type) {
	case kInstTypeIntInt:
		try {
			int32 arg1 = _stack.pop().getInt();
			int32 arg2 = _stack.pop().getInt();
			_stack.push(arg2 < arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	case kInstTypeFloatFloat:
		try {
			float arg1 = _stack.pop().getFloat();
			float arg2 = _stack.pop().getFloat();
			_stack.push(arg2 < arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	default:
		throw Common::Exception("o_lt: Illegal type %d", type);
	}
}

void NCSScript::o_leq(InstructionType type) {
	switch (type) {
	case kInstTypeIntInt:
		try {
			int32 arg1 = _stack.pop().getInt();
			int32 arg2 = _stack.pop().getInt();
			_stack.push(arg2 <= arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	case kInstTypeFloatFloat:
		try {
			float arg1 = _stack.pop().getFloat();
			float arg2 = _stack.pop().getFloat();
			_stack.push(arg2 <= arg1);
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	default:
		throw Common::Exception("o_leq: Illegal type %d", type);
	}
}

void NCSScript::o_shleft(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_shleft: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 << arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_shright(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_shright: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 >> arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_ushright(InstructionType type) {
	// TODO: Difference between this and o_shright

	if (type != kInstTypeIntInt)
		throw Common::Exception("o_ushright: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 >> arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_mod(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("o_mod: Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 % arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_neg(InstructionType type) {
	switch (type) {
	case kInstTypeInt:
		try {
			_stack.push(-_stack.pop().getInt());
		} catch (Common::Exception e) {
			throw e;
		}
		break;
	case kInstTypeFloat:
		try {
			_stack.push(-_stack.pop().getFloat());
		} catch (Common::Exception e) {
			throw e;
		}
	default:
		throw Common::Exception("o_neg: Illegal type %d", type);
	}
}

void NCSScript::o_comp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_comp: Illegal type %d", type);

	try {
		_stack.push(~_stack.pop().getInt());
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSScript::o_movsp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_movsp: Illegal type %d", type);

	_stack.setStackPtr(_stack.getStackPtr() + _script->readSint32LE());
}

void NCSScript::o_jmp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_jmp: Illegal type %d", type);

	int32 offset = _script->readSint32LE();
	_script->seek(offset - 2, SEEK_CUR);
}

void NCSScript::o_jz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_jz: Illegal type %d", type);

	int32 offset = _script->readSint32LE();

	if (!_stack.pop().getInt())
		_script->seek(offset - 2, SEEK_CUR);
}

void NCSScript::o_not(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_not: Illegal type %d", type);

	_stack.push(!_stack.pop().getInt());
}

void NCSScript::o_decsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_decsp: Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(oldStackPtr + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSScript::o_incsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_incsp: Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(oldStackPtr + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() + 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSScript::o_jnz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_jnz: Illegal type %d", type);

	int32 offset = _script->readSint32LE();

	if (_stack.pop().getInt())
		_script->seek(offset - 2, SEEK_CUR);
}

void NCSScript::o_decbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_decbp: Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSScript::o_incbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("o_incbp: Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() + 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSScript::o_savebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_savebp: Illegal type %d", type);

	_savedBasePtr = _stack.getBasePtr();
	_stack.setBasePtr(_stack.getStackPtr());
}

void NCSScript::o_restorebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("o_restorebp: Illegal type %d", type);

	_stack.setBasePtr(_savedBasePtr);
}

void NCSScript::o_nop(InstructionType type) {
	// Nothing! Yay!
}

} // End of namespace Aurora
