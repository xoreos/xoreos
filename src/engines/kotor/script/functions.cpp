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
 *  Star Wars: Knights of the Old Republic engine functions.
 */

#include <cassert>

#include <functional>

#include "src/common/util.h"

#include "src/aurora/nwscript/functionman.h"

#include "src/engines/kotor/game.h"

#include "src/engines/kotor/script/functions.h"
#include "src/engines/kotor/script/function_tables.h"

namespace Engines {

namespace KotOR {

Functions::Functions(Game &game) : KotORBase::Functions(game) {
	registerFunctions();
}

void Functions::registerFunctions() {
	assert(ARRAYSIZE(kFunctionPointers) == ARRAYSIZE(kFunctionSignatures));
	assert(ARRAYSIZE(kFunctionPointers) == ARRAYSIZE(kFunctionDefaults));

	for (size_t i = 0; i < ARRAYSIZE(kFunctionPointers); i++) {
		const FunctionPointer   &fPtr = kFunctionPointers[i];
		const FunctionSignature &fSig = kFunctionSignatures[i];
		const FunctionDefaults  &fDef = kFunctionDefaults[i];

		const uint32_t id = fPtr.id;

		assert((fSig.id == id) && (fDef.id == id));

		Aurora::NWScript::Signature signature;
		signature.push_back(fSig.returnType);
		for (size_t j = 0; j < ARRAYSIZE(fSig.parameters); j++) {
			if (fSig.parameters[j] == kTypeVoid)
				break;

			signature.push_back(fSig.parameters[j]);
		}

		Aurora::NWScript::Parameters defaults;
		for (size_t j = 0; j < ARRAYSIZE(fDef.defaults); j++) {
			if (!fDef.defaults[j])
				break;

			defaults.push_back(Aurora::NWScript::Variable(*fDef.defaults[j]));
		}

		const funcPtr f = fPtr.func ? fPtr.func : &Functions::unimplementedFunction;

		FunctionMan.registerFunction(fPtr.name, id, std::bind(f, this, std::placeholders::_1), signature, defaults);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
