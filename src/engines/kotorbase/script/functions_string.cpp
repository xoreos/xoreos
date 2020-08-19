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
 *  Star Wars: Knights of the Old Republic engine functions operating on strings.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/debug.h"
#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/datetime.h"

#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/util.h"

#include "src/engines/aurora/tokenman.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/objectcontainer.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::writeTimestampedLogEntry(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString tstamp = Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T', '-', ':');

	status("KotOR: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
}

void Functions::sendMessageToPC(Aurora::NWScript::FunctionContext &ctx) {
	Creature *pc = ObjectContainer::toPC(getParamObject(ctx, 0));
	if (!pc) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: No PC", ctx.getName().c_str());
		return;
	}

	const Common::UString &msg = ctx.getParams()[1].getString();

	warning("Send message to PC \"%s\": \"%s\"", pc->getName().c_str(), msg.c_str());
}

void Functions::printInteger(Aurora::NWScript::FunctionContext &ctx) {
	status("KotOR: %d", ctx.getParams()[0].getInt());
}

void Functions::printFloat(Aurora::NWScript::FunctionContext &ctx) {
	const float value    = ctx.getParams()[0].getFloat();
	const int   width    = ctx.getParams()[1].getInt();
	const int   decimals = ctx.getParams()[2].getInt();

	status("KotOR: %s", formatFloat(value, width, decimals).c_str());
}

void Functions::printString(Aurora::NWScript::FunctionContext &ctx) {
	status("KotOR: %s", ctx.getParams()[0].getString().c_str());
}

void Functions::printObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	status("KotOR: object<%s,%p)", Aurora::NWScript::formatTag(object).c_str(), static_cast<void *>(object));
}

void Functions::objectToString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	ctx.getReturn() = Common::UString::format("object<%s,%p)", Aurora::NWScript::formatTag(object).c_str(),
	                                          static_cast<void *>(object));
}

void Functions::printVector(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	const bool prepend = ctx.getParams()[1].getInt() != 0;

	status("KotOR: %s%f, %f, %f", prepend ? "PRINTVECTOR:" : "", x, y, z);
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

void Functions::intToHexString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::UString::format("0x%08x", (uint32_t) ctx.getParams()[0].getInt());
}

void Functions::stringToInt(Aurora::NWScript::FunctionContext &ctx) {
	int32_t i = 0;

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

void Functions::getStringLength(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) ctx.getParams()[0].getString().size();
}

void Functions::getStringUpperCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toUpper();
}

void Functions::getStringLowerCase(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getParams()[0].getString().toLower();
}

void Functions::getStringRight(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32_t n = ctx.getParams()[1].getInt();
	if ((n <= 0) || ((size_t)n > str.size())) {
		debugC(Common::kDebugEngineScripts, 1, "Functions::%s: \"%s\", %d",
		       ctx.getName().c_str(), str.c_str(), n);
		return;
	}

	ctx.getReturn() = str.substr(str.getPosition(str.size() - (size_t) n), str.end());
}

void Functions::getStringLeft(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32_t n = ctx.getParams()[1].getInt();
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

void Functions::getSubString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32_t offset = ctx.getParams()[1].getInt();
	const int32_t count  = ctx.getParams()[2].getInt();

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
	const Common::UString &str = ctx.getParams()[0].getString();
	const Common::UString &sub = ctx.getParams()[1].getString();

	ctx.getReturn() = -1;

	Common::UString::iterator it = str.findFirst(sub);
	if (it == str.end())
		return;

	ctx.getReturn() = (int32_t) str.getPosition(it);
}

void Functions::getStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	const uint32_t strRef = (uint32_t) ctx.getParams()[0].getInt();

	ctx.getReturn() = TalkMan.getString(strRef);
}

} // End of namespace KotORBase

} // End of namespace Engines
