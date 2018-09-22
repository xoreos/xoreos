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
  *  Unit tests for the base Trigger class.
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
	assert((size % 3) == 0);
	load(verts, size);
}

void UtilTrigger::load(const float verts[], const size_t size) {
	float x, y, z;
	int i = 0;

	for (size_t n = 0; n < size; n++) {
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
 *     |              :          / \ 6
 *   4 -              -         /   \
 *     |              :        /     \
 *   3 -      B-------C       /       G 
 *     |     /        :\     /       /
 *   2 -    / 3       - \ 4 /       /
 *     |   /          :  \ /       /
 *   1 -  A           -   /       F 7
 *     |   \          :  / \       \
 *   0 -..:.\.:...:...*./.:.\.:...:.\.:...:
 *     |     \        :/     \       \
 *  -1 -      \   2   /   8   \       E 
 *     |       \     /:        \     /
 *  -2 -      1 \   / -         \ 5 /
 *     |         \ /  :          \ /
 *  -3 -          I   -           D 
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos1[] = {
    -2.0, -3.0, // 1 -- Outside
    -1.0, -1.0, // 2 -- Inside
    -2.0,  2.0, // 3 -- Inside
     1.0,  2.0, // 4 -- Outside
     3.0, -2.0, // 5 -- Inside
     3.8,  5.5, // 6 -- Outside
     3.5,  1.0, // 7 -- Outside
     1.0, -1.0, // 8 -- Outside
};

static const float kTestClass1[] = {
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

/*
 * Test Region 2:
 *
 *   5 -        C---------------------D
 *     |       /      :                \
 *   4 -    1 /       H-----------G   7 \
 *     |     /       /:       6   |      \
 *   3 -    /       / -           F-------E
 *     |   /       /  :             8
 *   2 -  B       J   -   P-----------Q
 *     |  |       |   :   | 5         |
 *   1 -  |   2   |   -   N---M       |
 *     |  |       |   :       /       |
 *   0 -..:...:...:...*...:../:...:...:...:
 *     |  |       |   :     /         |
 *  -1 -  A       | 3 -    / 4        R 
 *     |   \      K---:---L          /
 *  -2 -    \         -             /
 *     |     \        :            /
 *  -3 -      T-------------------S
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos2[] {
    -2.5,  4.0, // 1 -- Outside
    -2.0,  1.0, // 2 -- Inside
    -0.5, -1.0, // 3 -- Outside
     1.7,  1.0, // 4 -- Inside
     1.5,  1.5, // 5 -- Inside
     2.0,  3.5, // 6 -- Outside
     4.0,  4.0, // 7 -- Inside
};

static const float kTestClass2[] = {
    -1.0, -3.0,  1.0, // A
    -1.0,  2.0,  1.0, // B
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

/*
 * Test Region 3:
 *
 *   5 -              -               J---K
 *     |              :               |   |
 *   4 -      A---B   -               |   |
 *     |      |   |   :               | 8 |
 *   3 -      | 2 |   -   E---F       |   |
 *     |      |   |   :   |   |     7 |   |   9
 *   2 -      |   |   -   |   G-------H   |
 *     |      |   |   :   |       6       |
 *   1 -  1   |   |   -   |   P-------N   |
 *     |      |   | 3 :   |   | 5     |   |
 *   0 -..:...:...:...*...:...:...:...:...:
 *     |      |   |   :   |   |       |   |
 *  -1 -      |   C-------D   |       M---L
 *     |      |       :     4 |
 *  -2 -      R---------------Q
 *     |              :
 *  -3 -              -  10
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos3[] {
    -1.0,  1.0, // 1 -- Outside
    -1.5,  3.0, // 2 -- Inside
    -0.5,  0.5, // 3 -- Outside
     1.5, -1.5, // 4 -- Inside
     2.5,  0.5, // 5 -- Outside
     3.0,  1.5, // 6 -- Inside
     3.5,  2.5, // 7 -- Outside
     4.5,  3.5, // 8 -- Inside
     6.0,  2.5, // 9 -- Outside
     1.0, -3.0, //10 -- Outside
};

static const float kTestClass3[] = {
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

/*
 * Test Region 4:
 *
 *   5 -  1           -
 *     |              :           
 *   4 -      C-----------------------D
 *     |     /        :   4          /
 *   3 -    /       Q-----------P   /
 *     |   /       /  :        /   /
 *   2 -  B   2   /   -       /   /
 *     |  |      /    :      /   /
 *   1 -  |     /     -     / 5 / G---J
 *     |  |    /      :    /   /  |    \
 *   0 -..:...:...:...*.../...:...:...:.\.:
 *     |  |  /        :  /   /    |      \
 *  -1 -  | /     3   - /   / 7   |   8   K  10
 *     |  |/          :/   /      |      /
 *  -2 -  A           N   E-------F     /
 *     |              | 6              /9
 *  -3 -              M---------------L
 *     |                                  
 *     +--|---|---|---|---|---|---|---|---|-
 *       -3  -2  -1   0   1   2   3   4   5
 */
static const float kTestClassPos4[] {
    -3.0,  5.0, // 1 -- Outside
    -2.0,  2.0, // 2 -- Inside
    -1.0, -1.0, // 3 -- Outside
     1.0,  3.5, // 4 -- Inside
     2.0,  1.0, // 5 -- Inside
     0.5, -2.5, // 6 -- Inside
     2.0, -1.0, // 7 -- Outside
     4.0, -1.0, // 8 -- Inside
     4.5, -2.5, // 9 -- Outside
     6.0, -1.0, //10 -- Outside
};

static const float kTestClass4[] = {
    -3.0, -2.0, 1.0e20, // A
    -3.0,  2.0, 2.0e19, // B
    -2.0,  4.0, 3.0e18, // C
     4.0,  4.0, 4.0e17, // D
     1.0, -2.0, 5.0e16, // E
     3.0, -2.0, 6.0e15, // F
     3.0,  1.0, 7.0e14, // G
     4.0,  1.0, 8.0e13, // J
     5.0, -1.0, 9.0e13, // K
     4.0, -3.0, 8.0e12, // L
     0.0, -3.0, 7.0e11, // M
     0.0, -2.0, 6.0e10, // N
     2.5,  3.0, 5.0e09, // P
     0.5,  3.0, 4.0e08, // Q
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
		EXPECT_EQ(trigger.contains(x, y), result[n]);
	}
}

} // End of namespace Engines

