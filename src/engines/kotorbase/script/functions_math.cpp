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
 *  Star Wars: Knights of the Old Republic engine functions doing mathematical operations.
 */

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::abs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getInt());
}

void Functions::fabs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getFloat());
}

void Functions::cos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = cosf(Common::deg2rad(ctx.getParams()[0].getFloat()));
}

void Functions::sin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = sinf(Common::deg2rad(ctx.getParams()[0].getFloat()));
}

void Functions::tan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = tanf(Common::deg2rad(ctx.getParams()[0].getFloat()));
}

void Functions::acos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::rad2deg(acosf(ctx.getParams()[0].getFloat()));
}

void Functions::asin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::rad2deg(asinf(ctx.getParams()[0].getFloat()));
}

void Functions::atan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = Common::rad2deg(atanf(ctx.getParams()[0].getFloat()));
}

void Functions::log(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = logf(ctx.getParams()[0].getFloat());
}

void Functions::pow(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = powf(ctx.getParams()[0].getFloat(), ctx.getParams()[1].getFloat());
}

void Functions::sqrt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = sqrtf(ctx.getParams()[0].getFloat());
}

void Functions::random(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(0, ctx.getParams()[0].getInt() - 1);
}

void Functions::d2(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 2, ctx.getParams()[0].getInt());
}

void Functions::d3(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 3, ctx.getParams()[0].getInt());
}

void Functions::d4(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 4, ctx.getParams()[0].getInt());
}

void Functions::d6(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 6, ctx.getParams()[0].getInt());
}

void Functions::d8(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 8, ctx.getParams()[0].getInt());
}

void Functions::d10(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 10, ctx.getParams()[0].getInt());
}

void Functions::d12(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 12, ctx.getParams()[0].getInt());
}

void Functions::d20(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 20, ctx.getParams()[0].getInt());
}

void Functions::d100(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getRandom(1, 100, ctx.getParams()[0].getInt());
}

void Functions::intToFloat(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (float) ctx.getParams()[0].getInt();
}

void Functions::floatToInt(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) ctx.getParams()[0].getFloat();
}

void Functions::vector(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(ctx.getParams()[0].getFloat(),
	                          ctx.getParams()[1].getFloat(),
	                          ctx.getParams()[2].getFloat());
}

void Functions::vectorMagnitude(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	ctx.getReturn() = sqrtf(x*x + y*y + z*z);
}

void Functions::vectorNormalize(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	const float length = sqrtf(x*x + y*y + z*z);

	ctx.getReturn().setVector(x / length, y / length, z / length);
}

} // End of namespace KotORBase

} // End of namespace Engines
