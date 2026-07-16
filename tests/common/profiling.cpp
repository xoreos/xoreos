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
 *  Unit tests for the profiling helpers.
 */

#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "src/common/profiling.h"

namespace Common {

TEST(ProfileCounter, IncrementAndRead) {
	ProfileCounter c("nwscript.o_action");
	EXPECT_EQ(c.value(), 0u);
	c.inc();
	c.inc();
	c.inc();
	EXPECT_EQ(c.value(), 3u);
	EXPECT_EQ(c.name(), "nwscript.o_action");
}

TEST(ProfileCounter, ConcurrentIncrementIsThreadSafe) {
	ProfileCounter c("concurrent.test");
	constexpr int kThreads = 8;
	constexpr int kPerThread = 10000;

	std::vector<std::thread> threads;
	for (int i = 0; i < kThreads; ++i) {
		threads.emplace_back([&c] {
			for (int j = 0; j < kPerThread; ++j)
				c.inc();
		});
	}
	for (auto &t : threads) t.join();

	EXPECT_EQ(c.value(), static_cast<uint64_t>(kThreads * kPerThread));
}

TEST(CoverageSet, AddIsUnique) {
	CoverageSet s("model_nwn.node_types");
	EXPECT_FALSE(s.contains("trimesh"));
	s.add("trimesh");
	s.add("danglymesh");
	s.add("trimesh");  // duplicate
	EXPECT_TRUE(s.contains("trimesh"));
	EXPECT_TRUE(s.contains("danglymesh"));
	std::vector<std::string> v = s.values();
	ASSERT_EQ(v.size(), 2u);
}

TEST(ProfilingManager, DisabledByDefault) {
	EXPECT_FALSE(ProfilingManager::enabled());
}

TEST(ProfilingManager, FlushWritesJson) {
	ProfileCounter c("flush.test");
	c.inc();
	c.inc();
	CoverageSet s("flush.cover");
	s.add("foo");

	ProfilingManager::setEnabled(true);
	ProfilingManager::registerCounter(c);
	ProfilingManager::registerCoverage(s);

	const std::string path = "xoreos.test.profile.json";
	ProfilingManager::flush(path);

	std::ifstream f(path);
	ASSERT_TRUE(f.good());
	std::stringstream ss; ss << f.rdbuf();
	std::string contents = ss.str();
	EXPECT_NE(contents.find("\"flush.test\""), std::string::npos);
	EXPECT_NE(contents.find("\"flush.cover\""), std::string::npos);
	EXPECT_NE(contents.find("\"foo\""), std::string::npos);

	std::remove(path.c_str());
	ProfilingManager::setEnabled(false);
}

} // End of namespace Common
