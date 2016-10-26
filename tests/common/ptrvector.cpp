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
 *  Unit tests for our PtrVector templates.
 */

#include "gtest/gtest.h"

#include "src/common/system.h"
#include "src/common/ptrvector.h"

static unsigned int kConstructorCalled = 0;
static unsigned int kDestructorCalled  = 0;

class TestClass {
public:
	TestClass () { kConstructorCalled++; }
	~TestClass() { kDestructorCalled++; }
};

class PtrVector : public ::testing::Test {
protected:
	void SetUp() {
		kConstructorCalled = 0;
		kDestructorCalled = 0;
	}
};

GTEST_TEST_F(PtrVector, createAndDestroy) {
	{
		Common::PtrVector<TestClass> ptrVector;

		ptrVector.push_back(new TestClass);
		ptrVector.push_back(new TestClass);
		ptrVector.push_back(new TestClass);
		ptrVector.push_back(new TestClass);

		EXPECT_EQ(kConstructorCalled, 4);
		EXPECT_EQ(kDestructorCalled , 0);
	}

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 4);
}

GTEST_TEST_F(PtrVector, clear) {
	Common::PtrVector<TestClass> ptrVector;

	ptrVector.push_back(new TestClass);
	ptrVector.clear();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrVector, resize) {
	Common::PtrVector<TestClass> ptrVector;

	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);

	ptrVector.resize(1);

	EXPECT_EQ(ptrVector.size(), 1);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 2);

	ptrVector.resize(3);

	EXPECT_EQ(ptrVector.size(), 3);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 2);

	ptrVector.clear();

	EXPECT_EQ(ptrVector.size(), 0);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrVector.resize(1);
	EXPECT_EQ(ptrVector.size(), 1);
	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);

	EXPECT_EQ(ptrVector.front(), static_cast<TestClass *>(0));
}

GTEST_TEST_F(PtrVector, popBack) {
	Common::PtrVector<TestClass> ptrVector;

	ptrVector.push_back(new TestClass);
	ptrVector.pop_back();

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrVector, erasePosition) {
	Common::PtrVector<TestClass> ptrVector;

	ptrVector.push_back(new TestClass);
	ptrVector.erase(ptrVector.begin());

	EXPECT_EQ(kConstructorCalled, 1);
	EXPECT_EQ(kDestructorCalled , 1);
}

GTEST_TEST_F(PtrVector, eraseRange) {
	Common::PtrVector<TestClass> ptrVector;

	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);
	ptrVector.erase(ptrVector.begin(), ptrVector.end());

	EXPECT_EQ(kConstructorCalled, 3);
	EXPECT_EQ(kDestructorCalled , 3);
}

GTEST_TEST_F(PtrVector, assign) {
	Common::PtrVector<TestClass> ptrVector;
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);

	std::vector<TestClass *> vector;
	vector.push_back(new TestClass);
	vector.push_back(new TestClass);
	vector.push_back(new TestClass);

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 0);

	ptrVector.assign(vector.begin(), vector.end());
	vector.clear();

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrVector.clear();

	EXPECT_EQ(kConstructorCalled, 6);
	EXPECT_EQ(kDestructorCalled , 6);
}

GTEST_TEST_F(PtrVector, assignN) {
	Common::PtrVector<TestClass> ptrVector;
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);
	ptrVector.push_back(new TestClass);

	ptrVector.assign(1, new TestClass);

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 3);

	ptrVector.clear();

	EXPECT_EQ(kConstructorCalled, 4);
	EXPECT_EQ(kDestructorCalled , 4);
}
