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
 *  Unit test for the GDA column headers hash calculation
 */

#include "gtest/gtest.h"

#include "src/aurora/gdaheaders.h"

GTEST_TEST(gdaHeaders, findGDAHeader) {
	struct TestCase {
		constexpr explicit TestCase(uint32_t hash, const char *expected) : _hash(hash), _expected(expected) {}

		uint32_t _hash;

		const char *_expected;
	};

	static constexpr TestCase testCases[] = {
		TestCase(   1421660U, "AttackScatter"),
		TestCase(1511494451U, nullptr        ),
		TestCase(3330190315U, "Slot5"        ),
		TestCase(4294639615U, "CameraOffset" )
	};

	for (size_t i = 0; i < std::size(testCases); i++) {
		const char *const result = Aurora::findGDAHeader(testCases[i]._hash);

		if (testCases[i]._expected == nullptr) {
			EXPECT_EQ(nullptr, result) << "At index " << i;
		} else {
			EXPECT_STREQ(testCases[i]._expected, result) << "At index " << i;
		}
	}
}
