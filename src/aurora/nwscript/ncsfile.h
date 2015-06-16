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

#ifndef AURORA_NWSCRIPT_NCSFILE_H
#define AURORA_NWSCRIPT_NCSFILE_H

#include <vector>
#include <stack>

#include "src/common/types.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/variable.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Aurora {

namespace NWScript {

class NCSStack : public std::vector<Variable> {
public:
	NCSStack();
	~NCSStack();

	void reset();

	bool empty() const;

	Variable &top();
	Variable pop();
	void push(const Variable &obj);

	Variable &getRelSP(int32 pos);
	void setRelSP(int32 pos, const Variable &obj);

	Variable &getRelBP(int32 pos);
	void setRelBP(int32 pos, const Variable &obj);

	int32 getStackPtr();
	void  setStackPtr(int32 pos);

	int32 getBasePtr();
	void  setBasePtr(int32 pos);

	void print() const;

private:
	int32 _stackPtr;
	int32 _basePtr;
};

#define DECLARE_OPCODE(x) void x(InstructionType type)

/** An NCS, BioWare's NWN Compile Script. */
class NCSFile : public AuroraBase {
public:
	NCSFile(Common::SeekableReadStream *ncs);
	NCSFile(const Common::UString &ncs);
	~NCSFile();

	const Common::UString &getName() const;

	/** Run the current script, from start to finish. */
	const Variable &run(Object *owner = 0, Object *triggerer = 0);

	/** Run the current script, from this state to finish. */
	const Variable &run(const ScriptState &state, Object *owner = 0, Object *triggerer = 0);

	static ScriptState getEmptyState();

private:
	enum InstructionType {
		// Unary
		kInstTypeNone      =  0,
		kInstTypeDirect    =  1,
		kInstTypeInt       =  3,
		kInstTypeFloat     =  4,
		kInstTypeString    =  5,
		kInstTypeObject    =  6,
		kInstTypeEffect    = 16,
		kInstTypeEvent     = 17,
		kInstTypeLocation  = 18,
		kInstTypeTalent    = 19,

		// Binary
		kInstTypeIntInt           = 32,
		kInstTypeFloatFloat       = 33,
		kInstTypeObjectObject     = 34,
		kInstTypeStringString     = 35,
		kInstTypeStructStruct     = 36,
		kInstTypeIntFloat         = 37,
		kInstTypeFloatInt         = 38,
		kInstTypeEffectEffect     = 48,
		kInstTypeEventEvent       = 49,
		kInstTypeLocationLocation = 50,
		kInstTypeTalentTalent     = 51,
		kInstTypeVectorVector     = 58,
		kInstTypeVectorFloat      = 59,
		kInstTypeFloatVector      = 60
	};

	Common::UString _name;

	NCSStack _stack;
	Common::SeekableReadStream *_script;

	Variable _return;

	Object *_owner;
	Object *_triggerer;

	std::stack<uint32> _returnOffsets;

	Variable _storedState;

	typedef void (NCSFile::*OpcodeProc)(InstructionType type);
	struct Opcode {
		OpcodeProc proc;
		const char *desc;
	};
	const Opcode *_opcodes;
	size_t _opcodeListSize;
	void setupOpcodes();

	void load();

	/** Reset the script for another execution. */
	void reset();

	const Variable &execute(Object *owner = 0, Object *triggerer = 0);

	/** Execute one script step. */
	bool executeStep();

	void decompile(); // TODO

	void callEngine(Aurora::NWScript::FunctionContext &ctx, uint32 function, uint8 argCount);

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
	DECLARE_OPCODE(o_decsp);
	DECLARE_OPCODE(o_incsp);
	DECLARE_OPCODE(o_jnz);
	DECLARE_OPCODE(o_cpdownbp);
	DECLARE_OPCODE(o_cptopbp);
	DECLARE_OPCODE(o_decbp);
	DECLARE_OPCODE(o_incbp);
	DECLARE_OPCODE(o_savebp);
	DECLARE_OPCODE(o_restorebp);
	DECLARE_OPCODE(o_storestate);
};

#undef DECLARE_OPCODE

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_NCSFILE_H
