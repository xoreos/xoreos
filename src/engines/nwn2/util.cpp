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

/** @file engines/nwn2/util.cpp
 *  NWN2 utility functions
 */

#include "common/ustring.h"

#include "aurora/gfffile.h"

#include "engines/nwn2/util.h"

namespace Engines {

namespace NWN2 {

bool readTint(const Aurora::GFFStruct &gff, float t[3][4]) {
	if (!gff.hasField("Tintable"))
		return false;

	const Aurora::GFFStruct &tintable = gff.getStruct("Tintable");
	if (!tintable.hasField("Tint"))
		return false;

	const Aurora::GFFStruct &tint = tintable.getStruct("Tint");

	for (int i = 0; i < 3; i++) {
		Common::UString index = Common::UString::sprintf("%d", i + 1);
		if (!tint.hasField(index))
			continue;

		const Aurora::GFFStruct &tintN = tint.getStruct(index);

		t[i][0] = tintN.getUint("r", t[i][0] * 255) / 255.0f;
		t[i][1] = tintN.getUint("g", t[i][1] * 255) / 255.0f;
		t[i][2] = tintN.getUint("b", t[i][2] * 255) / 255.0f;
		t[i][3] = tintN.getUint("a", t[i][3] * 255) / 255.0f;
	}

	return true;
}

bool readTint(const Aurora::GFFStruct &gff, const Common::UString &strct, float t[3][4]) {
	if (!gff.hasField(strct))
		return false;

	return readTint(gff.getStruct(strct), t);
}

} // End of namespace NWN2

} // End of namespace Engines
