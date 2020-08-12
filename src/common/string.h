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
 *  Utility templates and functions for working with strings.
 */

#ifndef XOREOS_COMMON_STRING_H
#define XOREOS_COMMON_STRING_H

#include <string>

#include "src/common/system.h"

namespace Common {
namespace String {

/** Print formatted data into a std::string object, similar to sprintf(). */
std::string format(const char *format, ...) GCC_PRINTF(1, 2);

/** Print formatted data into a std::string object, similar to vsprintf(). */
std::string formatV(const char *format, va_list args) GCC_PRINTF(1, 0);

} // End of namespace String
} // End of namespace Common

#endif // XOREOS_COMMON_STRING_H
