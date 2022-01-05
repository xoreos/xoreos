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
#include <cstdlib>

#include <functional>

#include "src/common/util.h"
#include "src/common/random.h"
#include "src/common/string.h"

#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/util.h"

#include "src/graphics/graphics.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/game.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"

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

int32_t Functions::getRandom(int min, int max, int32_t n) {
	if (n < 1)
		n = 1;

	int32_t r = 0;

	while (n-- > 0)
		r += RNG.getNext(min, max + 1);

	return r;
}

Common::UString Functions::formatFloat(float f, int width, int decimals) {
	return Common::String::format("%*.*f", width, decimals, f);
}

Aurora::NWScript::Object *Functions::getParamObject(const Aurora::NWScript::FunctionContext &ctx, size_t n) {
	NWN::Object *object = NWN::ObjectContainer::toObject(ctx.getParams()[n].getObject());
	if (!object || (object->getType() == kObjectTypeInvalid))
		return 0;

	if (object->getType() == kObjectTypeSelf)
		return ctx.getCaller();

	return object;
}

void Functions::jumpTo(NWN::Object *object, Area *area, float x, float y, float z) {
	// Sanity check
	if (!object->getArea() || !area) {
		warning("Functions::jumpTo(): No area?!? (%d, %d)", object->getArea() != 0, area != 0);
		return;
	}

	GfxMan.lockFrame();

	// Are we moving between areas?
	if (object->getArea() != area) {
		const Common::UString &areaFrom = object->getArea()->getResRef();
		const Common::UString &areaTo   = area->getResRef();

		warning("TODO: Functions::jumpTo(): Moving from \"%s\" to \"%s\"", areaFrom.c_str(), areaTo.c_str());

		NWN::Object *pc = NWN::ObjectContainer::toObject(_game->getModule().getPC());
		if (pc) {
			const Common::UString &pcArea = pc->getArea()->getResRef();

			if (areaFrom == pcArea) {
				// Moving away from the currently visible area.

				object->hide();
				object->unloadModel();

			} else if (areaTo == pcArea) {
				// Moving into the currently visible area.

				object->loadModel();
				object->show();
			}

		}

		object->setArea(area);
	}

	// Update position
	object->setPosition(x, y, z);

	GfxMan.unlockFrame();

	if (object == _game->getModule().getPC())
		_game->getModule().movedPC();
}

} // End of namespace NWN

} // End of namespace Engines
