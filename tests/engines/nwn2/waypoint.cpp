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
 *  Unit tests for NWN2 waypoint objects.
 */

#include "gtest/gtest.h"

#include "tests/engines/nwn2/waypoint.h"

#include "src/common/memreadstream.h"
#include "src/common/scopedptr.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/waypoint.h"

static const float kEpsilon = 1e-5f;

Engines::NWN2::Waypoint *getWaypoint(const unsigned int index) {
	Common::ScopedPtr<Common::MemoryReadStream> stream(new Common::MemoryReadStream(kDataWaypoint));
	if (!stream)
		throw Common::Exception("No test data available");
	Common::ScopedPtr<Aurora::GFF3File> gff(new Aurora::GFF3File(stream.release(), MKTAG('G', 'I', 'T', ' ')));

	// Get the waypoint list
	const Aurora::GFF3Struct &top = gff->getTopLevel();
	const Aurora::GFF3List &wplist = top.getList("WaypointList");
	if (index >= wplist.size())
		throw Common::Exception("Invalid data found");

	// Get the selected waypoint data
	Aurora::GFF3List::const_iterator it = wplist.begin();
	it += index;

	return new Engines::NWN2::Waypoint(**it);
}

/**
 * Check the map note waypoint
 *
 * Expected values:
 * -------------------------------------
 * Tag            = "mapnote1"
 * MapNoteText    = "My note"
 * HasMapNote     = true
 * MapNoteEnabled = true
 * X/Y/ZPosition  = 6.743333, 4.81983, 0
 * Heading        = 31.5129414
 */

GTEST_TEST(NWN2Waypoint, mapnote1) {
	Common::ScopedPtr<Engines::NWN2::Waypoint> wp(getWaypoint(0));
	float x, y, z, angle;

	EXPECT_STREQ(wp->getTag().c_str(), "mapnote1");
	EXPECT_TRUE(wp->hasMapNote());
	EXPECT_TRUE(wp->enabledMapNote());
	EXPECT_STREQ(wp->getMapNote().c_str(), "[???]"); // Should be "My note"

	wp->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 6.743333) < kEpsilon);
	EXPECT_TRUE(abs(y - 4.81983) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	wp->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - (-31.5129414)) < kEpsilon);
}

/**
 * Check the generic waypoint
 *
 * Expected values:
 * -------------------------------------
 * Tag            = "waypoint1"
 * HasMapNote     = false
 * MapNoteEnabled = false
 * X/Y/ZPosition  = 3.91353, 5.69942, 0
 * Heading        = -33.80449
 */

GTEST_TEST(NWN2Waypoint, waypoint1) {
	Common::ScopedPtr<Engines::NWN2::Waypoint> wp(getWaypoint(1));
	float x, y, z, angle;

	EXPECT_STREQ(wp->getTag().c_str(), "waypoint1");
	EXPECT_FALSE(wp->hasMapNote());
	EXPECT_FALSE(wp->enabledMapNote());
	EXPECT_STREQ(wp->getMapNote().c_str(), "");

	wp->getPosition(x, y, z);
	EXPECT_TRUE(abs(x - 3.91353) < kEpsilon);
	EXPECT_TRUE(abs(y - 5.69942) < kEpsilon);
	EXPECT_TRUE(abs(z) < kEpsilon);

	wp->getOrientation(x, y, z, angle);
	EXPECT_TRUE(abs(angle - 33.80449) < kEpsilon);
}
