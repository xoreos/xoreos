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
 *  Jade Empire engine functions reading 2DA values.
 */

#include "src/common/util.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/jade/script/functions.h"

namespace Engines {

namespace Jade {

const Aurora::TwoDAFile &Functions::findTable(int32_t nr) {
	const Aurora::TwoDAFile &scriptlist = TwoDAReg.get2DA("scriptlist");
	int32_t twodasNr = scriptlist.getRow(nr).getInt("2da");

	const Aurora::TwoDAFile &twodas = TwoDAReg.get2DA("2das");
	const Common::UString twodaName = twodas.getRow(twodasNr).getString("file");

	return TwoDAReg.get2DA(twodaName);
}

void Functions::get2DANumRows(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = (int32_t) table.getRowCount();
}

void Functions::get2DANumColumn(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = (int32_t) table.getColumnCount();
}

void Functions::get2DAEntryIntByString(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	Common::UString &columnName = ctx.getParams()[2].getString();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getInt(columnName);
}

void Functions::get2DAEntryFloatByString(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	Common::UString &columnName = ctx.getParams()[2].getString();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getFloat(columnName);
}

void Functions::get2DAEntryStringByString(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	Common::UString &columnName = ctx.getParams()[2].getString();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getString(columnName);
}

void Functions::get2DAEntryInt(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	int32_t columnNr = ctx.getParams()[2].getInt();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getInt(columnNr);
}

void Functions::get2DAEntryFloat(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	int32_t columnNr = ctx.getParams()[2].getInt();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getFloat(columnNr);
}

void Functions::get2DAEntryString(Aurora::NWScript::FunctionContext &ctx) {
	int32_t tableNr = ctx.getParams()[0].getInt();
	int32_t rowNr = ctx.getParams()[1].getInt();
	int32_t columnNr = ctx.getParams()[2].getInt();

	const Aurora::TwoDAFile &table = findTable(tableNr);

	ctx.getReturn() = table.getRow(rowNr).getString(columnNr);
}

} // End of namespace Jade

} // End of namespace Engines
