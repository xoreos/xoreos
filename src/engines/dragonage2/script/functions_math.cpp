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
 *  Dragon Age II engine functions doing mathematical operations.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/random.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/dragonage2/script/functions.h"

namespace Engines {

namespace DragonAge2 {

void Functions::abs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getInt());
}

void Functions::fabs(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ABS(ctx.getParams()[0].getFloat());
}

void Functions::cos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = cosf(ctx.getParams()[0].getFloat());
}

void Functions::sin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = sinf(ctx.getParams()[0].getFloat());
}

void Functions::tan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = tanf(ctx.getParams()[0].getFloat());
}

void Functions::acos(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = acosf(ctx.getParams()[0].getFloat());
}

void Functions::asin(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = asinf(ctx.getParams()[0].getFloat());
}

void Functions::atan(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = atanf(ctx.getParams()[0].getFloat());
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
	ctx.getReturn() = RNG.getNext(0, ctx.getParams()[0].getInt());
}

void Functions::randomFloat(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = RNG.getNext(0.0f, 1.0f);
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

void Functions::isVectorEmpty(Aurora::NWScript::FunctionContext &ctx) {
	static const float kEpsilon = 1e-5f;

	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	ctx.getReturn() = (ABS(x) < kEpsilon) && (ABS(y) < kEpsilon) && (ABS(z) < kEpsilon);
}

void Functions::getVectorMagnitude(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	ctx.getReturn() = sqrtf(x*x + y*y + z*z);
}

void Functions::getVectorNormalize(Aurora::NWScript::FunctionContext &ctx) {
	float x, y, z;
	ctx.getParams()[0].getVector(x, y, z);

	const float length = sqrtf(x*x + y*y + z*z);

	ctx.getReturn().setVector(x / length, y / length, z / length);
}

void Functions::getArraySize(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) ctx.getParams()[0].getArraySize();
}

} // End of namespace DragonAge2

} // End of namespace Engines
