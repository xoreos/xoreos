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
 *  Unit tests for our PtrList templates.
 */

#include "gtest/gtest.h"

#include "src/common/system.h"
#include "src/common/ptrlist.h"

static unsigned int kConstructorCalled = 0;
static unsigned int kDestructorCalled  = 0;

class TestClass {
public:
	TestClass () { kConstructorCalled++; }
	~TestClass() { kDestructorCalled++; }
};

class PtrList : public ::testing::Test {
protected:
	void SetUp() {
		kConstructorCalled = 0;
		kDestructorCalled = 0;
	}
};

GTEST_TEST_F(PtrList, createAndDestroy) {
	{
		Common::PtrList<TestClass> ptrList;

		ptrList.push_back(new TestClass);
		ptrList.push_back(new TestClass);
		ptrList.push_front(new TestClass);
		ptrList.push_front(new TestClass);

		EXPECT_EQ(kConstructorCalled, 4);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 4);
}

GTEST_TEST_F(PtrList, clear) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.clear();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrList, resize) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);

	ptrList.resize(1);

	EXPECT_EQ(ptrList.size(), 1);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 2);

	ptrList.resize(3);

	EXPECT_EQ(ptrList.size(), 3);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 2);

	ptrList.clear();

	EXPECT_EQ(ptrList.size(), 0);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrList.resize(1);
	EXPECT_EQ(ptrList.size(), 1);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);

	EXPECT_EQ(ptrList.front(), static_cast<TestClass *>(0));
}

GTEST_TEST_F(PtrList, popFront) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.pop_front();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrList, popBack) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.pop_back();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrList, erasePosition) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.erase(ptrList.begin());

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrList, eraseRange) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.erase(ptrList.begin(), ptrList.end());

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);
}

GTEST_TEST_F(PtrList, remove) {
	Common::PtrList<TestClass> ptrList;

	TestClass *testClass = new TestClass;

	ptrList.push_back(new TestClass);
	ptrList.push_back(testClass);
	ptrList.push_back(new TestClass);

	ptrList.remove(testClass);

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 1);
}

static bool testPred(const TestClass * const UNUSED(x)) {
	return true;
}

GTEST_TEST_F(PtrList, removeIf) {
	Common::PtrList<TestClass> ptrList;

	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);

	ptrList.remove_if(testPred);

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);
}

GTEST_TEST_F(PtrList, assign) {
	Common::PtrList<TestClass> ptrList;
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);

	std::list<TestClass *> list;
	list.push_back(new TestClass);
	list.push_back(new TestClass);
	list.push_back(new TestClass);

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 0);

	ptrList.assign(list.begin(), list.end());
	list.clear();

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrList.clear();

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 6);
}

GTEST_TEST_F(PtrList, assignN) {
	Common::PtrList<TestClass> ptrList;
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);
	ptrList.push_back(new TestClass);

	ptrList.assign(1, new TestClass);

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrList.clear();

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 4);
}
