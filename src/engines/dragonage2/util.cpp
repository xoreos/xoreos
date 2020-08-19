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
 *  Dragon Age II utility functions.
 */

#include "src/common/ustring.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/gdafile.h"

#include "src/engines/dragonage2/util.h"

namespace Engines {

namespace DragonAge2 {

const Aurora::GDAFile &getMGDA(uint32_t id) {
	const Aurora::GDAFile &m2da  = TwoDAReg.getMGDA("m2da_");

	const Common::UString sheetName = m2da.getString(m2da.findRow(id), "Worksheet");

	return TwoDAReg.getMGDA(sheetName);
}

} // End of namespace DragonAge2

} // End of namespace Engines
