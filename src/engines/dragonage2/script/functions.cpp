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
 *  Dragon Age II engine functions.
 */

#include <cassert>
#include <cstdlib>

#include <functional>

#include "src/common/util.h"

#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/util.h"

#include "src/engines/dragonage2/types.h"
#include "src/engines/dragonage2/game.h"
#include "src/engines/dragonage2/objectcontainer.h"
#include "src/engines/dragonage2/object.h"

#include "src/engines/dragonage2/script/functions.h"

#include "src/engines/dragonage2/script/function_tables.h"

namespace Engines {

namespace DragonAge2 {

Functions::Functions(Game &game) : _game(&game) {
	registerFunctions();
}

Functions::~Functions() {
	FunctionMan.clear();
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

void Functions::unimplementedFunction(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: %s %s(%s)", Aurora::NWScript::formatType(ctx.getReturn().getType()).c_str(),
	                           ctx.getName().c_str(), Aurora::NWScript::formatParams(ctx).c_str());
}

Common::UString Functions::formatFloat(float f, int width, int decimals) {
	return Common::UString::format("%*.*f", width, decimals, f);
}

Aurora::NWScript::Object *Functions::getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n) {
	DragonAge2::Object *object = DragonAge2::ObjectContainer::toObject(ctx.getParams()[n].getObject());
	if (!object || (object->getType() == kObjectTypeInvalid))
		return 0;

	if (object->getType() == kObjectTypeSelf)
		return ctx.getCaller();

	return object;
}

void Functions::jumpTo(DragonAge2::Object *object, float x, float y, float z) {
	object->setPosition(x, y, z);
}

} // End of namespace DragonAge2

} // End of namespace Engines
