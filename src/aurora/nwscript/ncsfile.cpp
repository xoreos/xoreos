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
 *  Handling BioWare's NWN Compiled Scripts.
 */

/* Based on the NCS specs by Torlack.
 *
 * Torlack's own site is down, but our docs repository hosts a
 * a mirror (<https://github.com/xoreos/xoreos-docs>).
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"
#include "src/common/debug.h"

#include "src/aurora/resman.h"

#include "src/aurora/nwscript/ncsfile.h"
#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/functionman.h"

using Common::kDebugScripts;

static const uint32 kNCSTag    = MKTAG('N', 'C', 'S', ' ');
static const uint32 kVersion10 = MKTAG('V', '1', '.', '0');

static const uint32 kScriptObjectSelf        = 0x00000000;
static const uint32 kScriptObjectInvalid     = 0x00000001;
static const uint32 kScriptObjectTypeInvalid = 0x7F000000;

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

Variable &NCSStack::getRelSP(int32 pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::get(): Illegal position %d", pos);

	int32 stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::get(): Position %d below the bottom", pos);

	return at(stackPos);
}

void NCSStack::setRelSP(int32 pos, const Variable &obj) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::set(): Illegal position %d", pos);

	int32 stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::set(): Position %d below the bottom", pos);

	at(stackPos) = obj;
}

Variable &NCSStack::getRelBP(int32 pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::get(): Illegal position %d", pos);

	int32 stackPos = _basePtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::get(): Position %d below the bottom", pos);

	return at(stackPos);
}

void NCSStack::setRelBP(int32 pos, const Variable &obj) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::set(): Illegal position %d", pos);

	int32 stackPos = _basePtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::set(): Position %d below the bottom", pos);

	at(stackPos) = obj;
}

int32 NCSStack::getStackPtr() {
	return (_stackPtr + 1) * -4;
}

void NCSStack::setStackPtr(int32 pos) {
	if ((pos > 0) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setStackPtr(): Illegal position %d", pos);

	_stackPtr = (pos / -4) - 1;

	if ((int32)size() < (_stackPtr + 1))
		resize(_stackPtr + 1);
}

int32 NCSStack::getBasePtr() {
	return (_basePtr + 1) * -4;
}

void NCSStack::setBasePtr(int32 pos) {
	if ((pos > 0) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setBasePtr(): Illegal position %d", pos);

	_basePtr = (pos / -4) - 1;
}

void NCSStack::print() const {
	if (!DebugMan.isEnabled(2, kDebugScripts))
		return;

	debugC(2, kDebugScripts, ".--- %d ---.", _stackPtr);
	for (int32 i = _stackPtr; i >= 0; i--) {
		const Variable &var = at(i);

		if      (var.getType() == kTypeInt)
			debugC(2, kDebugScripts, "| %d: %d", var.getType(), var.getInt());
		else if (var.getType() == kTypeFloat)
			debugC(2, kDebugScripts, "| %d: %f", var.getType(), var.getFloat());
		else if (var.getType() == kTypeString)
			debugC(2, kDebugScripts, "| %d: \"%s\"", var.getType(), var.getString().c_str());
		else if (var.getType() == kTypeObject) {
			if (!var.getObject())
				debugC(2, kDebugScripts, "| %d: 0", var.getType());
			else
				debugC(2, kDebugScripts, "| %d: \"%s\"", var.getType(), var.getObject()->getTag().c_str());
		} else
			debugC(2, kDebugScripts, "| %d", var.getType());
	}
	debugC(2, kDebugScripts, "'--- ---'");
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

NCSFile::NCSFile(Common::SeekableReadStream *ncs) : _owner(0), _triggerer(0) {
	_script = ncs;

	try {
		load();
	} catch (...) {
		delete _script;
	}
}

NCSFile::NCSFile(const Common::UString &ncs) : _name(ncs), _script(0),
	_owner(0), _triggerer(0) {

	_script = ResMan.getResource(ncs, kFileTypeNCS);
	if (!_script)
		throw Common::Exception("No such NCS \"%s\"", ncs.c_str());

	load();
}

NCSFile::~NCSFile() {
	delete _script;
}

const Common::UString &NCSFile::getName() const {
	return _name;
}

ScriptState NCSFile::getEmptyState() {
	ScriptState state;

	state.offset = 13; // 8 byte header + 5 byte program size dummy op

	return state;
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
		throw Common::Exception("Script size %u > stream size %u", length, (uint)_script->size());
	if (length < ((uint32) _script->size()))
		warning("TODO: NCSFile::load(): Script size %u < stream size %u", length, (uint)_script->size());

	setupOpcodes();

	reset();
}

void NCSFile::reset() {
	_stack.reset();

	while (!_returnOffsets.empty())
		_returnOffsets.pop();

	_storedState.setType(kTypeVoid);
	_return.setType(kTypeVoid);

	_script->seek(13); // 8 byte header + 5 byte program size dummy op
}

const Variable &NCSFile::run(Object *owner, Object *triggerer) {
	return run(getEmptyState(), owner, triggerer);
}

const Variable &NCSFile::run(const ScriptState &state, Object *owner, Object *triggerer) {
	debugC(1, kDebugScripts, "=== Running script \"%s\" (%d) ===",
	       _name.c_str(), state.offset);

	reset();

	_script->seek(state.offset);

	// Push global variables
	std::vector<class Variable>::const_reverse_iterator var;
	for (var = state.globals.rbegin(); var != state.globals.rend(); ++var)
		_stack.push(*var);

	_stack.setBasePtr(_stack.getStackPtr());

	// Push local variables
	for (var = state.locals.rbegin(); var != state.locals.rend(); ++var)
		_stack.push(*var);

	return execute(owner, triggerer);
}

const Variable &NCSFile::execute(Object *owner, Object *triggerer) {
	_owner     = owner;
	_triggerer = triggerer;

	while (executeStep())
		;

	if (!_stack.empty())
		_return = _stack.top();

	if (!_stack.empty() && (_stack.top().getType() == kTypeInt))
		debugC(1, kDebugScripts, "=> Script\"%s\" returns: %d",
		       _name.c_str(), _stack.top().getInt());

	_owner     = 0;
	_triggerer = 0;

	return _return;
}

bool NCSFile::executeStep() {
	byte opcode, type;

	try {
		opcode = _script->readByte();
		type   = _script->readByte();
	} catch (...) {
		return false;
	}

	if (opcode >= _opcodeListSize)
		throw Common::Exception("NCSFile::executeStep(): Illegal instruction 0x%02x", opcode);

	debugC(1, kDebugScripts, "NWScript opcode %s [0x%02X]", _opcodes[opcode].desc, opcode);

	try {
		(this->*(_opcodes[opcode].proc))((InstructionType)type);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}

	_stack.print();
	debugC(2, kDebugScripts, "[RETURN: %d]",
	       _returnOffsets.empty() ? -1 : _returnOffsets.top());

	return true;
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
		case kInstTypeEffect:
			_stack.push(kTypeEngineType);
			break;
		case kInstTypeEvent:
			_stack.push(kTypeEngineType);
			break;
		case kInstTypeLocation:
			_stack.push(kTypeEngineType);
			break;
		case kInstTypeTalent:
			_stack.push(kTypeEngineType);
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
			_stack.push(Common::readStringFixed(*_script, Common::kEncodingASCII, _script->readUint16BE()));
			break;
		}

		case kInstTypeObject: {
			uint32 objectID = _script->readUint32BE();

			if      (objectID == kScriptObjectSelf)
				_stack.push(_owner);
			else if (objectID == kScriptObjectInvalid)
				_stack.push((Object *) 0);
			else if (objectID == kScriptObjectTypeInvalid)
				_stack.push((Object *) 0);
			else
				throw Common::Exception("NCSFile::o_const(): Illegal object ID %d", objectID);

			break;
		}

		default:
			throw Common::Exception("NCSFile::o_const(): Illegal type %d", type);
	}
}

void NCSFile::callEngine(Aurora::NWScript::FunctionContext &ctx,
                         uint32 function, uint8 argCount) {

	if ((argCount < ctx.getParamMin()) || (argCount > ctx.getParamMax()))
		throw Common::Exception("NCSFile::callEngine(): Argument count mismatch (%u vs %u - %u)",
		                        argCount, (uint)ctx.getParamMin(), (uint)ctx.getParamMax());

	ctx.setCurrentScript(this);
	ctx.setCaller(_owner);
	ctx.setTriggerer(_triggerer);

	ctx.setParamsSpecified(argCount);
	for (uint8 i = 0; i < argCount; i++) {
		Variable &param = ctx.getParams()[i];

		switch (param.getType()) {
			case kTypeInt:
			case kTypeFloat:
			case kTypeString:
			case kTypeObject:
			case kTypeEngineType:
				param = _stack.pop();
				break;

			case kTypeVector: {
				float z = _stack.pop().getFloat();
				float y = _stack.pop().getFloat();
				float x = _stack.pop().getFloat();

				param.setVector(x, y, z);
				break;
			}

			case kTypeScriptState:
				param.getScriptState() = _storedState.getScriptState();
				_storedState.setType(kTypeVoid);
				break;

			default:
				throw Common::Exception("NCSFile::callEngine(): Invalid argument type %d",
				                        param.getType());
				break;
		}

	}

	debugC(1, kDebugScripts, "NWScript engine function %s (%d)",
	       ctx.getName().c_str(), function);
	FunctionMan.call(function, ctx);

	Variable &retVal = ctx.getReturn();
	switch (retVal.getType()) {
		case kTypeVoid:
			break;

		case kTypeInt:
		case kTypeFloat:
		case kTypeString:
		case kTypeObject:
		case kTypeEngineType:
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
}

void NCSFile::o_action(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_action(): Illegal type %d", type);

	uint16 routineNumber = _script->readUint16BE();
	uint8  argCount      = _script->readByte();

	Aurora::NWScript::FunctionContext ctx = FunctionMan.createContext(routineNumber);

	try {
		callEngine(ctx, routineNumber, argCount);
	} catch (Common::Exception &e) {
		e.add("Failed running engine function \"%s\" (%d)",
		      ctx.getName().c_str(), routineNumber);
		throw;
	}
}

void NCSFile::o_logand(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logand(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_logor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 || arg2);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_incor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_incor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 | arg2);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_excor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_excor(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 ^ arg2);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_booland(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_booland(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg1 && arg2);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
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
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		case kInstTypeFloatFloat:
			try {
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 >= arg1);
			} catch (Common::Exception &UNUSED(e)) {
				throw;
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
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		case kInstTypeFloatFloat:
			try {
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 > arg1);
			} catch (Common::Exception &UNUSED(e)) {
				throw;
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
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		case kInstTypeFloatFloat:
			try {
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 < arg1);
			} catch (Common::Exception &UNUSED(e)) {
				throw;
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
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		case kInstTypeFloatFloat:
			try {
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 <= arg1);
			} catch (Common::Exception &UNUSED(e)) {
				throw;
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
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_shright(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_shright(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();
		_stack.push(arg2 >> arg1);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
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
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_mod(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_mod(): Illegal type %d", type);

	try {
		int32 arg1 = _stack.pop().getInt();
		int32 arg2 = _stack.pop().getInt();

		if (arg1 == 0)
			throw Common::Exception("NCSFile::o_mod(): Modulus by zero");
		else if (arg1 < 0 || arg2 < 0)
			throw Common::Exception("NCSFile::o_mod(): Modulus by negative number (%d %% %d)", arg2, arg1);

		_stack.push(arg2 % arg1);
	} catch (Common::Exception &UNUSED(e)) {
		throw;
	}
}

void NCSFile::o_neg(InstructionType type) {
	switch (type) {
		case kInstTypeInt:
			try {
				_stack.push(-_stack.pop().getInt());
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		case kInstTypeFloat:
			try {
				_stack.push(-_stack.pop().getFloat());
			} catch (Common::Exception &UNUSED(e)) {
				throw;
			}
			break;

		default:
			throw Common::Exception("NCSFile::o_neg(): Illegal type %d", type);
	}
}

void NCSFile::o_comp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_comp(): Illegal type %d", type);

	try {
		_stack.push(~_stack.pop().getInt());
	} catch (Common::Exception &UNUSED(e)) {
		throw;
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
	_script->skip(offset - 6);
}

void NCSFile::o_jz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jz(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	if (!_stack.pop().getInt())
		_script->skip(offset - 6);
}

void NCSFile::o_not(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_not(): Illegal type %d", type);

	_stack.push(!_stack.pop().getInt());
}

void NCSFile::o_decsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decsp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	_stack.setRelSP(offset, _stack.getRelSP(offset).getInt() - 1);
}

void NCSFile::o_incsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incsp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	_stack.setRelSP(offset, _stack.getRelSP(offset).getInt() + 1);
}

void NCSFile::o_jnz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jnz(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	if (_stack.pop().getInt())
		_script->skip(offset - 6);
}

void NCSFile::o_decbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decbp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	_stack.setRelBP(offset, _stack.getRelBP(offset).getInt() - 1);
}

void NCSFile::o_incbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incbp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	_stack.setRelBP(offset, _stack.getRelBP(offset).getInt() + 1);
}

void NCSFile::o_savebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_savebp(): Illegal type %d", type);

	_stack.push(_stack.getBasePtr());
	_stack.setBasePtr(_stack.getStackPtr());
}

void NCSFile::o_restorebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_restorebp(): Illegal type %d", type);

	_stack.setBasePtr(_stack.pop().getInt());
}

void NCSFile::o_nop(InstructionType UNUSED(type)) {
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
		_stack.setRelSP(offset, _stack.getRelSP(startPos));

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
		_stack.push(_stack.getRelSP(offset));

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

			if (op2.getInt() == 0)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

			if (op1.getInt() == INT32_MIN && op2.getInt() == -1)
				throw Common::Exception("NCSFile::o_div: Quotient overflow");

			_stack.push((int32) (op1.getInt() / op2.getInt()));
			break;
		}

		case kInstTypeFloatFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			if (op2.getFloat() == 0.0f)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

			_stack.push(op1.getFloat() / op2.getFloat());
			break;
		}

		case kInstTypeIntFloat: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			if (op2.getFloat() == 0.0f)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

			_stack.push(((float) op1.getInt()) / op2.getFloat());
			break;
		}

		case kInstTypeFloatInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			if (op2.getInt() == 0)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

			_stack.push(op1.getFloat() / ((float) op2.getInt()));
			break;
		}

		case kInstTypeVectorFloat: {
			Variable op2  = _stack.pop();
			Variable op1z = _stack.pop();
			Variable op1y = _stack.pop();
			Variable op1x = _stack.pop();

			if (op2.getFloat() == 0.0f)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

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

			if (op2x.getFloat() == 0.0f || op2y.getFloat() == 0.0f || op2z.getFloat() == 0.0f)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

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
	uint8  offset = (uint8) type;

	// TODO: NCSFile::o_storestateall(): See o_storestate.
	//       Supposedly obsolete. Whether it's used anywhere remains to be seen.
	warning("TODO: NCSFile::o_storestateall(): %d", offset);
}

void NCSFile::o_jsr(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jsr(): Illegal type %d", type);

	int32 offset = _script->readSint32BE();

	// Push the current script position
	_returnOffsets.push(_script->pos());

	_script->skip(offset - 6);
}

void NCSFile::o_retn(InstructionType UNUSED(type)) {
	uint32 returnAddress = _script->size();
	if (!_returnOffsets.empty()) {
		returnAddress = _returnOffsets.top();
		_returnOffsets.pop();
	}

	_script->seek(returnAddress);
}

void NCSFile::o_destruct(InstructionType UNUSED(type)) {
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

	for (std::vector<Variable>::reverse_iterator t = tmp.rbegin(); t != tmp.rend(); ++t)
		_stack.push(*t);
}

void NCSFile::o_cpdownbp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cpdownbp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE() - 4;
	int16 size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cpdownbp(): Illegal size %d", size);

	int32 startPos = -size;
	while (size > 0) {
		_stack.setRelBP(offset, _stack.getRelSP(startPos));

		startPos += 4;
		offset   += 4;
		size     -= 4;
	}
}

void NCSFile::o_cptopbp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cptopbp(): Illegal type %d", type);

	int32 offset = _script->readSint32BE() - 4;
	int16 size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cptopbp(): Illegal size %d", size);

	while (size > 0) {
		_stack.push(_stack.getRelBP(offset));

		size   -= 4;
		offset += 4;
	}
}

void NCSFile::o_storestate(InstructionType type) {
	uint8  offset = (uint8) type;
	uint32 sizeBP = _script->readUint32BE();
	uint32 sizeSP = _script->readUint32BE();

	if ((sizeBP % 4) != 0)
		throw Common::Exception("NCSFile::o_storestate(): Illegal BP size %d", sizeBP);
	if ((sizeSP % 4) != 0)
		throw Common::Exception("NCSFile::o_storestate(): Illegal SP size %d", sizeSP);

	_storedState.setType(kTypeScriptState);
	ScriptState &state = _storedState.getScriptState();

	state.offset = _script->pos() - 10 + offset;

	sizeBP /= 4;
	sizeSP /= 4;

	for (int32 posBP = -4; sizeBP > 0; sizeBP--, posBP -= 4)
		state.globals.push_back(_stack.getRelBP(posBP));

	for (int32 posSP = -4; sizeSP > 0; sizeSP--, posSP -= 4)
		state.locals.push_back(_stack.getRelSP(posSP));
}

} // End of namespace NWScript

} // End of namespace Aurora
