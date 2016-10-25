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
 *  Unit tests for our DisposablePtr/DisposableArray templates.
 */

#include "gtest/gtest.h"

#include "src/common/disposableptr.h"

static unsigned int kConstructorCalled = 0;
static unsigned int kDestructorCalled  = 0;

class TestClass {
public:
	TestClass () { kConstructorCalled++; }
	~TestClass() { kDestructorCalled++; }

	int getData() { return 23; }
};

// --- DisposablePtr ---

class DisposablePtr : public ::testing::Test {
protected:
	void SetUp() {
		kConstructorCalled = 0;
		kDestructorCalled = 0;
	}
};

GTEST_TEST_F(DisposablePtr, createAndDestroy) {
	{
		EXPECT_EQ(kConstructorCalled, 0);
		EXPECT_EQ(kDestructorCalled , 0);

		Common::DisposablePtr<TestClass> disposablePtr(new TestClass, true);

		EXPECT_EQ(kConstructorCalled, 1);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, reset) {
	Common::DisposablePtr<TestClass> disposablePtr(0, true);

	EXPECT_EQ(kConstructorCalled, 0);
	EXPECT_EQ(kDestructorCalled , 0);

	disposablePtr.reset(new TestClass);

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 0);

	disposablePtr.reset();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, castBool) {
	Common::DisposablePtr<TestClass> disposablePtr(0, true);
	EXPECT_FALSE((bool) disposablePtr);

	disposablePtr.reset(new TestClass);
	EXPECT_TRUE((bool) disposablePtr);

	disposablePtr.reset();
	EXPECT_FALSE((bool) disposablePtr);

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, get) {
	{
		Common::DisposablePtr<TestClass> disposablePtr(0, true);
		EXPECT_EQ(disposablePtr.get(), static_cast<TestClass *>(0));

		TestClass *testClass = new TestClass;
		disposablePtr.reset(testClass);

		EXPECT_NE(disposablePtr.get(), static_cast<TestClass *>(0));
		EXPECT_EQ(disposablePtr.get(), testClass);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, operatorDeref) {
	{
		TestClass *testClass1 = new TestClass;
		Common::DisposablePtr<TestClass> disposablePtr(testClass1, true);

		TestClass &testClass2 = *disposablePtr;
		EXPECT_EQ(&testClass2, testClass1);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, operatorMemberAccess) {
	Common::DisposablePtr<TestClass> disposablePtr(new TestClass, true);

	EXPECT_EQ(disposablePtr->getData(), 23);
}

GTEST_TEST_F(DisposablePtr, dontDispose) {
	TestClass *testClass1 = new TestClass;

	{
		Common::DisposablePtr<TestClass> disposablePtr(testClass1, false);

		TestClass *testClass2 = disposablePtr.get();
		EXPECT_EQ(testClass1, testClass2);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	ASSERT_EQ(kDestructorCalled , 0);

	delete testClass1;
}

GTEST_TEST_F(DisposablePtr, unconditionalDispose) {
	Common::DisposablePtr<TestClass> disposablePtr(new TestClass, false);
	disposablePtr.dispose();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(DisposablePtr, setDisposable) {
	{
		Common::DisposablePtr<TestClass> disposablePtr(new TestClass, false);
		disposablePtr.setDisposable(true);
	}

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);

	TestClass *testClass = new TestClass;

	{
		Common::DisposablePtr<TestClass> disposablePtr(testClass, true);
		disposablePtr.setDisposable(false);
	}

	EXPECT_EQ(kConstructorCalled, 2);
	ASSERT_EQ(kDestructorCalled , 1);

	delete testClass;
}

// --- DisposableArray ---

class DisposableArray : public DisposablePtr {
};

GTEST_TEST_F(DisposableArray, createAndDestroy) {
	{
		EXPECT_EQ(kConstructorCalled, 0);
		EXPECT_EQ(kDestructorCalled , 0);

		Common::DisposableArray<TestClass> disposableArray(new TestClass[5], true);

		EXPECT_EQ(kConstructorCalled, 5);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(DisposableArray, reset) {
	Common::DisposableArray<TestClass> disposableArray(0, true);

	EXPECT_EQ(kConstructorCalled, 0);
	EXPECT_EQ(kDestructorCalled , 0);

	disposableArray.reset(new TestClass[5]);

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 0);

	disposableArray.reset();

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(DisposableArray, castBool) {
	Common::DisposableArray<TestClass> disposableArray(0, true);
	EXPECT_FALSE((bool) disposableArray);

	disposableArray.reset(new TestClass[5]);
	EXPECT_TRUE((bool) disposableArray);

	disposableArray.reset();
	EXPECT_FALSE((bool) disposableArray);

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(DisposableArray, get) {
	{
		Common::DisposableArray<TestClass> disposableArray(0, true);
		EXPECT_EQ(disposableArray.get(), static_cast<TestClass *>(0));

		TestClass *testClass = new TestClass[5];
		disposableArray.reset(testClass);

		EXPECT_NE(disposableArray.get(), static_cast<TestClass *>(0));
		EXPECT_EQ(disposableArray.get(), testClass);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(DisposableArray, operatorMemberAccess) {
	TestClass *testClass = new TestClass[5];
	Common::DisposableArray<TestClass> disposableArray(testClass, true);

	for (size_t i = 0; i < 5; i++) {
		EXPECT_EQ(&disposableArray[i], &testClass[i]);
		EXPECT_EQ(disposableArray[i].getData(), 23);
	}
}

GTEST_TEST_F(DisposableArray, dontDispose) {
	TestClass *testClass1 = new TestClass[5];

	{
		Common::DisposableArray<TestClass> disposableArray(testClass1, false);

		TestClass *testClass2 = disposableArray.get();
		EXPECT_EQ(testClass1, testClass2);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	ASSERT_EQ(kDestructorCalled , 0);

	delete[] testClass1;
}

GTEST_TEST_F(DisposableArray, unconditionalDispose) {
	Common::DisposableArray<TestClass> disposableArray(new TestClass[5], false);
	disposableArray.dispose();

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);
}

GTEST_TEST_F(DisposableArray, setDisposable) {
	{
		Common::DisposableArray<TestClass> disposableArray(new TestClass[5], false);
		disposableArray.setDisposable(true);
	}

	EXPECT_EQ(kConstructorCalled, 5);
	EXPECT_EQ(kDestructorCalled , 5);

	TestClass *testClass = new TestClass[5];

	{
		Common::DisposableArray<TestClass> disposableArray(testClass, true);
		disposableArray.setDisposable(false);
	}

	EXPECT_EQ(kConstructorCalled, 10);
	ASSERT_EQ(kDestructorCalled ,  5);

	delete[] testClass;
}
