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
 *  Implementation of the profiling helpers.
 */

#include <cstdio>
#include <fstream>
#include <sstream>

#include "src/common/profiling.h"
#include "src/common/ustring.h"
#include "src/common/writefile.h"

namespace Common {

bool &ProfilingManager::_enabled() {
	static bool e = false;
	return e;
}

std::unordered_map<std::string, ProfileCounter *> &ProfilingManager::_counters() {
	static std::unordered_map<std::string, ProfileCounter *> m;
	return m;
}

std::unordered_map<std::string, CoverageSet *> &ProfilingManager::_coverages() {
	static std::unordered_map<std::string, CoverageSet *> m;
	return m;
}

std::mutex &ProfilingManager::_registryMutex() {
	static std::mutex m;
	return m;
}

void ProfilingManager::registerCounter(ProfileCounter &c) {
	std::lock_guard<std::mutex> lock(_registryMutex());
	_counters()[c.name()] = &c;
}

void ProfilingManager::registerCoverage(CoverageSet &s) {
	std::lock_guard<std::mutex> lock(_registryMutex());
	_coverages()[s.name()] = &s;
}

static std::string jsonEscape(const std::string &s) {
	std::string out;
	out.reserve(s.size() + 2);
	for (char c : s) {
		switch (c) {
		case '"':  out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\n': out += "\\n";  break;
		case '\r': out += "\\r";  break;
		case '\t': out += "\\t";  break;
		default:
			if (static_cast<unsigned char>(c) < 0x20)
				out += '?';
			else
				out += c;
		}
	}
	return out;
}

void ProfilingManager::flush(const std::string &path) {
	std::lock_guard<std::mutex> lock(_registryMutex());

	std::ostringstream out;
	out << "{\n  \"version\": 1,\n";

	out << "  \"counters\": {\n";
	bool first = true;
	for (auto &kv : _counters()) {
		if (!first) out << ",\n";
		first = false;
		out << "    \"" << jsonEscape(kv.first) << "\": " << kv.second->value();
	}
	out << "\n  },\n";

	out << "  \"coverage\": {\n";
	first = true;
	for (auto &kv : _coverages()) {
		if (!first) out << ",\n";
		first = false;
		out << "    \"" << jsonEscape(kv.first) << "\": [";
		auto values = kv.second->values();
		for (size_t i = 0; i < values.size(); ++i) {
			if (i) out << ", ";
			out << "\"" << jsonEscape(values[i]) << "\"";
		}
		out << "]";
	}
	out << "\n  }\n";

	out << "}\n";

	// Atomic write: write to tmp, then rename.
	const std::string tmpPath = path + ".tmp";
	{
		WriteFile f;
		if (!f.open(UString(tmpPath))) return;
		f.writeString(out.str());
		f.close();
	}
	std::rename(tmpPath.c_str(), path.c_str());
}

} // End of namespace Common
