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
 *  Dragon Age II engine functions operating on strings.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/debug.h"
#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/datetime.h"

#include "src/aurora/language.h"
#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/ncsfile.h"
#include "src/aurora/nwscript/util.h"

#include "src/engines/dragonage2/script/functions.h"

namespace Engines {

namespace DragonAge2 {

void Functions::printToLog(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString tstamp = Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T', '-', ':');

	status("DragonAge2: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
}

void Functions::printToLogAndFlush(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: We're already flushing our log file after every line...

	const Common::UString tstamp = Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T', '-', ':');

	status("DragonAge2: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
}

void Functions::logTrace(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString tstamp = Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T', '-', ':');

	status("DragonAge2: %s: [%d]<%s>: %s", tstamp.c_str(), ctx.getParams()[0].getInt(),
	       Aurora::NWScript::formatTag(ctx.getParams()[2].getObject()).c_str(),
	       ctx.getParams()[1].getString().c_str());
}

void Functions::printWarning(Aurora::NWScript::FunctionContext &ctx) {
	status("DragonAge2: WARNING: %s", ctx.getParams()[0].getString().c_str());
}

void Functions::DEBUG_printToScreen(Aurora::NWScript::FunctionContext &ctx) {
	status("DragonAge2: DEBUG: %s", ctx.getParams()[0].getString().c_str());
}

void Functions::printToLogWindow(Aurora::NWScript::FunctionContext &ctx) {
	status("DragonAge2: LOG(%s): %s", ctx.getParams()[1].getString().c_str(),
	       ctx.getParams()[0].getString().c_str());
}

void Functions::printInteger(Aurora::NWScript::FunctionContext &ctx) {
	status("DragonAge2: %d", ctx.getParams()[0].getInt());
}

void Functions::printFloat(Aurora::NWScript::FunctionContext &ctx) {
	const float value    = ctx.getParams()[0].getFloat();
	const int   width    = ctx.getParams()[1].getInt();
	const int   decimals = ctx.getParams()[2].getInt();

	status("DragonAge2: %s", formatFloat(value, width, decimals).c_str());
}

void Functions::printString(Aurora::NWScript::FunctionContext &ctx) {
	status("DragonAge2: %s", ctx.getParams()[0].getString().c_str());
}

void Functions::printObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	status("DragonAge2: object<%s,%p)", Aurora::NWScript::formatTag(object).c_str(),
	       static_cast<void *>(object));
}

void Functions::printVector(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	const bool prepend = ctx.getParams()[1].getInt() != 0;

	status("DragonAge2: %s%f, %f, %f", prepend ? "PRINTVECTOR:" : "", x, y, z);
}

void Functions::printResource(Aurora::NWScript::FunctionContext &ctx) {
	const bool prepend = ctx.getParams()[1].getInt() != 0;

	status("DragonAge2: %s%s", prepend ? "PRINTRESOURCE" : "", ctx.getParams()[0].getString().c_str());
}

void Functions::intToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::composeString(ctx.getParams()[0].getInt());
}

void Functions::floatToString(Aurora::NWScript::FunctionContext &ctx) {
	const float value    = ctx.getParams()[0].getFloat();
	const int   width    = ctx.getParams()[1].getInt();
	const int   decimals = ctx.getParams()[2].getInt();

	ctx.getReturn() = formatFloat(value, width, decimals);
}

void Functions::objectToString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	ctx.getReturn() = Common::UString::format("object<%s,%p)", Aurora::NWScript::formatTag(object).c_str(),
	                                          static_cast<void *>(object));
}

void Functions::vectorToString(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	ctx.getReturn() = Common::UString::format("%f %f %f", x, y, z);
}

void Functions::resourceToString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0];
}

void Functions::toString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = "";

	switch (ctx.getParams()[0].getType()) {
		case Aurora::NWScript::kTypeInt:
			intToString(ctx);
			break;

		case Aurora::NWScript::kTypeFloat:
			floatToString(ctx);
			break;

		case Aurora::NWScript::kTypeString:
			ctx.getReturn() = ctx.getParams()[0];
			break;

		case Aurora::NWScript::kTypeObject:
			objectToString(ctx);
			break;

		case Aurora::NWScript::kTypeVector:
			vectorToString(ctx);
			break;

		default:
			break;
	}
}

void Functions::intToHexString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::format("0x%08x", (uint32) ctx.getParams()[0].getInt());
}

void Functions::stringToInt(Aurora::NWScript::FunctionContext &ctx) {
	int32 i = 0;

	try {
		Common::parseString(ctx.getParams()[0].getString(), i);
	} catch (Common::Exception &e) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: %s (\"%s\")",
		       ctx.getName().c_str(), e.what(), ctx.getParams()[0].getString().c_str());
	}

	ctx.getReturn() = i;
}

void Functions::stringToFloat(Aurora::NWScript::FunctionContext &ctx) {
	float f = 0.0f;

	try {
		Common::parseString(ctx.getParams()[0].getString(), f);
	} catch (Common::Exception &e) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: %s (\"%s\")",
		       ctx.getName().c_str(), e.what(), ctx.getParams()[0].getString().c_str());
	}

	ctx.getReturn() = f;
}

void Functions::stringToVector(Aurora::NWScript::FunctionContext &ctx) {
	float x = 0.0f, y = 0.0f, z = 0.0f;
	ctx.getReturn().setVector(x, y, z);

	std::vector<Common::UString> parts;
	if (Common::UString::split(ctx.getParams()[0].getString(), ' ', parts) != 3) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\"",
		       ctx.getName().c_str(), ctx.getParams()[0].getString().c_str());
		return;
	}

	try {
		Common::parseString(parts[0], x);
		Common::parseString(parts[1], y);
		Common::parseString(parts[2], z);
	} catch (Common::Exception &e) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: %s (\"%s\")",
		       ctx.getName().c_str(), e.what(), ctx.getParams()[0].getString().c_str());
		return;
	}

	ctx.getReturn().setVector(x, y, z);
}

void Functions::charToInt(Aurora::NWScript::FunctionContext &ctx) {
	uint32 c = 0;

	const Common::UString &str = ctx.getParams()[0].getString();
	if (!str.empty())
		c = *str.begin();

	if (!Common::UString::isASCII(c))
		c = 0;

	ctx.getReturn() = (int32) c;
}

void Functions::intToChar(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString((uint32) (ctx.getParams()[0].getInt() & 0x7F), 1);
}

void Functions::isStringEmpty(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().empty();
}

void Functions::getStringLength(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) ctx.getParams()[0].getString().size();
}

void Functions::stringUpperCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toUpper();
}

void Functions::stringLowerCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toLower();
}

void Functions::stringRight(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32 n = ctx.getParams()[1].getInt();
	if ((n <= 0) || ((size_t)n > str.size())) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\", %d",
		       ctx.getName().c_str(), str.c_str(), n);
		return;
	}

	ctx.getReturn() = str.substr(str.getPosition(str.size() - (size_t) n), str.end());
}

void Functions::stringLeft(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32 n = ctx.getParams()[1].getInt();
	if ((n < 0) || ((size_t)n >= str.size())) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\", %d",
		       ctx.getName().c_str(), str.c_str(), n);
		return;
	}

	ctx.getReturn() = str.substr(str.begin(), str.getPosition((size_t) n));
}

void Functions::insertString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();
	if (ctx.getParams()[2].getInt() < 0) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: %d",
		       ctx.getName().c_str(), ctx.getParams()[2].getInt());
		return;
	}

	Common::UString str = ctx.getParams()[0].getString();

	str.insert(str.getPosition((size_t) ctx.getParams()[2].getInt()), ctx.getParams()[1].getString());

	ctx.getReturn() = str;
}

void Functions::subString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32 offset = ctx.getParams()[1].getInt();
	const int32 count  = ctx.getParams()[2].getInt();

	if ((offset < 0) || ((size_t)offset >= str.size()) || (count <= 0)) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\", %d, %d",
		       ctx.getName().c_str(), str.c_str(), offset, count);
		return;
	}

	Common::UString::iterator from = str.getPosition((size_t) offset);
	Common::UString::iterator to   = str.getPosition(MIN<size_t>(offset + count, str.size()));

	ctx.getReturn() = str.substr(from, to);
}

void Functions::findSubString(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &str   = ctx.getParams()[0].getString();
	const Common::UString &sub   = ctx.getParams()[1].getString();
	const int32            start = ctx.getParams()[2].getInt();

	ctx.getReturn() = -1;

	if ((start < 0) || ((size_t)start >= str.size())) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\", \"%s\", %d",
		       ctx.getName().c_str(), str.c_str(), sub.c_str(), start);
		return;
	}

	const Common::UString subStr = str.substr(str.getPosition(start), str.end());

	Common::UString::iterator it = subStr.findFirst(sub);
	if (it == subStr.end())
		return;

	ctx.getReturn() = (int32) (subStr.getPosition(it) + start);
}

void Functions::getStringByStringId(Aurora::NWScript::FunctionContext &ctx) {
	const uint32 strRef = (uint32) ctx.getParams()[0].getInt();

	ctx.getReturn() = TalkMan.getString(strRef, Aurora::kLanguageGenderMale);
}

void Functions::getTlkTableString(Aurora::NWScript::FunctionContext &ctx) {
	const uint32 strRef = (uint32) ctx.getParams()[0].getInt();
	const Aurora::LanguageGender gender = (Aurora::LanguageGender) ctx.getParams()[1].getInt();

	ctx.getReturn() = TalkMan.getString(strRef, gender);
}

void Functions::getCurrentScriptName(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = "";

	const Aurora::NWScript::NCSFile *ncs = ctx.getCurrentScript();
	if (!ncs)
		return;

	ctx.getReturn() = ncs->getName();
}

void Functions::getCurrentScriptResource(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = "";

	const Aurora::NWScript::NCSFile *ncs = ctx.getCurrentScript();
	if (!ncs)
		return;

	ctx.getReturn() = ncs->getName() + ".ncs";
}

} // End of namespace DragonAge2

} // End of namespace Engines
