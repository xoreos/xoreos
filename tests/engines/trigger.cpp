/* xoreos - A reimplementation of BioWare's Aurora engine *
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
  *  Unit tests for the Engines::Trigger class.
  */

#include "gtest/gtest.h"

#include "src/common/util.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/trigger.h"


namespace Engines {

// Utility class for testing Engines::Trigger
class UtilTrigger : public Trigger {
public:
	UtilTrigger(const float verts[], const size_t size);

protected:
	void load(const float verts[], const size_t size);
};

UtilTrigger::UtilTrigger(const float verts[], const size_t size)
	: Trigger() {
	load(verts, size);
	prepare();
}

void UtilTrigger::load(const float verts[], const size_t size) {
	float x, y, z;
	size_t i = 0;

	assert((size % 3) == 0);

	while (i < size) {
		x = verts[i++];
		y = verts[i++];
		z = verts[i++];
		_geometry.push_back(glm::vec3(x, y, z));
	}
}


/*
 * Test Region 1:
 *
 *   5 -              -           H
 *     |              :          / \ 5 
 *   4 -              -         /   \
 *     |              :        /     \
 *   3 -      B-------C       /       G 
 *     |     /        :\     /       /
 *   2 -    / 2       - \ 3 /       /
 *     |   /          :  \ /       /
 *   1 -  A           -   /       F 6 
 *     |   \          :  / \       \
 *   0 -..:.\.:...:...*./.:.\.:...:.\.:...:
 *     |     \        :/     \       \
 *  -1 -      \   1   /   7   \       E 
 *     |       \     /:        \     /
 *  -2 -      0 \   / -         \ 4 /
 *     |         \ /  :          \ /
 *  -3 -          I   -           D 
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos1[16] = {
    -2.0, -2.0, // 0 -- Outside
    -1.0, -1.0, // 1 -- Inside
    -2.0,  2.0, // 2 -- Inside
     1.0,  2.0, // 3 -- Outside
     3.0, -2.0, // 4 -- Inside
     3.9,  4.9, // 5 -- Outside
     3.5,  1.0, // 6 -- Outside
     1.0, -1.0, // 7 -- Outside
};

static const float kTestClass1[27] = {
    -3.0,  1.0,  0.0, // A
    -2.0,  3.0,  0.5, // B
     0.0,  3.0,  1.5, // C
     3.0, -3.0,  2.0, // D
     4.0, -1.0,  0.5, // E
     3.0,  1.0,  0.0, // F
     4.0,  3.0, -1.0, // G
     3.0,  5.0, -2.0, // H
    -1.0, -3.0, -0.5  // I
};

GTEST_TEST(TestTrigger, contains_part_1) {
	size_t size = ARRAYSIZE(kTestClass1);
	UtilTrigger trigger(kTestClass1, size);
	const size_t num = 8;
	bool result[num] = {
		false, true, true, false,
		true, false, false, false };
	int i = 0;

	for (size_t n = 0; n < num; n++) {
		float x = kTestClassPos1[i++];
		float y = kTestClassPos1[i++];
		EXPECT_EQ(trigger.contains(x, y), result[n]) << "At index " << n;
	}
}

/*
 * Test Region 2:
 *
 *   5 -        C---------------------D
 *     |       /      :                \
 *   4 -    0 /       H-----------G   6 \
 *     |     /       /:       5   |      \
 *   3 -    /       / -           F-------E
 *     |   /       /  :            7 
 *   2 -  B       J   -   P-----------Q
 *     |  |       |   :   | 4         |
 *   1 -  |   1   |   -   N---M       |
 *     |  |       |   :       /       |
 *   0 -..:...:...:...*...:../:...:...:...:
 *     |  |       |   :     /         |
 *  -1 -  A       | 2 -    / 3        R 
 *     |   \      K---:---L          /
 *  -2 -    \         -             /
 *     |     \        :            /
 *  -3 -      T-------------------S
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos2[16] = {
    -2.5,  4.0, // 0 -- Outside
    -2.0,  1.0, // 1 -- Inside
    -0.5, -1.0, // 2 -- Outside
     1.7, -1.0, // 3 -- Inside
     1.5,  1.5, // 4 -- Inside
     2.0,  3.5, // 5 -- Outside
     4.0,  4.0, // 6 -- Inside
     3.0,  2.5, // 7 -- Outside
};

static const float kTestClass2[54] = {
    -3.0, -1.0,  1.0, // A
    -3.0,  2.0,  1.0, // B
    -1.5,  5.0,  1.0, // C
     4.0,  5.0, -1.0, // D
     5.0,  3.0, -1.0, // E
     3.0,  3.0, -1.0, // F
     3.0,  4.0, -1.0, // G
     0.0,  4.0, -1.0, // H
    -1.0,  2.0,  1.0, // J
    -1.0, -1.5,  1.0, // K
     1.0, -1.5, -1.0, // L
     2.0,  1.0, -1.0, // M
     1.0,  1.0, -1.0, // N
     1.0,  2.0, -1.0, // P
     4.0,  2.0, -1.0, // Q
     4.0, -1.0, -1.0, // R
     3.0, -3.0, -1.0, // S
    -2.0, -3.0,  1.0, // T
};

GTEST_TEST(TestTrigger, contains_part_2) {
	size_t size = ARRAYSIZE(kTestClass2);
	UtilTrigger trigger(kTestClass2, size);
	const size_t num = 8;
	bool result[num] = {
		false, true, false, true,
		true, false, true, false };
	int i = 0;

	for (size_t n = 0; n < num; n++) {
		float x = kTestClassPos2[i++];
		float y = kTestClassPos2[i++];
		EXPECT_EQ(trigger.contains(x, y), result[n]) << "At index " << n;
	}
}

/*
 * Test Region 3:
 *
 *   5 -              -               J---K
 *     |              :               |   |
 *   4 -      A---B   -               |   |
 *     |      |   |   :               | 7 |
 *   3 -      | 1 |   -   E---F       |   |
 *     |      |   |   :   |   |     6 |   |   8
 *   2 -      |   |   -   |   G-------H   |
 *     |      |   |   :   |       5       |
 *   1 -  0   |   |   -   |   P-------N   |
 *     |      |   | 2 :   |   | 4     |   |
 *   0 -..:...:...:...*...:...:...:...:...:
 *     |      |   |   :   |   |       |   |
 *  -1 -      |   C-------D   |       M---L
 *     |      |       :     3 |
 *  -2 -      R---------------Q
 *     |              :
 *  -3 -              -   9 
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos3[20] = {
    -3.0,  1.0, // 0 -- Outside
    -1.5,  3.0, // 1 -- Inside
    -0.5,  0.5, // 2 -- Outside
     1.5, -1.5, // 3 -- Inside
     2.5,  0.5, // 4 -- Outside
     3.0,  1.5, // 5 -- Inside
     3.5,  2.5, // 6 -- Outside
     4.5,  3.5, // 7 -- Inside
     6.0,  2.5, // 8 -- Outside
     1.0, -3.0, // 9 -- Outside
};

static const float kTestClass3[48] = {
    -2.0,  4.0, -1.0, // A
    -1.0,  4.0, -1.0, // B
    -1.0, -1.0, -1.0, // C
     1.0,  1.0,  0.0, // D
     1.0,  3.0,  0.0, // E
     2.0,  3.0,  0.0, // F
     2.0,  2.0,  0.0, // G
     4.0,  2.0,  0.0, // H
     4.0,  5.0,  0.0, // J
     5.0,  5.0,  0.0, // K
     5.0, -1.0,  0.0, // L
     4.0, -1.0,  0.0, // M
     4.0,  1.0,  0.0, // N
     2.0,  1.0,  0.0, // P
     2.0, -2.0,  0.0, // Q
    -2.0, -2.0, -1.0, // R
};

GTEST_TEST(TestTrigger, contains_part_3) {
	size_t size = ARRAYSIZE(kTestClass3);
	UtilTrigger trigger(kTestClass3, size);
	const size_t num = 10;
	bool result[num] = {
		false, true, false, true, false,
		true, false, true, false, false };
	int i = 0;

	for (size_t n = 0; n < num; n++) {
		float x = kTestClassPos3[i++];
		float y = kTestClassPos3[i++];
		EXPECT_EQ(trigger.contains(x, y), result[n]) << "At index " << n;
	}
}

/*
 * Test Region 4:
 *
 *   5 -  0           -
 *     |              :           
 *   4 -      C-----------------------D
 *     |     /        :   3          /
 *   3 -    /       Q-----------P   /
 *     |   /       /  :        /   /
 *   2 -  B   1   /   -       /   /
 *     |  |      /    :      /   /
 *   1 -  |     /     -     / 4 / G---J
 *     |  |    /      :    /   /  |    \
 *   0 -..:...:...:...*.../...:...:...:.\.:
 *     |  |  /        :  /   /    |      \
 *  -1 -  | /     2   - /   / 6   |   7   K   9
 *     |  |/          :/   /      |      /
 *  -2 -  A           N   E-------F     /
 *     |              | 5              / 8
 *  -3 -              M---------------L
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos4[20] = {
    -3.0,  5.0, // 0 -- Outside
    -2.0,  2.0, // 1 -- Inside
    -1.0, -1.0, // 2 -- Outside
     1.0,  3.5, // 3 -- Inside
     2.0,  1.0, // 4 -- Inside
     0.5, -2.5, // 5 -- Inside
     2.0, -1.0, // 6 -- Outside
     4.0, -1.0, // 7 -- Inside
     4.8, -2.8, // 8 -- Outside
     6.0, -1.0, // 9 -- Outside
};

static const float kTestClass4[42] = {
    -3.0, -2.0, -1.0, // A
    -3.0,  2.0, -2.0, // B
    -2.0,  4.0, -3.0, // C
     4.0,  4.0, 4.0, // D
     1.0, -2.0, 5.0, // E
     3.0, -2.0, 6.0, // F
     3.0,  1.0, 7.0, // G
     4.0,  1.0, 8.0, // J
     5.0, -1.0, 9.0, // K
     4.0, -3.0, 8.0, // L
     0.0, -3.0, 7.0, // M
     0.0, -2.0, 6.0, // N
     2.5,  3.0, 5.0, // P
     0.5,  3.0, -4.0, // Q
};

GTEST_TEST(TestTrigger, contains_part_4) {
	size_t size = ARRAYSIZE(kTestClass4);
	UtilTrigger trigger(kTestClass4, size);
	const size_t num = 10;
	bool result[num] = {
		false, true, false, true, true,
		true, false, true, false, false };
	int i = 0;

	for (size_t n = 0; n < num; n++) {
		float x = kTestClassPos4[i++];
		float y = kTestClassPos4[i++];
		EXPECT_EQ(trigger.contains(x, y), result[n]) << "At index " << n;
	}
}

} // End of namespace Engines

