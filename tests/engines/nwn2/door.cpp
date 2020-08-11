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
 *  Unit tests for NWN2 door objects.
 */

#include <memory>

#include "gtest/gtest.h"

#include "tests/engines/nwn2/door.h"

#include "src/common/memreadstream.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/nwn2.h"
#include "src/engines/nwn2/console.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/door.h"

static const float kEpsilon = 1e-5f;

Engines::NWN2::Door *getDoor(const unsigned int index) {
	std::unique_ptr<Common::MemoryReadStream> stream = std::make_unique<Common::MemoryReadStream>(kDataDoor);
	if (!stream)
		throw Common::Exception("No test data available");
	std::unique_ptr<Aurora::GFF3File> gff = std::make_unique<Aurora::GFF3File>(stream.release(), MKTAG('G', 'I', 'T', ' '));

	// Get the door list
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	const Aurora::GFF3List &doorList = top.getList("Door List");
	if (index >= doorList.size())
		throw Common::Exception("Invalid data found");

	// Get the selected door data
	Aurora::GFF3List::const_iterator it = doorList.begin();
	it += index;

	std::unique_ptr<Engines::NWN2::Module> module = std::make_unique<Engines::NWN2::Module>();
	return new Engines::NWN2::Door(*module, **it);
}

/** Check the first door. */

GTEST_TEST(NWN2Door, door1) {
	std::unique_ptr<Engines::NWN2::Door> door(getDoor(0));
	float x, y, z, angle;

	EXPECT_STREQ(door->getTag().c_str(), "mytag1");
	EXPECT_TRUE(door->isStatic());
	EXPECT_TRUE(door->isUsable());
	EXPECT_FALSE(door->isClickable());
//	EXPECT_FALSE(door->isOpen());
	EXPECT_TRUE(door->isLocked());
	EXPECT_TRUE(door->isLockable());
	EXPECT_TRUE(door->isKeyRequired());

	EXPECT_EQ(door->getLockLockDC(), 22);
	EXPECT_EQ(door->getLockUnlockDC(), 15);
	EXPECT_STREQ(door->getLockKeyTag().c_str(), "door1_key");
	EXPECT_EQ(door->getCurrentHP(), 15);
	EXPECT_EQ(door->getMaxHP(), 15);

	EXPECT_FALSE(door->getIsTrapped());
	EXPECT_FALSE(door->getTrapActive());
	EXPECT_TRUE(door->getTrapDetectable());
	EXPECT_TRUE(door->getTrapDisarmable());
	EXPECT_FALSE(door->getTrapFlagged());
	EXPECT_TRUE(door->getTrapOneShot());
	EXPECT_TRUE(door->getTrapRecoverable());

	EXPECT_EQ(door->getTrapBaseType(), 0);
	EXPECT_EQ(door->getTrapDetectDC(), 21);
	EXPECT_EQ(door->getTrapDisarmDC(), 24);
	EXPECT_STREQ(door->getTrapKeyTag().c_str(), "door1_key");

	door->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 3.60024) < kEpsilon);
	EXPECT_TRUE(abs(y - 8.99981) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	door->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - (0)) < kEpsilon);
}

/** Check 'set' calls to the first door. */

GTEST_TEST(NWN2Door, door1changes) {
	std::unique_ptr<Engines::NWN2::Door> door(getDoor(0));

//	door->setLocked(false);
	door->setLockable(false);
	door->setKeyRequired(false);
	door->setLockLockDC(21);
	door->setLockUnlockDC(17);
	door->setLockKeyTag("door1_keylock");

	door->setTrapDetectable(false);
	door->setTrapDisarmable(false);
	door->setTrapOneShot(false);
	door->setTrapRecoverable(false);
	door->setTrapDetectDC(25);
	door->setTrapDisarmDC(29);
	door->setTrapKeyTag("door1_newkey");

//	EXPECT_FALSE(door->isLocked());
	EXPECT_FALSE(door->isLockable());
	EXPECT_FALSE(door->isKeyRequired());

	EXPECT_EQ(door->getLockLockDC(), 21);
	EXPECT_EQ(door->getLockUnlockDC(), 17);
	EXPECT_STREQ(door->getLockKeyTag().c_str(), "door1_keylock");

	EXPECT_FALSE(door->getTrapDetectable());
	EXPECT_FALSE(door->getTrapDisarmable());
	EXPECT_FALSE(door->getTrapOneShot());
	EXPECT_FALSE(door->getTrapRecoverable());

	EXPECT_EQ(door->getTrapDetectDC(), 25);
	EXPECT_EQ(door->getTrapDisarmDC(), 29);
	EXPECT_STREQ(door->getTrapKeyTag().c_str(), "door1_newkey");
}

/** Check the second door. */

GTEST_TEST(NWN2Door, door2) {
	std::unique_ptr<Engines::NWN2::Door> door(getDoor(1));
	float x, y, z, angle;

	EXPECT_STREQ(door->getTag().c_str(), "mytag2");
	EXPECT_FALSE(door->isStatic());
	EXPECT_TRUE(door->isUsable());
	EXPECT_TRUE(door->isClickable());
//	EXPECT_FALSE(door->isOpen());
	EXPECT_FALSE(door->isLocked());
	EXPECT_FALSE(door->isLockable());
	EXPECT_FALSE(door->isKeyRequired());

	EXPECT_EQ(door->getLockLockDC(), 18);
	EXPECT_EQ(door->getLockUnlockDC(), 0);
	EXPECT_STREQ(door->getLockKeyTag().c_str(), "");
	EXPECT_EQ(door->getCurrentHP(), 15);
	EXPECT_EQ(door->getMaxHP(), 15);

	EXPECT_FALSE(door->getIsTrapped());
	EXPECT_FALSE(door->getTrapActive());
	EXPECT_TRUE(door->getTrapDetectable());
	EXPECT_TRUE(door->getTrapDisarmable());
	EXPECT_FALSE(door->getTrapFlagged());
	EXPECT_TRUE(door->getTrapOneShot());
	EXPECT_TRUE(door->getTrapRecoverable());

	EXPECT_EQ(door->getTrapBaseType(), 0);
	EXPECT_EQ(door->getTrapDetectDC(), 0);
	EXPECT_EQ(door->getTrapDisarmDC(), 15);
	EXPECT_STREQ(door->getTrapKeyTag().c_str(), "");

	door->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 9.00029) < kEpsilon);
	EXPECT_TRUE(abs(y - 5.40050) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	door->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - (-90.0002)) < kEpsilon);
}

/** Check the third door. */

GTEST_TEST(NWN2Door, door3) {
	std::unique_ptr<Engines::NWN2::Door> door(getDoor(2));
	float x, y, z, angle;

	EXPECT_STREQ(door->getTag().c_str(), "mytag3");
	EXPECT_TRUE(door->isStatic());
	EXPECT_TRUE(door->isUsable());
	EXPECT_FALSE(door->isClickable());
//	EXPECT_FALSE(door->isOpen());
	EXPECT_FALSE(door->isLocked());
	EXPECT_FALSE(door->isLockable());
	EXPECT_FALSE(door->isKeyRequired());

	EXPECT_EQ(door->getLockLockDC(), 18);
	EXPECT_EQ(door->getLockUnlockDC(), 0);
	EXPECT_STREQ(door->getLockKeyTag().c_str(), "");
	EXPECT_EQ(door->getCurrentHP(), 15);
	EXPECT_EQ(door->getMaxHP(), 15);

	EXPECT_TRUE(door->getIsTrapped());
	EXPECT_FALSE(door->getTrapActive());
	EXPECT_TRUE(door->getTrapDetectable());
	EXPECT_TRUE(door->getTrapDisarmable());
	EXPECT_FALSE(door->getTrapFlagged());
	EXPECT_TRUE(door->getTrapOneShot());
	EXPECT_FALSE(door->getTrapRecoverable());

	EXPECT_EQ(door->getTrapBaseType(), 35);
	EXPECT_EQ(door->getTrapDetectDC(), 31);
	EXPECT_EQ(door->getTrapDisarmDC(), 25);
	EXPECT_STREQ(door->getTrapKeyTag().c_str(), "");

	door->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 5.40049) < kEpsilon);
	EXPECT_TRUE(abs(y - (-0.00029)) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	door->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - 179.999847) < kEpsilon);
}

/** Check the fourth door. */

GTEST_TEST(NWN2Door, door4) {
	std::unique_ptr<Engines::NWN2::Door> door(getDoor(3));
	float x, y, z, angle;

	EXPECT_STREQ(door->getTag().c_str(), "mytag4");
	EXPECT_FALSE(door->isStatic());
	EXPECT_TRUE(door->isUsable());
	EXPECT_TRUE(door->isClickable());
//	EXPECT_FALSE(door->isOpen());
	EXPECT_FALSE(door->isLocked());
	EXPECT_FALSE(door->isLockable());
	EXPECT_FALSE(door->isKeyRequired());

	EXPECT_EQ(door->getLockLockDC(), 18);
	EXPECT_EQ(door->getLockUnlockDC(), 0);
	EXPECT_STREQ(door->getLockKeyTag().c_str(), "");
	EXPECT_EQ(door->getCurrentHP(), 25);
	EXPECT_EQ(door->getMaxHP(), 30);

	EXPECT_FALSE(door->getIsTrapped());
	EXPECT_FALSE(door->getTrapActive());
	EXPECT_TRUE(door->getTrapDetectable());
	EXPECT_TRUE(door->getTrapDisarmable());
	EXPECT_FALSE(door->getTrapFlagged());
	EXPECT_TRUE(door->getTrapOneShot());
	EXPECT_TRUE(door->getTrapRecoverable());

	EXPECT_EQ(door->getTrapBaseType(), 0);
	EXPECT_EQ(door->getTrapDetectDC(), 0);
	EXPECT_EQ(door->getTrapDisarmDC(), 15);
	EXPECT_STREQ(door->getTrapKeyTag().c_str(), "");

	door->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - (-0.00029)) < kEpsilon);
	EXPECT_TRUE(abs(y - 3.59951) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	door->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - 90.0002) < kEpsilon);
}
