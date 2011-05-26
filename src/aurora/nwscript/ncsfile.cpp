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
#include "aurora/nwscript/functionman.h"

static const uint32 kNCSTag    = MKID_BE('NCS ');
static const uint32 kVersion10 = MKID_BE('V1.0');

static const uint32 kScriptObjectSelf    = 0;
static const uint32 kScriptObjectInvalid = 1;

namespace Aurora {

namespace NWScript {

NCSStack::NCSStack() {
	reset();
}

NCSStack::~NCSStack() {
}

void NCSStack::reset() {
	clear();

	_stackPtr = -1;
	_basePtr  = -1;
}

bool NCSStack::empty() const {
	return _stackPtr < 0;
}

Variable &NCSStack::top() {
	if (_stackPtr == -1)
		throw Common::Exception("NCSStack: Stack underflow");

	return at(_stackPtr);
}

Variable NCSStack::pop() {
	if (_stackPtr == -1)
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

Variable &NCSStack::get(int32 pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::get(): Illegal position %d", pos);

	int32 stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::get(): Position %d below the bottom", pos);

	return at(stackPos);
}

void NCSStack::set(int32 pos, const Variable &obj) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::set(): Illegal position %d", pos);

	int32 stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::set(): Position %d below the bottom", pos);

	at(stackPos) = obj;
}

int32 NCSStack::getStackPtr() {
	return (_stackPtr + 1) * -4;
}

void NCSStack::setStackPtr(int32 pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setStackPtr(): Illegal position %d", pos);

	_stackPtr = (pos / -4) - 1;

	if (size() < (_stackPtr + 1))
		resize(_stackPtr + 1);
}

int32 NCSStack::getBasePtr() {
	return (_basePtr + 1) * -4;
}

void NCSStack::setBasePtr(int32 pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setBasePtr(): Illegal position %d", pos);

	_basePtr = (pos / -4) - 1;
}

void NCSStack::print() const {
	warning(".--- %d ---.", _stackPtr);
	for (int32 i = _stackPtr; i >= 0; i--) {
		const Variable &var = at(i);

		if      (var.getType() == kTypeInt)
			warning("| %d: %d", var.getType(), var.getInt());
		else if (var.getType() == kTypeFloat)
			warning("| %d: %f", var.getType(), var.getFloat());
		else if (var.getType() == kTypeString)
			warning("| %d: \"%s\"", var.getType(), var.getString().c_str());
		else
			warning("| %d", var.getType());
	}
	warning("'--- ---'");
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

NCSFile::NCSFile(Common::SeekableReadStream *ncs, Object *self) : _objectSelf(self) {
	_script = ncs;

	load();
}

NCSFile::NCSFile(const Common::UString &ncs, Object *self) : _script(0), _objectSelf(self) {
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

	reset();
}

void NCSFile::reset() {
	_stack.reset();

	while (!_returnOffsets.empty())
		_returnOffsets.pop();

	_script->seek(13); // 8 byte header + 5 byte program size dummy op

	_savedBasePtr = -4;
}

const Variable &NCSFile::run() {
	reset();

	while (!_script->eos() && !_script->err())
		executeStep();

	if (_script->err())
		throw Common::Exception(Common::kReadError);

	if (_stack.empty())
		_stack.push(kTypeVoid);

	return _stack.top();
}

void NCSFile::executeStep() {
	byte opcode = _script->readByte();
	InstructionType type = (InstructionType)_script->readByte();

	if (opcode >= _opcodeListSize)
		throw Common::Exception("NCSFile::executeStep(): Illegal instruction 0x%02x", opcode);

	warning("NWScript opcode %s [0x%02X]", _opcodes[opcode].desc, opcode);

	try {
		(this->*(_opcodes[opcode].proc))(type);
	} catch (Common::Exception e) {
		throw e;
	}

	// _stack.print();
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
			_stack.push(_script->readSint32BE());
			break;

		case kInstTypeFloat:
			_stack.push(_script->readIEEEFloatBE());
			break;

		case kInstTypeString: {
			_stack.push(kTypeString);
			_stack.top().getString().readFixedASCII(*_script, _script->readUint16BE());
			break;
		}

		case kInstTypeObject: {
			uint32 objectID = _script->readUint32BE();

			if      (objectID == kScriptObjectSelf)
				_stack.push(_objectSelf);
			else if (objectID == kScriptObjectInvalid)
				_stack.push((Object *) 0);
			else
				throw Common::Exception("NCSFile::o_const(): Illegal object ID %d", objectID);

			break;
		}

		default:
			throw Common::Exception("NCSFile::o_const(): Illegal type %d", type);
	}
}

void NCSFile::callEngine(uint32 function, uint8 argCount) {
	Aurora::NWScript::FunctionContext ctx = FunctionMan.createContext(function);
	if ((argCount < ctx.getParamMin()) || (argCount > ctx.getParamMax()))
		throw Common::Exception("NCSFile::callEngine(): Argument count for function \"%s\" "
		                        "mismatch (got %d, want %d - %d)", ctx.getName().c_str(),
		                        argCount, ctx.getParamMin(), ctx.getParamMax());

	ctx.setCaller(_objectSelf);

	for (uint8 i = 0; i < argCount; i++) {
		Variable &param = ctx.getParams()[i];

		switch (param.getType()) {
			case kTypeInt:
			case kTypeFloat:
			case kTypeString:
			case kTypeObject:
				param = _stack.pop();
				break;

			case kTypeVector: {
				float z = _stack.pop().getFloat();
				float y = _stack.pop().getFloat();
				float x = _stack.pop().getFloat();

				param.setVector(x, y, z);
				break;
			}

			default:
				throw Common::Exception("NCSFile::callEngine(): Invalid argument type %d",
				                        param.getType());
				break;
		}

	}

	FunctionMan.call(function, ctx);

	Variable &retVal = ctx.getReturn();
	switch (retVal.getType()) {
		case kTypeVoid:
			break;

		case kTypeInt:
		case kTypeFloat:
		case kTypeString:
		case kTypeObject:
			_stack.push(retVal);
			break;

		case kTypeVector: {
			float x, y, z;
			retVal.getVector(x, y, z);

			_stack.push(x);
			_stack.push(y);
			_stack.push(z);
			break;
		}

		default:
			throw Common::Exception("NCSFile::callEngine(): Invalid return type %d",
			                        retVal.getType());
			break;
	}

	warning("NWScript engine function %s (%d)", ctx.getName().c_str(), function);
}

void NCSFile::o_action(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_action(): Illegal type %d", type);

	uint16 routineNumber = _script->readUint16BE();
	uint8  argCount      = _script->readByte();

	callEngine(routineNumber, argCount);
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
		_script->readUint16BE();

	Variable arg1 = _stack.pop();
	Variable arg2 = _stack.pop();

	_stack.push(arg1 == arg2);
}

void NCSFile::o_neq(InstructionType type) {
	if (type == kInstTypeStructStruct) // TODO!
		_script->readUint16BE();

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

	_stack.setStackPtr(_stack.getStackPtr() - _script->readSint32BE());
}

void NCSFile::o_jmp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jmp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();
	_script->seek(offset - 6, SEEK_CUR);
}

void NCSFile::o_jz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jz(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	if (!_stack.pop().getInt())
		_script->seek(offset - 6, SEEK_CUR);
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
	_stack.setStackPtr(oldStackPtr + _script->readSint32BE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_incsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incsp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(oldStackPtr + _script->readSint32BE());
	_stack.push(_stack.pop().getInt() + 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_jnz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jnz(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	if (_stack.pop().getInt())
		_script->seek(offset - 6, SEEK_CUR);
}

void NCSFile::o_decbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decbp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32BE());
	_stack.push(_stack.pop().getInt() - 1);
	_stack.setStackPtr(oldStackPtr);
}

void NCSFile::o_incbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incbp(): Illegal type %d", type);

	int32 oldStackPtr = _stack.getStackPtr();
	_stack.setStackPtr(_stack.getBasePtr() + _script->readSint32BE());
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
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cpdownsp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();
	int16 size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cpdownsp(): Illegal size %d", size);

	int32 startPos = -size;
	while (size > 0) {
		_stack.set(offset, _stack.get(startPos));

		startPos += 4;
		offset   += 4;
		size     -= 4;
	}
}

void NCSFile::o_cptopsp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cptopsp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();
	int16 size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cptopsp(): Illegal size %d", size);

	while (size > 0) {
		_stack.push(_stack.get(offset));

		size -= 4;
	}
}

void NCSFile::o_add(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32) (op1.getInt() + op2.getInt()));
			break;
		}

		case kInstTypeFloatFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() + op2.getFloat());
			break;
		}

		case kInstTypeIntFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(((float) op1.getInt()) + op2.getFloat());
			break;
		}

		case kInstTypeFloatInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() + ((float) op2.getInt()));
			break;
		}

		case kInstTypeStringString: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getString() + op2.getString());
			break;
		}

		case kInstTypeVectorVector: {
			Variable op2z = _stack.pop();
			Variable op2y = _stack.pop();
			Variable op2x = _stack.pop();
			Variable op1z = _stack.pop();
			Variable op1y = _stack.pop();
			Variable op1x = _stack.pop();

			_stack.push(op1z.getFloat() + op2z.getFloat());
			_stack.push(op1y.getFloat() + op2y.getFloat());
			_stack.push(op1x.getFloat() + op2x.getFloat());
			break;
		}

		default:
			throw Common::Exception("NCSFile::o_add(): Illegal type %d", type);
	}
}

void NCSFile::o_sub(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32) (op1.getInt() - op2.getInt()));
			break;
		}

		case kInstTypeFloatFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() - op2.getFloat());
			break;
		}

		case kInstTypeIntFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(((float) op1.getInt()) - op2.getFloat());
			break;
		}

		case kInstTypeFloatInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() - ((float) op2.getInt()));
			break;
		}

		case kInstTypeVectorVector: {
			Variable op2z = _stack.pop();
			Variable op2y = _stack.pop();
			Variable op2x = _stack.pop();
			Variable op1z = _stack.pop();
			Variable op1y = _stack.pop();
			Variable op1x = _stack.pop();

			_stack.push(op1z.getFloat() - op2z.getFloat());
			_stack.push(op1y.getFloat() - op2y.getFloat());
			_stack.push(op1x.getFloat() - op2x.getFloat());
			break;
		}

		default:
			throw Common::Exception("NCSFile::o_sub(): Illegal type %d", type);
	}
}

void NCSFile::o_mul(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32) (op1.getInt() * op2.getInt()));
			break;
		}

		case kInstTypeFloatFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() * op2.getFloat());
			break;
		}

		case kInstTypeIntFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(((float) op1.getInt()) * op2.getFloat());
			break;
		}

		case kInstTypeFloatInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() * ((float) op2.getInt()));
			break;
		}

		case kInstTypeVectorFloat: {
			Variable op2  = _stack.pop();
			Variable op1z = _stack.pop();
			Variable op1y = _stack.pop();
			Variable op1x = _stack.pop();

			_stack.push(op1z.getFloat() * op2.getFloat());
			_stack.push(op1y.getFloat() * op2.getFloat());
			_stack.push(op1x.getFloat() * op2.getFloat());
			break;
		}

		case kInstTypeFloatVector: {
			Variable op2z = _stack.pop();
			Variable op2y = _stack.pop();
			Variable op2x = _stack.pop();
			Variable op1  = _stack.pop();

			_stack.push(op1.getFloat() * op2z.getFloat());
			_stack.push(op1.getFloat() * op2y.getFloat());
			_stack.push(op1.getFloat() * op2x.getFloat());
			break;
		}

		default:
			throw Common::Exception("NCSFile::o_mul(): Illegal type %d", type);
	}
}

void NCSFile::o_div(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32) (op1.getInt() / op2.getInt()));
			break;
		}

		case kInstTypeFloatFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() / op2.getFloat());
			break;
		}

		case kInstTypeIntFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(((float) op1.getInt()) / op2.getFloat());
			break;
		}

		case kInstTypeFloatInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push(op1.getFloat() / ((float) op2.getInt()));
			break;
		}

		case kInstTypeVectorFloat: {
			Variable op2  = _stack.pop();
			Variable op1z = _stack.pop();
			Variable op1y = _stack.pop();
			Variable op1x = _stack.pop();

			_stack.push(op1z.getFloat() / op2.getFloat());
			_stack.push(op1y.getFloat() / op2.getFloat());
			_stack.push(op1x.getFloat() / op2.getFloat());
			break;
		}

		case kInstTypeFloatVector: {
			Variable op2z = _stack.pop();
			Variable op2y = _stack.pop();
			Variable op2x = _stack.pop();
			Variable op1  = _stack.pop();

			_stack.push(op1.getFloat() / op2z.getFloat());
			_stack.push(op1.getFloat() / op2y.getFloat());
			_stack.push(op1.getFloat() / op2x.getFloat());
			break;
		}

		default:
			throw Common::Exception("NCSFile::o_div(): Illegal type %d", type);
	}
}

void NCSFile::o_storestateall(InstructionType type) {
	throw Common::Exception("TODO: NCSFile::o_storestateall()");
}

void NCSFile::o_jsr(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jsr(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	// Push the current script position
	_returnOffsets.push(_script->pos());

	_script->seek(offset - 6, SEEK_CUR);
}

void NCSFile::o_retn(InstructionType type) {
	uint32 returnAddress = _script->size();
	if (!_returnOffsets.empty()) {
		returnAddress = _returnOffsets.top();
		_returnOffsets.pop();
	}

	_script->seek(returnAddress);
}

void NCSFile::o_destruct(InstructionType type) {
	int16 stackSize        = _script->readSint16BE();
	int16 dontRemoveOffset = _script->readSint16BE();
	int16 dontRemoveSize   = _script->readSint16BE();

	if ((stackSize % 4) != 0)
		throw Common::Exception("NCSFile::o_destruct(): Illegal stack size %d", stackSize);
	if ((dontRemoveOffset % 4) != 0)
		throw Common::Exception("NCSFile::o_destruct(): Illegal offset %d", dontRemoveOffset);
	if ((dontRemoveSize % 4) != 0)
		throw Common::Exception("NCSFile::o_destruct(): Illegal size %d", dontRemoveSize);

	std::vector<Variable> tmp;
	tmp.reserve(dontRemoveSize / 4);

	while (stackSize > 0) {

		if ((stackSize <= (dontRemoveOffset + dontRemoveSize)) &&
		    (stackSize >   dontRemoveOffset))
			tmp.push_back(_stack.top());

		_stack.pop();

		stackSize -= 4;
	}

	for (std::vector<Variable>::const_reverse_iterator t = tmp.rbegin(); t != tmp.rend(); ++t)
		_stack.push(*t);
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
