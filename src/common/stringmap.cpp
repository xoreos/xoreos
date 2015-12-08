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
 *  A map to quickly match strings from a list.
 */

#include <cstring>

#include "src/common/stringmap.h"
#include "src/common/util.h"

namespace Common {

StringListMap::StringListMap(const char * const *strings, size_t count, bool onlyFirstWord) : _onlyFirstWord(onlyFirstWord) {
	for (size_t i = 0; i < count; i++)
		_map.insert(std::make_pair(UString(strings[i]), i));
}

size_t StringListMap::find(const char *str, const char **match) const {
	UString sStr = str;

	if (_onlyFirstWord) {
		UString::iterator space = sStr.findFirst(' ');
		if (space != sStr.end())
			sStr.truncate(space);
	}

	StrMap::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return kInvalidIndex;

	if (match)
		*match = str + s->first.size() + 1;
	return s->second;
}

size_t StringListMap::find(const UString &str, const char **match) const {
	UString sStr = str;

	if (_onlyFirstWord) {
		UString::iterator space = sStr.findFirst(' ');
		if (space != sStr.end())
			sStr.truncate(space);
	}

	StrMap::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return kInvalidIndex;

	if (match)
		*match = str.c_str() + s->first.size() + 1;
	return s->second;
}

} // End of namespace Common
