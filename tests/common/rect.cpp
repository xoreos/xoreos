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
 *  Unit tests for our rectangle class.
 */

#include "gtest/gtest.h"

#include "src/common/rect.h"

GTEST_TEST(Rect, SimpleRect) {
	Common::Rect rect;
	rect.x = 5;
	rect.y = 5;
	rect.w = 5;
	rect.h = 5;

	EXPECT_FALSE(rect.empty());
}

GTEST_TEST(Rect, EmptyRect) {
	Common::Rect rect;
	rect.x = 5;
	rect.y = 5;
	rect.w = 0;
	rect.h = 5;

	EXPECT_TRUE(rect.empty());
}
