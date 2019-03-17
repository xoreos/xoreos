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
 *  Neverwinter Nights 2 engine functions messing with item properties.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/itemproperty.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::getItemPropertyType(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	ctx.getReturn() = (prop) ? prop->getItemPropertyType() : -1;
}

void Functions::getItemPropertySubType(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint16 subType = prop->getItemPropertySubType();
		if (subType != UINT16_MAX)
			result = (int) subType;
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyParam1(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8 param1 = prop->getItemPropertyParam1();
		if (param1 != UINT8_MAX)
			result = (int) param1;
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyParam1Value(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8 param1Value = prop->getItemPropertyParam1Value();
		if (param1Value != UINT8_MAX)
			result = (int) param1Value;
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyCostTable(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8 costTable = prop->getItemPropertyCostTable();
		if (costTable != UINT8_MAX)
			result = (int) costTable;
	}
	ctx.getReturn() = result;
}

void Functions::getItemPropertyCostTableValue(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	int result = -1;
	if (prop && prop->getItemPropertyType() != kItemPropertyInvalid) {
		const uint8 costTableValue = prop->getItemPropertyCostTableValue();
		if (costTableValue != UINT8_MAX)
			result = (int) costTableValue;
	}
	ctx.getReturn() = result;
}

void Functions::getIsItemPropertyValid(Aurora::NWScript::FunctionContext &ctx) {
	ItemProperty *prop = NWN2::ObjectContainer::toItemProperty(ctx.getParams()[0].getEngineType());
	if (prop)
		ctx.getReturn() = prop->getIsItemPropertyValid();
}

} // End of namespace NWN2

} // End of namespace Engines
