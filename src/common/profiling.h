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
 *  Lightweight profiling helpers: thread-safe counters, set-shaped coverage,
 *  and a JSON dumper. All work is gated on ProfilingManager::enabled() so
 *  production builds have near-zero cost.
 */

#ifndef COMMON_PROFILING_H
#define COMMON_PROFILING_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Common {

class ProfileCounter {
public:
	explicit ProfileCounter(std::string name) : _name(std::move(name)), _value(0) {}

	void inc() { _value.fetch_add(1, std::memory_order_relaxed); }
	uint64_t value() const { return _value.load(std::memory_order_relaxed); }
	const std::string &name() const { return _name; }

private:
	std::string _name;
	std::atomic<uint64_t> _value;
};

class CoverageSet {
public:
	explicit CoverageSet(std::string name) : _name(std::move(name)) {}

	void add(const std::string &v) {
		std::lock_guard<std::mutex> lock(_mutex);
		_set.insert(v);
	}
	bool contains(const std::string &v) const {
		std::lock_guard<std::mutex> lock(_mutex);
		return _set.count(v) > 0;
	}
	std::vector<std::string> values() const {
		std::lock_guard<std::mutex> lock(_mutex);
		return std::vector<std::string>(_set.begin(), _set.end());
	}
	const std::string &name() const { return _name; }

private:
	std::string _name;
	mutable std::mutex _mutex;
	std::unordered_set<std::string> _set;
};

class ProfilingManager {
public:
	static bool enabled() { return _enabled(); }
	static void setEnabled(bool e) { _enabled() = e; }

	static void registerCounter(ProfileCounter &c);
	static void registerCoverage(CoverageSet &s);
	static void flush(const std::string &path);

private:
	static bool &_enabled();
	static std::unordered_map<std::string, ProfileCounter *> &_counters();
	static std::unordered_map<std::string, CoverageSet *> &_coverages();
	static std::mutex &_registryMutex();
};

} // End of namespace Common

#endif // COMMON_PROFILING_H
