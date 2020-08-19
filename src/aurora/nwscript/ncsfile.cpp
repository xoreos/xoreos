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

#include <cassert>

#include <boost/make_shared.hpp>

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

static const uint32_t kNCSTag    = MKTAG('N', 'C', 'S', ' ');
static const uint32_t kVersion10 = MKTAG('V', '1', '.', '0');

static const uint32_t kScriptObjectSelf        = 0x00000000;
static const uint32_t kScriptObjectInvalid     = 0x00000001;
static const uint32_t kScriptObjectInvalid2    = 0xFFFFFFFF;
static const uint32_t kScriptObjectTypeInvalid = 0x7F000000;

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

	if (_stackPtr == (int32_t)size() - 1)
		push_back(obj);
	else
		at(_stackPtr + 1) = obj;

	_stackPtr++;
}

Variable &NCSStack::getRelSP(int32_t pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::get(): Illegal position %d", pos);

	int32_t stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::get(): Position %d below the bottom", pos);

	return at(stackPos);
}

void NCSStack::setRelSP(int32_t pos, const Variable &obj) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::set(): Illegal position %d", pos);

	int32_t stackPos = _stackPtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::set(): Position %d below the bottom", pos);

	at(stackPos) = obj;
}

Variable &NCSStack::getRelBP(int32_t pos) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::get(): Illegal position %d", pos);

	int32_t stackPos = _basePtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::get(): Position %d below the bottom", pos);

	return at(stackPos);
}

void NCSStack::setRelBP(int32_t pos, const Variable &obj) {
	if ((pos > -4) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::set(): Illegal position %d", pos);

	int32_t stackPos = _basePtr - ((pos / -4) - 1);
	if (stackPos < 0)
		throw Common::Exception("NCSStack::set(): Position %d below the bottom", pos);

	at(stackPos) = obj;
}

int32_t NCSStack::getStackPtr() {
	return (_stackPtr + 1) * -4;
}

void NCSStack::setStackPtr(int32_t pos) {
	if ((pos > 0) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setStackPtr(): Illegal position %d", pos);

	_stackPtr = (pos / -4) - 1;

	if ((int32_t)size() < (_stackPtr + 1))
		resize(_stackPtr + 1);
}

int32_t NCSStack::getBasePtr() {
	return (_basePtr + 1) * -4;
}

void NCSStack::setBasePtr(int32_t pos) {
	if ((pos > 0) || ((pos % 4) != 0))
		throw Common::Exception("NCSStack::setBasePtr(): Illegal position %d", pos);

	_basePtr = (pos / -4) - 1;
}

void NCSStack::print() const {
	if (!DebugMan.isEnabled(kDebugScripts, 3))
		return;

	debugC(kDebugScripts, 3, ".--- %d ---.", _stackPtr);
	for (int32_t i = _stackPtr; i >= 0; i--) {
		const Variable &var = at(i);

		Common::UString str;
		formatVariable(str, var);

		debugC(kDebugScripts, 3, "| %04d: %02d - %s", i, var.getType(), str.c_str());
	}
	debugC(kDebugScripts, 3, "'--- ---'");
}


#define OPCODE(x) { &NCSFile::x, #x }
#define OPCODE0() { 0, "" }

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
		OPCODE(o_nop),
		OPCODE0(),
		OPCODE0(),
		// 0x30
		OPCODE(o_writearray),
		OPCODE0(),
		OPCODE(o_readarray),
		OPCODE0(),
		// 0x34
		OPCODE0(),
		OPCODE0(),
		OPCODE0(),
		OPCODE(o_getref),
		// 0x38
		OPCODE0(),
		OPCODE(o_getrefarray)
	};

	_opcodes = opcodes;
	_opcodeListSize = ARRAYSIZE(opcodes);
}

#undef OPCODE

NCSFile::NCSFile(Common::SeekableReadStream *ncs) : _script(ncs) {
	assert(_script);

	load();
}

NCSFile::NCSFile(const Common::UString &ncs) : _name(ncs) {
	_script.reset(ResMan.getResource(ncs, kFileTypeNCS));
	if (!_script)
		throw Common::Exception("No such NCS \"%s\"", ncs.c_str());

	load();
}

NCSFile::~NCSFile() {
}

const Common::UString &NCSFile::getName() const {
	return _name;
}

VariableContainer &NCSFile::getEnvironment() {
	return _env;
}

void NCSFile::setEnvironment(const VariableContainer &env) {
	_env = env;
}

void NCSFile::setParameters(std::vector<int> parameters) {
	_parameters = parameters;
}

int NCSFile::getParameter(size_t index) const {
	if (_parameters.size() >= index)
		return 0;

	return _parameters[index];
}

void NCSFile::setParameterString(const Common::UString &parameterString) {
	_parameterString = parameterString;
}

const Common::UString &NCSFile::getParameterString() const {
	return _parameterString;
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

	uint32_t length = _script->readUint32BE();
	if (length > ((uint32_t) _script->size()))
		throw Common::Exception("Script size %u > stream size %u", length, (uint)_script->size());
	if (length < ((uint32_t) _script->size()))
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
	return run(getEmptyState(), ObjectReference(owner), ObjectReference(triggerer));
}

const Variable &NCSFile::run(const ObjectReference owner, const ObjectReference triggerer) {
	return run(getEmptyState(), ObjectReference(owner), ObjectReference(triggerer));
}

const Variable &NCSFile::run(const ScriptState &state, Object *owner, Object *triggerer) {
	return run(state, ObjectReference(owner), ObjectReference(triggerer));
}

const Variable &NCSFile::run(const ScriptState &state, const ObjectReference owner, const ObjectReference triggerer) {
	debugC(kDebugScripts, 1, "=== Running script \"%s\" (%d) ===",
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

const Variable &NCSFile::execute(const ObjectReference owner, const ObjectReference triggerer) {
	_owner     = owner;
	_triggerer = triggerer;

	while (executeStep())
		;

	if (!_stack.empty())
		_return = _stack.top();

	if (!_stack.empty() && (_stack.top().getType() == kTypeInt))
		debugC(kDebugScripts, 1, "=> Script\"%s\" returns: %d",
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

	if ((opcode >= _opcodeListSize) || (!_opcodes[opcode].proc))
		throw Common::Exception("NCSFile::executeStep(): Illegal instruction 0x%02x", opcode);

	debugC(kDebugScripts, 1, "NWScript opcode %s [0x%02X]", _opcodes[opcode].desc, opcode);

	(this->*(_opcodes[opcode].proc))((InstructionType)type);

	_stack.print();
	debugC(kDebugScripts, 2, "[RETURN: %d]",
	       _returnOffsets.empty() ? -1 : _returnOffsets.top());

	return true;
}

void NCSFile::decompile() {
	uint32_t oldScriptPos = _script->pos();
	_script->seek(13); // 8 byte header + 5 byte program size dummy op

	// TODO

	_script->seek(oldScriptPos);
}

// OPCODES!

/** RSADD: push an empty variable onto the stack. */
void NCSFile::o_rsadd(InstructionType type) {
	switch (type) {
		case kInstTypeInt:
			_stack.push(kTypeInt);
			break;
		case kInstTypeFloat:
			_stack.push(kTypeFloat);
			break;
		case kInstTypeString:
		case kInstTypeResource:
			_stack.push(kTypeString);
			break;
		case kInstTypeObject:
			_stack.push(kTypeObject);
			break;
		case kInstTypeEngineType0:
		case kInstTypeEngineType1:
		case kInstTypeEngineType2:
		case kInstTypeEngineType3:
		case kInstTypeEngineType4:
		case kInstTypeEngineType5:
			_stack.push(kTypeEngineType);
			break;
		case kInstTypeIntArray:
		case kInstTypeFloatArray:
		case kInstTypeStringArray:
		case kInstTypeObjectArray:
		case kInstTypeResourceArray:
		case kInstTypeEngineType0Array:
		case kInstTypeEngineType1Array:
		case kInstTypeEngineType2Array:
		case kInstTypeEngineType3Array:
		case kInstTypeEngineType4Array:
		case kInstTypeEngineType5Array:
			_stack.push(kTypeArray);
			break;
		default:
			throw Common::Exception("NCSFile::o_rsadd(): Illegal type %d", type);
	}
}

/** CONST: push a constant (predetermined value) variable onto the stack. */
void NCSFile::o_const(InstructionType type) {
	switch (type) {
		case kInstTypeInt:
			_stack.push(_script->readSint32BE());
			break;

		case kInstTypeFloat:
			_stack.push(_script->readIEEEFloatBE());
			break;

		case kInstTypeString:
		case kInstTypeResource: {
			_stack.push(Common::readStringFixed(*_script, Common::kEncodingASCII, _script->readUint16BE()));
			break;
		}

		case kInstTypeObject: {
			/* The scripts only know of two constant objects:
			 * - OBJECT_SELF, the owner object of the script
			 * - OBJECT_INVALID, an invalid object
			 *
			 * In the bytecode, the latter can be initialized by a few different
			 * magic values. They *should* all have the same effect, though.
			 */

			uint32_t objectID = _script->readUint32BE();

			if      (objectID == kScriptObjectSelf)
				_stack.push(_owner);
			else if (objectID == kScriptObjectInvalid)
				_stack.push((Object *) 0);
			else if (objectID == kScriptObjectInvalid2)
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

/** Helper function for o_action(), doing the actual engine function calling. */
void NCSFile::callEngine(Aurora::NWScript::FunctionContext &ctx,
                         uint32_t function, uint8_t argCount) {

	if ((argCount < ctx.getParamMin()) || (argCount > ctx.getParamMax()))
		throw Common::Exception("NCSFile::callEngine(): Argument count mismatch (%u vs %u - %u)",
		                        argCount, (uint)ctx.getParamMin(), (uint)ctx.getParamMax());

	ctx.setCurrentScript(this);
	ctx.setCaller(_owner);
	ctx.setTriggerer(_triggerer);

	// Pop parameters
	ctx.setParamsSpecified(argCount);
	for (uint8_t i = 0; i < argCount; i++) {
		Variable &param = ctx.getParams()[i];

		Type type = param.getType();
		if (type == kTypeAny)
			type = _stack.top().getType();

		switch (type) {
			case kTypeInt:
			case kTypeFloat:
			case kTypeString:
			case kTypeObject:
			case kTypeEngineType:
			case kTypeReference:
			case kTypeArray:
				param = _stack.pop();
				break;

			case kTypeVector: {
				// A vector is held as three floats on the stack

				float z = _stack.pop().getFloat();
				float y = _stack.pop().getFloat();
				float x = _stack.pop().getFloat();

				param.setVector(x, y, z);
				break;
			}

			case kTypeScriptState:
				// The script state, "action" type, isn't stored on the stack at all

				param.getScriptState() = _storedState.getScriptState();
				_storedState.setType(kTypeVoid);
				break;

			default:
				throw Common::Exception("NCSFile::callEngine(): Invalid argument type %d",
				                        param.getType());
				break;
		}

	}

	// Call the engine function
	debugC(kDebugScripts, 1, "NWScript engine function %s (%d)", ctx.getName().c_str(), function);
	FunctionMan.call(function, ctx);

	// Push return values
	Variable &retVal = ctx.getReturn();
	switch (retVal.getType()) {
		case kTypeVoid:
			break;

		case kTypeInt:
		case kTypeFloat:
		case kTypeString:
		case kTypeObject:
		case kTypeEngineType:
		case kTypeArray:
			_stack.push(retVal);
			break;

		case kTypeVector: {
			// A vector is held as three floats on the stack

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

/** ACTION: call a game-specific engine function. */
void NCSFile::o_action(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_action(): Illegal type %d", type);

	uint16_t routineNumber = _script->readUint16BE();
	uint8_t  argCount      = _script->readByte();

	Aurora::NWScript::FunctionContext ctx = FunctionMan.createContext(routineNumber);

	try {
		callEngine(ctx, routineNumber, argCount);
	} catch (Common::Exception &e) {
		e.add("Failed running engine function \"%s\" (%d)",
		      ctx.getName().c_str(), routineNumber);
		throw;
	}
}

/** LOGAND: perform a logical boolean AND (&&). */
void NCSFile::o_logand(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logand(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg1 && arg2);
}

/** LOGOR: perform a logical boolean OR (||). */
void NCSFile::o_logor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_logor(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg1 || arg2);
}

/** INCOR: perform a bit-wise inclusive OR (|). */
void NCSFile::o_incor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_incor(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg1 | arg2);
}

/** EXCOR: perform a bit-wise exclusive OR (^). */
void NCSFile::o_excor(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_excor(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg1 ^ arg2);
}

/** BOOLAND: perform a bit-wise AND (&). */
void NCSFile::o_booland(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_booland(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg1 & arg2);
}

/** EQ: compare the top-most stack elements for equality (==). */
void NCSFile::o_eq(InstructionType type) {
	size_t n = 1;

	if (type == kInstTypeStructStruct) {
		// Comparisons between two structs (or two vectors) come with the size of the type

		const size_t size = _script->readUint16BE();

		if ((size % 4) != 0)
			throw Common::Exception("NCSFile::o_eq(): size %% 4 != 0");

		n = size / 4;
	}

	std::vector<Variable> args1, args2;

	args1.reserve(n);
	args2.reserve(n);

	for (size_t i = 0; i < n; i++)
		args1.push_back(_stack.pop());

	for (size_t i = 0; i < n; i++)
		args2.push_back(_stack.pop());

	_stack.push(args1 == args2);
}

/** NEQ: compare the top-most stack elements for inequality (!=). */
void NCSFile::o_neq(InstructionType type) {
	size_t n = 1;

	if (type == kInstTypeStructStruct) {
		// Comparisons between two structs (or two vectors) come with the size of the type

		const size_t size = _script->readUint16BE();

		if ((size % 4) != 0)
			throw Common::Exception("NCSFile::o_neq(): size %% 4 != 0");

		n = size / 4;
	}

	std::vector<Variable> args1, args2;

	args1.reserve(n);
	args2.reserve(n);

	for (size_t i = 0; i < n; i++)
		args1.push_back(_stack.pop());

	for (size_t i = 0; i < n; i++)
		args2.push_back(_stack.pop());

	_stack.push(args1 != args2);
}

/** GEQ: compare the top-most stack elements, greater-or-equal (>=). */
void NCSFile::o_geq(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt:
			{
				int32_t arg1 = _stack.pop().getInt();
				int32_t arg2 = _stack.pop().getInt();
				_stack.push(arg2 >= arg1);
			}
			break;

		case kInstTypeFloatFloat:
			{
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 >= arg1);
			}
			break;

		default:
			throw Common::Exception("NCSFile::o_geq(): Illegal type %d", type);
	}
}

/** GT: compare the top-most stack elements, greater (>). */
void NCSFile::o_gt(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt:
			{
				int32_t arg1 = _stack.pop().getInt();
				int32_t arg2 = _stack.pop().getInt();
				_stack.push(arg2 > arg1);
			}
			break;

		case kInstTypeFloatFloat:
			{
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 > arg1);
			}
			break;

		default:
			throw Common::Exception("NCSFile::o_gt(): Illegal type %d", type);
	}
}

/** LT: compare the top-most stack elements, less (<). */
void NCSFile::o_lt(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt:
			{
				int32_t arg1 = _stack.pop().getInt();
				int32_t arg2 = _stack.pop().getInt();
				_stack.push(arg2 < arg1);
			}
			break;

		case kInstTypeFloatFloat:
			{
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 < arg1);
			}
			break;

		default:
			throw Common::Exception("NCSFile::o_lt(): Illegal type %d", type);
	}
}

/** LEQ: compare the top-most stack elements, less-or-equal (<=). */
void NCSFile::o_leq(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt:
			{
				int32_t arg1 = _stack.pop().getInt();
				int32_t arg2 = _stack.pop().getInt();
				_stack.push(arg2 <= arg1);
			}
			break;

		case kInstTypeFloatFloat:
			{
				float arg1 = _stack.pop().getFloat();
				float arg2 = _stack.pop().getFloat();
				_stack.push(arg2 <= arg1);
			}
			break;

		default:
			throw Common::Exception("NCSFile::o_leq(): Illegal type %d", type);
	}
}

/** SHLEFT: shift the top-most stack element to the left (<<). */
void NCSFile::o_shleft(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_shleft(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg2 << arg1);
}

/** SHRIGHT: signed-shift the top-most stack element to the right (>>>). */
void NCSFile::o_shright(InstructionType type) {
	/* According to Skywing's NWNScriptLib
	 * (<https://github.com/SkywingvL/nwn2dev-public/blob/master/NWNScriptLib/NWScriptVM.cpp#L2233>):
	 * "The operation implemented here is actually a complex sequence that, if
	 *  the amount to be shifted is negative, involves both a front-loaded and
	 *  end-loaded negate built on top of a signed shift." */

	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_shright(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();

	if (arg2 < 0) {
		arg2 = -arg2;
		_stack.push(-(arg2 >> arg1));
	} else
		_stack.push(arg2 >> arg1);
}

/** USHRIGHT: shift the top-most stack element to the right (>>). */
void NCSFile::o_ushright(InstructionType type) {
	/* According to Skywing's NWNScriptLib
	 * (<https://github.com/SkywingvL/nwn2dev-public/blob/master/NWNScriptLib/NWScriptVM.cpp#L2272>):
	 * "While this operator may have originally been intended to implement
	 *  an unsigned shift, it actually performs an arithmetic (signed) shift." */

	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_ushright(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();
	_stack.push(arg2 >> arg1);
}

/** MOD: calculate the remainder (modulo) of an integer division (%). */
void NCSFile::o_mod(InstructionType type) {
	if (type != kInstTypeIntInt)
		throw Common::Exception("NCSFile::o_mod(): Illegal type %d", type);

	int32_t arg1 = _stack.pop().getInt();
	int32_t arg2 = _stack.pop().getInt();

	if (arg1 == 0)
		throw Common::Exception("NCSFile::o_mod(): Modulus by zero");
	else if (arg1 < 0 || arg2 < 0)
		throw Common::Exception("NCSFile::o_mod(): Modulus by negative number (%d %% %d)", arg2, arg1);

	_stack.push(arg2 % arg1);
}

/** NEQ: negate the top-most stack element (unary -). */
void NCSFile::o_neg(InstructionType type) {
	switch (type) {
		case kInstTypeInt:
			_stack.push(-_stack.pop().getInt());
			break;

		case kInstTypeFloat:
			_stack.push(-_stack.pop().getFloat());
			break;

		default:
			throw Common::Exception("NCSFile::o_neg(): Illegal type %d", type);
	}
}

/** COMP: calculate the 1-complement of the top-most stack element (~). */
void NCSFile::o_comp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_comp(): Illegal type %d", type);

	_stack.push(~_stack.pop().getInt());
}

/** MOVSP: pop elements off the stack. */
void NCSFile::o_movsp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_movsp(): Illegal type %d", type);

	_stack.setStackPtr(_stack.getStackPtr() - _script->readSint32BE());
}

/** JMP: jump directly to a different script offset. */
void NCSFile::o_jmp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jmp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	_script->skip(offset - 6);
}

/** JZ: jump conditionally if the top-most stack element is 0. */
void NCSFile::o_jz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jz(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	if (!_stack.pop().getInt())
		_script->skip(offset - 6);
}

/** NOT: boolean-negate the top-most stack element (!). */
void NCSFile::o_not(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_not(): Illegal type %d", type);

	_stack.push(!_stack.pop().getInt());
}

/** DECSP: decrement the value of a stack element (--). */
void NCSFile::o_decsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decsp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	_stack.setRelSP(offset, _stack.getRelSP(offset).getInt() - 1);
}

/** INCSP: increment the value of a stack element (++). */
void NCSFile::o_incsp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incsp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	_stack.setRelSP(offset, _stack.getRelSP(offset).getInt() + 1);
}

/** JNZ: jump conditionally if the top-most stack element is not 0. */
void NCSFile::o_jnz(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jnz(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	if (_stack.pop().getInt())
		_script->skip(offset - 6);
}

/** DECBP: decrement the value of a base-pointer stack element (--). */
void NCSFile::o_decbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_decbp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	_stack.setRelBP(offset, _stack.getRelBP(offset).getInt() - 1);
}

/** INCBP: increment the value of a base-pointer stack element (++). */
void NCSFile::o_incbp(InstructionType type) {
	if (type != kInstTypeInt)
		throw Common::Exception("NCSFile::o_incbp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	_stack.setRelBP(offset, _stack.getRelBP(offset).getInt() + 1);
}

/** SAVEBP: set the value of the base-pointer.
 *
 *  Used to create an anchor point to access global variables.
 */
void NCSFile::o_savebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_savebp(): Illegal type %d", type);

	_stack.push(_stack.getBasePtr());
	_stack.setBasePtr(_stack.getStackPtr());
}

/** RESTOREBP: restore the value of the base-pointer to a prior value.
 *
 *  Destroy the global variables anchor point after use.
 */
void NCSFile::o_restorebp(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_restorebp(): Illegal type %d", type);

	_stack.setBasePtr(_stack.pop().getInt());
}

/** NOP: no operation. */
void NCSFile::o_nop(InstructionType UNUSED(type)) {
	// Nothing! Yay!
}

/** CPDOWNSP: copy a value into an existing stack element. */
void NCSFile::o_cpdownsp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cpdownsp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cpdownsp(): Illegal size %d", size);

	int32_t startPos = -size;
	while (size > 0) {
		_stack.setRelSP(offset, _stack.getRelSP(startPos));

		startPos += 4;
		offset   += 4;
		size     -= 4;
	}
}

/** CPTOPSP: push a copy of a stack element on top of the stack. */
void NCSFile::o_cptopsp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cptopsp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cptopsp(): Illegal size %d", size);

	while (size > 0) {
		_stack.push(_stack.getRelSP(offset));

		size -= 4;
	}
}

/** ADD: add the top-most stack elements (+). */
void NCSFile::o_add(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32_t) (op1.getInt() + op2.getInt()));
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

/** SUB: subtract the top-most stack elements (-). */
void NCSFile::o_sub(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32_t) (op1.getInt() - op2.getInt()));
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

/** MUL: multiply the top-most stack elements (*). */
void NCSFile::o_mul(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			_stack.push((int32_t) (op1.getInt() * op2.getInt()));
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

/** DIV: divide the top-most stack elements (/). */
void NCSFile::o_div(InstructionType type) {
	switch (type) {
		case kInstTypeIntInt: {
			Variable op2 = _stack.pop();
			Variable op1 = _stack.pop();

			if (op2.getInt() == 0)
				throw Common::Exception("NCSFile::o_div(): Divide by zero");

			if (op1.getInt() == INT32_MIN && op2.getInt() == -1)
				throw Common::Exception("NCSFile::o_div: Quotient overflow");

			_stack.push((int32_t) (op1.getInt() / op2.getInt()));
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

/** STORESTATEALL: unused, obsolete opcode. Hopefully. */
void NCSFile::o_storestateall(InstructionType type) {
	uint8_t  offset = (uint8_t) type;

	// TODO: NCSFile::o_storestateall(): See o_storestate.
	//       Supposedly obsolete. Whether it's used anywhere remains to be seen.
	warning("TODO: NCSFile::o_storestateall(): %d", offset);
}

/** JSR: call a subroutine. */
void NCSFile::o_jsr(InstructionType type) {
	if (type != kInstTypeNone)
		throw Common::Exception("NCSFile::o_jsr(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();

	// Push the current script position
	_returnOffsets.push(_script->pos());

	_script->skip(offset - 6);
}

/** RETN: return from a subroutine call. */
void NCSFile::o_retn(InstructionType UNUSED(type)) {
	uint32_t returnAddress = _script->size();
	if (!_returnOffsets.empty()) {
		returnAddress = _returnOffsets.top();
		_returnOffsets.pop();
	}

	_script->seek(returnAddress);
}

/** DESTRUCT: remove elements from the stack.
 *
 *  Used to isolate struct elements.
 */
void NCSFile::o_destruct(InstructionType UNUSED(type)) {
	int16_t stackSize        = _script->readSint16BE();
	int16_t dontRemoveOffset = _script->readSint16BE();
	int16_t dontRemoveSize   = _script->readSint16BE();

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

/** CPDOWNBP: copy a value into an existing base-pointer stack element.
 *
 *  Used to write into a global variable.
 */
void NCSFile::o_cpdownbp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cpdownbp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE() - 4;
	int16_t size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cpdownbp(): Illegal size %d", size);

	int32_t startPos = -size;
	while (size > 0) {
		_stack.setRelBP(offset, _stack.getRelSP(startPos));

		startPos += 4;
		offset   += 4;
		size     -= 4;
	}
}

/** CPTOPBP: push a copy of a base-pointer stack element on top of the stack.
 *
 *  Used to read from a global variable.
 */
void NCSFile::o_cptopbp(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_cptopbp(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE() - 4;
	int16_t size   = _script->readSint16BE();

	if ((size % 4) != 0)
		throw Common::Exception("NCSFile::o_cptopbp(): Illegal size %d", size);

	while (size > 0) {
		_stack.push(_stack.getRelBP(offset));

		size   -= 4;
		offset += 4;
	}
}

/** STORESTATE: create a functor of a subroutine with the current stack.
 *
 *  Used to create the "action" variables when calling an engine function that
 *  assigns a function to an object, or delays a function, or similar.
 */
void NCSFile::o_storestate(InstructionType type) {
	uint8_t  offset = (uint8_t) type;
	uint32_t sizeBP = _script->readUint32BE();
	uint32_t sizeSP = _script->readUint32BE();

	if ((sizeBP % 4) != 0)
		throw Common::Exception("NCSFile::o_storestate(): Illegal BP size %d", sizeBP);
	if ((sizeSP % 4) != 0)
		throw Common::Exception("NCSFile::o_storestate(): Illegal SP size %d", sizeSP);

	_storedState.setType(kTypeScriptState);
	ScriptState &state = _storedState.getScriptState();

	state.offset = _script->pos() - 10 + offset;

	sizeBP /= 4;
	sizeSP /= 4;

	for (int32_t posBP = -4; sizeBP > 0; sizeBP--, posBP -= 4)
		state.globals.push_back(_stack.getRelBP(posBP));

	for (int32_t posSP = -4; sizeSP > 0; sizeSP--, posSP -= 4)
		state.locals.push_back(_stack.getRelSP(posSP));
}

/** WRITEARRAY: write the value of an array element on the stack.
 *
 *  Writes into the dynamic array variables types added with
 *  Dragon Age: Origins.
 *
 *  The index of the array to write and the value to write are at
 *  the top-most stack position; the offset to the array variable
 *  is passed as a direct argument to the instruction.
 *
 *  The index is popped off the stack, but the value written remains.
 */
void NCSFile::o_writearray(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_writearray(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if (size != 4)
		throw Common::Exception("NCSFile::o_writearray(): Invalid size %d", size);

	Variable &arrayVar = _stack.getRelSP(offset);

	Variable  indexVar = _stack.pop();
	Variable &valueVar = _stack.top();

	const int32_t index = indexVar.getInt();
	if (index < 0)
		throw Common::Exception("NCSFile::o_writearray(): Invalid index %d", index);

	arrayVar.growArray(valueVar.getType(), index + 1);
	arrayVar.getArray()[index] = boost::make_shared<Variable>(Variable(valueVar));
}

/** READARRAY: push the value of an array element onto of the stack.
 *
 *  Reads out of the dynamic array variables types added with
 *  Dragon Age: Origins.
 *
 *  The index of the array to read is at the top-most stack position;
 *  the offset to the array variable is passed as a direct argument to
 *  the instruction.
 *
 *  The index is popped off the stack, and the value read out of the
 *  array is pushed on top.
 */
void NCSFile::o_readarray(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_readarray(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if (size != 4)
		throw Common::Exception("NCSFile::o_readarray(): Invalid size %d", size);

	Variable::Array &array = _stack.getRelSP(offset).getArray();

	Variable indexVar  = _stack.pop();
	const int32_t index = indexVar.getInt();

	if ((index < 0) || ((uint)index >= array.size()))
		throw Common::Exception("NCSFile::o_readarray(): Index out of range (%d, %u)",
		                        index, (uint) array.size());

	_stack.push(*array[index]);
}

/** GETREF: push the reference to a stack element onto the stack.
 *
 *  Creates a reference to a variable, usually an engine type like
 *  Event or Effect, so that it can be modified by an engine function
 *  directly, without having to create a modified copy. Added with
 *  Dragon Age II.
 *
 *  The offset to the variable to create a reference to is passed
 *  as a direct argument to the instruction.
 */
void NCSFile::o_getref(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_getref(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if (size != 4)
		throw Common::Exception("NCSFile::o_getref(): Invalid size %d", size);

	Variable &var = _stack.getRelSP(offset);

	_stack.push(Variable(kTypeReference));
	_stack.top().setReference(&var);
}

/** GETREFARRAY: push the reference to an array element onto the stack.
 *
 *  Like GETREF, this creates a reference to a variable, usually an
 *  engine type like Event or Effect, so that it can be modified by an
 *  engine function directly, without having to create a modified copy.
 *  GETREFARRAY, however, create a reference to a variable held inside
 *  a dynamic array variable. Added with Dragon Age II.
 *
 *  The index of the array is at the top-most stack position; the
 *  offset to the array variable is passed as a direct argument to
 *  the instruction.
 *
 *  The index is popped off the stack, and the reference to the
 *  variable inside the array is pushed on top.
 */
void NCSFile::o_getrefarray(InstructionType type) {
	if (type != kInstTypeDirect)
		throw Common::Exception("NCSFile::o_getrefarray(): Illegal type %d", type);

	int32_t offset = _script->readSint32BE();
	int16_t size   = _script->readSint16BE();

	if (size != 4)
		throw Common::Exception("NCSFile::o_getrefarray(): Invalid size %d", size);

	Variable::Array &array = _stack.getRelSP(offset).getArray();

	Variable indexVar  = _stack.pop();
	const int32_t index = indexVar.getInt();

	if ((index < 0) || ((uint)index >= array.size()))
		throw Common::Exception("NCSFile::o_getrefarray(): Index out of range (%d, %u)",
		                        index, (uint) array.size());

	_stack.push(Variable(kTypeReference));
	_stack.top().setReference(&*array[index]);
}

} // End of namespace NWScript

} // End of namespace Aurora
