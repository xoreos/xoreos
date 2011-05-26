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

/** @file aurora/nwscript/ncsfile.cpp
 *  Handling BioWare's NWN Compiled Scripts.
 */

#include "common/util.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "aurora/error.h"
#include "aurora/resman.h"

#include "aurora/nwscript/ncsfile.h"

static const uint32 kNCSTag    = MKID_BE('NCS ');
static const uint32 kVersion10 = MKID_BE('V1.0');

namespace Aurora {

namespace NWScript {

NCSStack::NCSStack() {
	_stackPtr = -1;
	_basePtr  = -1;
}

NCSStack::~NCSStack() {
}

Variable &NCSStack::top() {
	return at(_stackPtr);
}

Variable NCSStack::pop() {
	if (_stackPtr == 0)
		throw Common::Exception("NCSStack: Stack underflow");

	return at(_stackPtr--);
}

void NCSStack::push(const Variable &obj) {
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
	if ((pos >= -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setStackPtr(): Illegal position %d", pos);

	_stackPtr = (pos / -4) - 1;
}

int32 NCSStack::getBasePtr() {
	return (_basePtr + 1) * -4;
}

void NCSStack::setBasePtr(int32 pos) {
	if ((pos >= -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setBasePtr(): Illegal position %d", pos);

	_basePtr = (pos / -4) - 1;
}

#define OPCODE(x) { &NCSFile::x, #x }

void NCSFile::setupOpcodes() {
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

NCSFile::NCSFile(Common::SeekableReadStream *ncs) {
	_script = ncs;

	load();
}

NCSFile::NCSFile(const Common::UString &ncs) : _script(0) {
	_script = ResMan.getResource(ncs, kFileTypeNCS);
	if (!_script)
		throw Common::Exception("No such NCS \"%s\"", ncs.c_str());

	load();
}

NCSFile::~NCSFile() {
	delete _script;
}

void NCSFile::load() {
	readHeader(*_script);

	if (_id != kNCSTag)
		throw Common::Exception("Try to load non-NCS file");

	if (_version != kVersion10)
		throw Common::Exception("Unsupported NCS file version %08X", _version);

	byte lengthOpcode = _script->readByte();
	if (lengthOpcode != 0x42)
		throw Common::Exception("Script size opcode != 0x42 (0x%02X)", lengthOpcode);

	uint32 length = _script->readUint32BE();
	if (length > ((uint32) _script->size()))
		throw Common::Exception("Script size %d > stream size %d", length, _script->size());
	if (length < ((uint32) _script->size()))
		warning("TODO: NCSFile::load(): Script size %d < stream size %d", length, _script->size());

	setupOpcodes();
	_savedBasePtr = -4;
}

void NCSFile::executeStep() {
	byte opcode = _script->readByte();
	InstructionType type = (InstructionType)_script->readByte();

	if (opcode >= _opcodeListSize || opcode == 0)
		throw Common::Exception("NCSFile::executeStep(): Illegal instruction 0x%02x", opcode);

	try {
		(this->*(_opcodes[opcode].proc))(type);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::decompile() {
	uint32 oldScriptPos = _script->pos();
	_script->seek(13); // 8 byte header + 5 byte program size dummy op

	// TODO

	_script->seek(oldScriptPos);
}

// OPCODES!

void NCSFile::o_rsadd(InstructionType type) {
	switch (type) {
	case kInstTypeInt:
		_stack.push(kTypeInt);
		break;
	case kInstTypeFloat:
		_stack.push(kTypeFloat);
		break;
	case kInstTypeString:
		_stack.push(kTypeString);
		break;
	case kInstTypeObject:
		_stack.push(kTypeObject);
		break;
	default:
		throw Common::Exception("NCSFile::o_rsadd(): Illegal type %d", type);
	}
}

void NCSFile::o_const(InstructionType type) {
	switch (type) {
	case kInstTypeInt:
		_stack.push(_script->readSint32LE());
		break;
	case kInstTypeFloat:
		_stack.push(_script->readIEEEFloatLE());
		break;
	case kInstTypeString: {
		_stack.push(kTypeString);
		_stack.top().getString().readFixedASCII(*_script, _script->readUint16LE());
		break;
	}
	case kInstTypeObject: {
		uint32 objectID = _script->readUint32LE();

		if (objectID != 0)
			throw Common::Exception("NCSFile::o_const(): Illegal object ID %d", objectID);

		_stack.push((Object *) 0);
		break;
	}
	default:
		throw Common::Exception("NCSFile::o_const(): Illegal type %d", type);
	}
}

void NCSFile::o_action(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_action(): Illegal type %d", type);

	uint16 routineNumber = _script->readUint16LE();
	byte argCount = _script->readByte();

	warning("STUB: NCSFile::o_action(): Run routine %d with %d args", routineNumber, argCount);

	// For now, pop off the args and put a 0 on the stack :P

	for (byte i = 0; i < argCount; i++)
		_stack.pop();
	_stack.push(0);
}

void NCSFile::o_logand(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logand(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_logor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 || arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_incor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_incor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 | arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_excor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_excor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 ^ arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_booland(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_booland(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_eq(InstructionType type) {
	if (type == kInstTypeStructStruct) // TODO!
		_script->readUint16LE();

	Variable arg1 = _stack.pop();
	Variable arg2 = _stack.pop();

	_stack.push(arg1 == arg2);
}

void NCSFile::o_neq(InstructionType type) {
	if (type == kInstTypeStructStruct) // TODO!
		_script->readUint16LE();

	Variable arg1 = _stack.pop();
	Variable arg2 = _stack.pop();

	_stack.push(arg1 != arg2);
}

void NCSFile::o_geq(InstructionType type) {
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
		throw Common::Exception("NCSFile::o_geq(): Illegal type %d", type);
	}
}

void NCSFile::o_gt(InstructionType type) {
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
		throw Common::Exception("NCSFile::o_gt(): Illegal type %d", type);
	}
}

void NCSFile::o_lt(InstructionType type) {
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
		throw Common::Exception("NCSFile::o_lt(): Illegal type %d", type);
	}
}

void NCSFile::o_leq(InstructionType type) {
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
		throw Common::Exception("NCSFile::o_leq(): Illegal type %d", type);
	}
}

void NCSFile::o_shleft(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_shleft(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 << arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_shright(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_shright(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 >> arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_ushright(InstructionType type) {
	// TODO: Difference between this and o_shright

	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_ushright(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 >> arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_mod(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_mod(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 % arg1);
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_neg(InstructionType type) {
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
		throw Common::Exception("NCSFile::o_neg(): Illegal type %d", type);
	}
}

void NCSFile::o_comp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_comp(): Illegal type %d", type);

	try {
		_stack.push(~_stack.pop().getInt());
	} catch (Common::Exception e) {
		throw e;
	}
}

void NCSFile::o_movsp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_movsp(): Illegal type %d", type);

	_stack.setStackPtr(_stack.getStackPtr() + _script->readSint32LE());
}

void NCSFile::o_jmp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jmp(): Illegal type %d", type);

	int32 offset = _script->readSint32LE();
	_script->seek(offset - 2, SEEK_CUR);
}

void NCSFile::o_jz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jz(): Illegal type %d", type);

	int32 offset = _script->readSint32LE();

	if (!_stack.pop().getInt())
		_script->seek(offset - 2, SEEK_CUR);
}

void NCSFile::o_not(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_not(): Illegal type %d", type);

	_stack.push(!_stack.pop().getInt());
}

void NCSFile::o_decsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decsp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(oldStackPtr + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_incsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incsp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(oldStackPtr + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() + 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_jnz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jnz(): Illegal type %d", type);

	int32 offset = _script->readSint32LE();

	if (_stack.pop().getInt())
		_script->seek(offset - 2, SEEK_CUR);
}

void NCSFile::o_decbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decbp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_incbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incbp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32LE());
	_stack.push(_stack.pop().getInt() + 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_savebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_savebp(): Illegal type %d", type);

	_savedBasePtr = _stack.getBasePtr();
	_stack.setBasePtr(_stack.getStackPtr());
}

void NCSFile::o_restorebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_restorebp(): Illegal type %d", type);

	_stack.setBasePtr(_savedBasePtr);
}

void NCSFile::o_nop(InstructionType type) {
	// Nothing! Yay!
}

void NCSFile::o_cpdownsp(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_cpdownsp()");
}

void NCSFile::o_cptopsp(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_cptopsp()");
}

void NCSFile::o_add(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_add()");
}

void NCSFile::o_sub(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_sub()");
}

void NCSFile::o_mul(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_mul()");
}

void NCSFile::o_div(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_div()");
}

void NCSFile::o_storestateall(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_storestateall()");
}

void NCSFile::o_jsr(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_jsr()");
}

void NCSFile::o_retn(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_retn()");
}

void NCSFile::o_destruct(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_destruct()");
}

void NCSFile::o_cpdownbp(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_cpdownbp()");
}

void NCSFile::o_cptopbp(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_cptopbp()");
}

void NCSFile::o_storestate(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_storestate()");
}

} // End of namespace NWScript

} // End of namespace Aurora
