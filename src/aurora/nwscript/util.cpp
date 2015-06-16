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
 *  NWScript utility functions.
 */

#include <cstdarg>

#include "src/common/util.h"

#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/variable.h"

namespace Aurora {

namespace NWScript {

Signature createSignature(size_t n, ...) {
	Signature signature;

	signature.reserve(n);

	va_list va;

	va_start(va, n);

	while (n-- > 0)
		signature.push_back((Type) va_arg(va, int));

	va_end(va);

	return signature;
}

Parameters createDefaults(size_t n, ...) {
	Parameters defaults;

	defaults.reserve(n);

	va_list va;

	va_start(va, n);

	while (n-- > 0)
		defaults.push_back(*va_arg(va, const Variable *));

	va_end(va);

	return defaults;
}

} // End of namespace NWScript

} // End of namespace Aurora
