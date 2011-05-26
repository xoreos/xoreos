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

/** @file aurora/nwscript/functioncontext.h
 *  Context of an NWScript function.
 */

#ifndef AURORA_NWSCRIPT_FUNCTIONCONTEXT_H
#define AURORA_NWSCRIPT_FUNCTIONCONTEXT_H

#include <vector>

#include "aurora/nwscript/types.h"
#include "aurora/nwscript/util.h"
#include "aurora/nwscript/variable.h"

namespace Aurora {

namespace NWScript {

class FunctionContext {
public:
	FunctionContext();
	FunctionContext(const FunctionContext &ctx);
	~FunctionContext();

	FunctionContext &operator=(const FunctionContext &ctx);

	void setSignature(const Signature &signature);

	const Signature &getSignature() const;

	Object *getCaller() const;
	void setCaller(Object *obj);

	Object *getTriggerer() const;
	void setTriggerer(Object *obj);

	Variable &getReturn();
	const Variable &getReturn() const;

	Parameters &getParams();
	const Parameters &getParams() const;

private:
	Signature _signature; ///< The function's signature.

	Object *_caller;    ///< The calling object.
	Object *_triggerer; ///< The triggering person.

	Variable   _return;     ///< The function's return value.
	Parameters _parameters; ///< The function's parameters.
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_FUNCTIONCONTEXT_H
