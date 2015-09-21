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

#include <boost/date_time/posix_time/posix_time.hpp>

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/strutil.h"

#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/aurora/tokenman.h"

#include "src/engines/kotor/objectcontainer.h"
#include "src/engines/kotor/object.h"
#include "src/engines/kotor/creature.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

void Functions::writeTimestampedLogEntry(Aurora::NWScript::FunctionContext &ctx) {
	boost::posix_time::ptime t(boost::posix_time::second_clock::universal_time());
	const Common::UString tstamp = Common::UString::format("%04d-%02d-%02dT%02d:%02d:%02d",
		(int) t.date().year(), (int) t.date().month(), (int) t.date().day(),
		(int) t.time_of_day().hours(), (int) t.time_of_day().minutes(),
		(int) t.time_of_day().seconds());

	status("KotOR: %s: %s", tstamp.c_str(), ctx.getParams()[0].getString().c_str());
}

void Functions::sendMessageToPC(Aurora::NWScript::FunctionContext &ctx) {
	Creature *pc = KotOR::ObjectContainer::toPC(getParamObject(ctx, 0));
	if (!pc)
		return;

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

	status("KotOR: object<%s,%p)", formatTag(object).c_str(), (void *) object);
}

void Functions::objectToString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	ctx.getReturn() = Common::UString::format("object<%s,%p)", formatTag(object).c_str(), (void *) object);
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
	ctx.getReturn() = Common::UString::format("0x%08x", (uint32) ctx.getParams()[0].getInt());
}

void Functions::stringToInt(Aurora::NWScript::FunctionContext &ctx) {
	int32 i = 0;
	Common::parseString(ctx.getParams()[0].getString(), i);

	ctx.getReturn() = i;
}

void Functions::stringToFloat(Aurora::NWScript::FunctionContext &ctx) {
	float f = 0;
	Common::parseString(ctx.getParams()[0].getString(), f);

	ctx.getReturn() = f;
}

void Functions::getStringLength(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) ctx.getParams()[0].getString().size();
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

	const int32 n = ctx.getParams()[1].getInt();
	if ((n <= 0) || ((size_t)n > str.size()))
		return;

	ctx.getReturn() = str.substr(str.getPosition(str.size() - (size_t) n), str.end());
}

void Functions::getStringLeft(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32 n = ctx.getParams()[1].getInt();
	if ((n < 0) || ((size_t)n >= str.size()))
		return;

	ctx.getReturn() = str.substr(str.begin(), str.getPosition((size_t) n));
}

void Functions::insertString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();
	if (ctx.getParams()[2].getInt() < 0)
		return;

	Common::UString str = ctx.getParams()[0].getString();

	str.insert(str.getPosition((size_t) ctx.getParams()[2].getInt()), ctx.getParams()[1].getString());

	ctx.getReturn() = str;
}

void Functions::getSubString(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Common::UString &str = ctx.getParams()[0].getString();

	const int32 offset = ctx.getParams()[1].getInt();
	const int32 count  = ctx.getParams()[2].getInt();

	if ((offset < 0) || ((size_t)offset >= str.size()) || (count <= 0))
		return;

	Common::UString::iterator from = str.getPosition((size_t) offset);
	Common::UString::iterator to   = str.getPosition((size_t) MIN<int32>(offset + count, str.size()));

	ctx.getReturn() = str.substr(from, to);
}

void Functions::findSubString(Aurora::NWScript::FunctionContext &ctx) {
	const Common::UString &str = ctx.getParams()[0].getString();
	const Common::UString &sub = ctx.getParams()[1].getString();

	Common::UString::iterator it = str.findFirst(sub);
	if (it == str.end())
		ctx.getReturn() = -1;

	ctx.getReturn() = (int32) str.getPosition(it);
}

void Functions::getStringByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	const uint32 strRef = (uint32) ctx.getParams()[0].getInt();
	const Aurora::LanguageGender gender = (Aurora::LanguageGender) ctx.getParams()[1].getInt();

	ctx.getReturn() = TalkMan.getString(strRef, gender);
}

} // End of namespace KotOR

} // End of namespace Engines
