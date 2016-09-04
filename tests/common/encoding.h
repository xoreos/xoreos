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
 *  Common utility functions used by the encoding-related unit tests.
 */

#ifndef TESTS_COMMON_ENCODING_H
#define TESTS_COMMON_ENCODING_H

#include <cstdio>
#include <cstdlib>

#include "tests/skip.h"

#include "src/common/encoding.h"

static void testSupport(Common::Encoding encoding) {
	if (Common::hasSupportEncoding(encoding))
		return;

	std::fprintf(stderr, "No support to transcode UTF-8 <-> %s\n", Common::getEncodingName(encoding).c_str());
	std::exit(SKIP_RETURN_CODE); // Skip the test if we don't have relevant support
}

#endif // TESTS_COMMON_ENCODING_H
