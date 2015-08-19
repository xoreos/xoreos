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
 *  Neverwinter Nights 2 utility functions.
 */

#ifndef ENGINES_NWN2_UTIL_H
#define ENGINES_NWN2_UTIL_H

#include "src/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN2 {

bool readTint(const Aurora::GFF3Struct &gff, float t[3][4]);
bool readTint(const Aurora::GFF3Struct &gff, const Common::UString &strct, float t[3][4]);

/** Strip text enclosed in {} from a string. */
Common::UString createDisplayName(const Common::UString &name);

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_UTIL_H
