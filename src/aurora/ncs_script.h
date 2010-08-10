/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/ncs_script.cpp
 *  Handling BioWare's NWN Compiled Scripts
 */

#ifndef AURORA_NCS_SCRIPT_H
#define AURORA_NCS_SCRIPT_H

#include <stack>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** A stack object for NCS scripts */
class StackObject {
public:
	enum StackObjectType {
		kStackObjectNone,
		kStackObjectInt,
		kStackObjectFloat,
		kStackObjectString,
		kStackObjectObject,
		kStackObjectVector,
		kStackObjectStruct,
		kStackObjectEngineType
	};

	StackObject(StackObjectType type = kStackObjectNone);
	StackObject(int32 val, StackObjectType = kStackObjectInt);
	StackObject(float val);
	StackObject(Common::UString val);

	int32 getInt() const;
	float getFloat() const;
	Common::UString getString() const;
	uint32 getObject() const;

	bool operator==(StackObject &obj);
	bool operator!=(StackObject &obj) { return !operator==(obj); }

	StackObjectType getType() { return _type; }

private:
	StackObjectType _type;

	uint32 _intVal;
	float _floatVal;
	Common::UString _stringVal;
};

#define DECLARE_OPCODE(x) void x(InstructionType type)

/** An NCS, BioWare's NWN Compile Script */
class NCSScript : public AuroraBase {
public:
	NCSScript() { setupOpcodes(); }
	~NCSScript() {}

	void load(Common::SeekableReadStream &ncs);
	void executeStep();
	void decompile();

	enum InstructionType {
		// Unary
		kInstTypeNone = 0,
		kInstTypeInt = 3,
		kInstTypeFloat = 4,
		kInstTypeString = 5,
		kInstTypeObject = 6,
		kInstTypeEffect = 16,
		kInstTypeEvent = 17,
		kInstTypeLocation = 18,
		kInstTypeTalent = 19,

		// Binary
		kInstTypeIntInt = 32,
		kInstTypeFloatFloat = 33,
		kInstTypeObjectObject = 34,
		kInstTypeStringString = 35,
		kInstTypeStructStruct = 36,
		kInstTypeIntFloat = 37,
		kInstTypeFloatInt = 38,
		kInstTypeEffectEffect = 48,
		kInstTypeEventEvent = 49,
		kInstTypeLocationLocation = 50,
		kInstTypeTalentTalent = 51,
		kInstTypeVectorVector = 58,
		kInstTypeVectorFloat = 59,
		kInstTypeFloatVector = 60
	};

private:
	std::stack<StackObject> _stack;
	Common::SeekableReadStream *_script;

	typedef void (NCSScript::*OpcodeProc)(InstructionType type);
	struct Opcode {
		OpcodeProc proc;
		const char *desc;
	};
	const Opcode *_opcodes;
	uint32 _opcodeListSize;
	void setupOpcodes();

	// Opcode declarations
	DECLARE_OPCODE(o_nop);
	DECLARE_OPCODE(o_cpdownsp);
	DECLARE_OPCODE(o_rsadd);
	DECLARE_OPCODE(o_cptopsp);
	DECLARE_OPCODE(o_const);
	DECLARE_OPCODE(o_action);
	DECLARE_OPCODE(o_logand);
	DECLARE_OPCODE(o_logor);
	DECLARE_OPCODE(o_incor);
	DECLARE_OPCODE(o_excor);
	DECLARE_OPCODE(o_booland);
	DECLARE_OPCODE(o_eq);
	DECLARE_OPCODE(o_neq);
	DECLARE_OPCODE(o_geq);
	DECLARE_OPCODE(o_gt);
	DECLARE_OPCODE(o_lt);
	DECLARE_OPCODE(o_leq);
	DECLARE_OPCODE(o_shleft);
	DECLARE_OPCODE(o_shright);
	DECLARE_OPCODE(o_ushright);
	DECLARE_OPCODE(o_add);
	DECLARE_OPCODE(o_sub);
	DECLARE_OPCODE(o_mul);
	DECLARE_OPCODE(o_div);
	DECLARE_OPCODE(o_mod);
	DECLARE_OPCODE(o_neg);
	DECLARE_OPCODE(o_comp);
	DECLARE_OPCODE(o_movsp);
	DECLARE_OPCODE(o_storestateall);
	DECLARE_OPCODE(o_jmp);
	DECLARE_OPCODE(o_jsr);
	DECLARE_OPCODE(o_jz);
	DECLARE_OPCODE(o_retn);
	DECLARE_OPCODE(o_destruct);
	DECLARE_OPCODE(o_not);
	DECLARE_OPCODE(o_decisp);
	DECLARE_OPCODE(o_incisp);
	DECLARE_OPCODE(o_jnz);
	DECLARE_OPCODE(o_cpdownbp);
	DECLARE_OPCODE(o_cptopbp);
	DECLARE_OPCODE(o_decibp);
	DECLARE_OPCODE(o_incibp);
	DECLARE_OPCODE(o_savebp);
	DECLARE_OPCODE(o_restorebp);
	DECLARE_OPCODE(o_storestate);
};

#undef DECLARE_OPCODE

} // End of namespace Aurora

#endif // AURORA_NCS_SCRIPT_H
