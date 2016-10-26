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
 *  Unit tests for our PtrMap templates.
 */

#include "gtest/gtest.h"

#include "src/common/system.h"
#include "src/common/ptrmap.h"

static unsigned int kConstructorCalled = 0;
static unsigned int kDestructorCalled  = 0;

class TestClass {
public:
	TestClass () { kConstructorCalled++; }
	~TestClass() { kDestructorCalled++; }
};

class PtrMap : public ::testing::Test {
protected:
	void SetUp() {
		kConstructorCalled = 0;
		kDestructorCalled = 0;
	}
};

GTEST_TEST_F(PtrMap, createAndDestroy) {
	{
		Common::PtrMap<int, TestClass> ptrMap;

		ptrMap.insert(std::make_pair(0, new TestClass));
		ptrMap.insert(std::make_pair(2, new TestClass));
		ptrMap.insert(std::make_pair(1, new TestClass));
		ptrMap.insert(std::make_pair(3, new TestClass));

		EXPECT_EQ(kConstructorCalled, 4);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 4);
}

GTEST_TEST_F(PtrMap, clear) {
	Common::PtrMap<int, TestClass> ptrMap;

	ptrMap.insert(std::make_pair(0, new TestClass));
	ptrMap.clear();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrMap, erasePosition) {
	Common::PtrMap<int, TestClass> ptrMap;

	ptrMap.insert(std::make_pair(0, new TestClass));
	ptrMap.erase(ptrMap.begin());

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrMap, eraseRange) {
	Common::PtrMap<int, TestClass> ptrMap;

	ptrMap.insert(std::make_pair(0, new TestClass));
	ptrMap.insert(std::make_pair(1, new TestClass));
	ptrMap.insert(std::make_pair(2, new TestClass));
	ptrMap.erase(ptrMap.begin(), ptrMap.end());

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);
}

GTEST_TEST_F(PtrMap, eraseVal) {
	Common::PtrMap<int, TestClass> ptrMap;

	ptrMap.insert(std::make_pair(0, new TestClass));
	ptrMap.insert(std::make_pair(1, new TestClass));
	ptrMap.insert(std::make_pair(2, new TestClass));

	ptrMap.erase(1);

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 1);
}
