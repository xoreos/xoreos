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
 *  Neverwinter Nights engine functions.
 */

#include <cassert>

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/aurora/nwscript/functionman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"

#include "src/engines/nwn/script/functions.h"

#include "src/engines/nwn/script/function_tables.h"

namespace Engines {

namespace NWN {

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

		const uint32 id = fPtr.id;

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
			if (fDef.defaults[j].getType() == kTypeVoid)
				break;

			defaults.push_back(fDef.defaults[j]);
		}

		const funcPtr f = fPtr.func ? fPtr.func : &Functions::unimplementedFunction;

		FunctionMan.registerFunction(fPtr.name, id, boost::bind(f, this, _1), signature, defaults);
	}
}

void Functions::unimplementedFunction(Aurora::NWScript::FunctionContext &ctx) {
	warning("TODO: %s(%s)", ctx.getName().c_str(), formatParams(ctx).c_str());
}

int32 Functions::random(int min, int max, int32 n) {
	if (n < 1)
		n = 1;

	int32 r = 0;

	while (n-- > 0)
		r += std::rand() % (max - min + 1) + min;

	return r;
}

Common::UString Functions::formatTag(const Aurora::NWScript::Object *object) {
	if (!object)
		return "0";

	return "\"" + object->getTag() + "\"";
}

Common::UString Functions::formatParams(const Aurora::NWScript::FunctionContext &ctx) {
	Common::UString params;
	for (size_t i = 0; i < ctx.getParams().size(); i++) {
		if (i != 0)
			params += ", ";

		switch (ctx.getParams()[i].getType()) {
			case kTypeVoid:
				params += "<void>";
				break;

			case kTypeInt:
				params += Common::composeString(ctx.getParams()[i].getInt());
				break;

			case kTypeFloat:
				params += Common::composeString(ctx.getParams()[i].getFloat());
				break;

			case kTypeString:
				params += "\"" + ctx.getParams()[i].getString() + "\"";
				break;

			case kTypeObject:
				params += "<object>(" + formatTag(ctx.getParams()[i].getObject()) + ")";
				break;

			case kTypeVector:
				{
					float x, y, z;
					ctx.getParams()[i].getVector(x, y, z);

					params += "(";
					params += Common::composeString(x) + ", ";
					params += Common::composeString(y) + ", ";
					params += Common::composeString(z);
					params += ")";
				}
				break;

			case kTypeStruct:
				params += "<struct>";
				break;

			case kTypeEngineType:
				params += "<engine>";
				break;

			case kTypeScriptState:
				params += "<state>";
				break;
		}
	}

	return params;
}

Aurora::NWScript::Object *Functions::getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getParams()[n].getObject());
	if (!object || (object->getType() == kObjectTypeInvalid))
		return 0;

	if (object->getType() == kObjectTypeSelf)
		return ctx.getCaller();

	return object;
}

} // End of namespace NWN

} // End of namespace Engines
