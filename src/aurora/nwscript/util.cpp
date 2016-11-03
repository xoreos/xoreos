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
 *  NWScript utility functions.
 */

#include "src/common/strutil.h"

#include "src/aurora/nwscript/object.h"
#include "src/aurora/nwscript/functioncontext.h"

namespace Aurora {

namespace NWScript {

Common::UString formatTag(const Object *object) {
	if (!object)
		return "0";

	return "\"" + object->getTag() + "\"";
}

void formatVariable(Common::UString &str, const Variable &var) {
	switch (var.getType()) {
		case kTypeInt:
			str += Common::composeString(var.getInt());
			break;

		case kTypeFloat:
			str += Common::composeString(var.getFloat());
			break;

		case kTypeString:
			str += "\"" + var.getString() + "\"";
			break;

		case kTypeObject:
			str += "<" + formatType(var.getType()) + ">(" + formatTag(var.getObject()) + ")";
			break;

		case kTypeVector:
			{
				float x, y, z;
				var.getVector(x, y, z);

				str += "[";
				str += Common::composeString(x) + ", ";
				str += Common::composeString(y) + ", ";
				str += Common::composeString(z);
				str += "]";
			}
			break;

		case kTypeArray:
			{
				const Variable::Array &array = var.getArray();

				str += "{";
				for (size_t i = 0; i < array.size(); i++) {
					if (i != 0)
						str += ", ";

					if (!array[i].get())
						str += "/";
					else
						formatVariable(str, *array[i]);
				}
				str += "}";
			}
			break;

		case kTypeReference:
			{
				Variable *ref = var.getReference();

				str += "<" + formatType(var.getType()) + ">(";
				if (ref)
					formatVariable(str, *ref);
				else
					str += "0";
				str += ")";
			}
			break;

		default:
			str += "<" + formatType(var.getType()) + ">";
			break;
	}
}

Common::UString formatParams(const FunctionContext &ctx) {
	Common::UString params;
	for (size_t i = 0; i < ctx.getParams().size(); i++) {
		if (i != 0)
			params += ", ";

		formatVariable(params, ctx.getParams()[i]);
	}

	return params;
}

Common::UString formatReturn(const FunctionContext &ctx) {
	Common::UString r;

	if (ctx.getReturn().getType() != kTypeVoid)
		formatVariable(r, ctx.getReturn());

	return r;
}

Common::UString formatType(Type type) {
	switch (type) {
		case kTypeVoid:
			return "void";

		case kTypeInt:
			return "int";

		case kTypeFloat:
			return "float";

		case kTypeString:
			return "string";

		case kTypeObject:
			return "object";

		case kTypeVector:
			return "vector";

		case kTypeEngineType:
			return "engine";

		case kTypeScriptState:
			return "state";

		case kTypeArray:
			return "array";

		case kTypeReference:
			return "ref";

		case kTypeAny:
			return "any";
	}

	return "unknown";
}

} // End of namespace NWScript

} // End of namespace Aurora
