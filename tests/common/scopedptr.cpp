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
 *  Unit tests for our ScopedPtr/ScopedArray templates.
 */

#include "gtest/gtest.h"

#include "src/common/scopedptr.h"

static unsigned int kConstructorCalled = 0;
static unsigned int kDestructorCalled  = 0;

class TestClass {
public:
	TestClass () { kConstructorCalled++; }
	~TestClass() { kDestructorCalled++; }

	int getData() { return 23; }
};

// --- ScopedPtr ---

class ScopedPtr : public ::testing::Test {
protected:
	void SetUp() {
		kConstructorCalled = 0;
		kDestructorCalled = 0;
	}
};

GTEST_TEST_F(ScopedPtr, createAndDestroy) {
	{
		EXPECT_EQ(kConstructorCalled, 0);
		EXPECT_EQ(kDestructorCalled , 0);

		Common::ScopedPtr<TestClass> scopedPtr(new TestClass);

		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, reset) {
	Common::ScopedPtr<TestClass> scopedPtr;

	EXPECT_EQ(kConstructorCalled, 0);
	EXPECT_EQ(kDestructorCalled , 0);

	scopedPtr.reset(new TestClass);

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 0);

	scopedPtr.reset();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, release) {
	{
		Common::ScopedPtr<TestClass> scopedPtr(new TestClass);

		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 0);

		TestClass *testClass = scopedPtr.release();

		EXPECT_NE(testClass, static_cast<TestClass *>(0));
		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 0);

		delete testClass;

		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 1);

		testClass = scopedPtr.release();

		EXPECT_EQ(testClass, static_cast<TestClass *>(0));
		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 1);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, castBool) {
	Common::ScopedPtr<TestClass> scopedPtr;
	EXPECT_FALSE((bool) scopedPtr);

	scopedPtr.reset(new TestClass);
	EXPECT_TRUE((bool) scopedPtr);

	scopedPtr.reset();
	EXPECT_FALSE((bool) scopedPtr);

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, get) {
	{
		Common::ScopedPtr<TestClass> scopedPtr;
		EXPECT_EQ(scopedPtr.get(), static_cast<TestClass *>(0));

		TestClass *testClass = new TestClass;
		scopedPtr.reset(testClass);

		EXPECT_NE(scopedPtr.get(), static_cast<TestClass *>(0));
		EXPECT_EQ(scopedPtr.get(), testClass);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, operatorDeref) {
	{
		TestClass *testClass1 = new TestClass;
		Common::ScopedPtr<TestClass> scopedPtr(testClass1);

		TestClass &testClass2 = *scopedPtr;
		EXPECT_EQ(&testClass2, testClass1);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(ScopedPtr, operatorMemberAccess) {
	Common::ScopedPtr<TestClass> scopedPtr(new TestClass);

	EXPECT_EQ(scopedPtr->getData(), 23);
}

// --- ScopedArray ---

class ScopedArray : public ScopedPtr {
};

GTEST_TEST_F(ScopedArray, createAndDestroy) {
	{
		EXPECT_EQ(kConstructorCalled, 0);
		EXPECT_EQ(kDestructorCalled , 0);

		Common::ScopedArray<TestClass> scopedArray(new TestClass[5]);

		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(ScopedArray, reset) {
	Common::ScopedArray<TestClass> scopedArray;

	EXPECT_EQ(kConstructorCalled, 0);
	EXPECT_EQ(kDestructorCalled , 0);

	scopedArray.reset(new TestClass[5]);

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 0);

	scopedArray.reset();

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(ScopedArray, release) {
	{
		Common::ScopedArray<TestClass> scopedArray(new TestClass[5]);

		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 0);

		TestClass *testClass = scopedArray.release();

		EXPECT_NE(testClass, static_cast<TestClass *>(0));
		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 0);

		delete[] testClass;

		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 5);

		testClass = scopedArray.release();

		EXPECT_EQ(testClass, static_cast<TestClass *>(0));
		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 5);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(ScopedArray, castBool) {
	Common::ScopedArray<TestClass> scopedArray;
	EXPECT_FALSE((bool) scopedArray);

	scopedArray.reset(new TestClass[5]);
	EXPECT_TRUE((bool) scopedArray);

	scopedArray.reset();
	EXPECT_FALSE((bool) scopedArray);

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(ScopedArray, get) {
	{
		Common::ScopedArray<TestClass> scopedArray;
		EXPECT_EQ(scopedArray.get(), static_cast<TestClass *>(0));

		TestClass *testClass = new TestClass[5];
		scopedArray.reset(testClass);

		EXPECT_NE(scopedArray.get(), static_cast<TestClass *>(0));
		EXPECT_EQ(scopedArray.get(), testClass);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(ScopedArray, operatorMemberAccess) {
	TestClass *testClass = new TestClass[5];
	Common::ScopedArray<TestClass> scopedArray(testClass);

	for (size_t i = 0; i < 5; i++) {
		EXPECT_EQ(&scopedArray[i], &testClass[i]);
		EXPECT_EQ(scopedArray[i].getData(), 23);
	}
}
