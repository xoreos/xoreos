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

#ifndef COMMON_STRWORDMAP_H
#define COMMON_STRWORDMAP_H

#include <map>

#include <boost/unordered/unordered_map.hpp>

#include "src/common/ustring.h"

namespace Common {

typedef std::map<UString, UString> StringMap;
typedef std::map<UString, UString, UString::iless> StringIMap;

typedef boost::unordered_map<UString, UString, hashUStringCaseSensitive> StringHashMap;
typedef boost::unordered_map<UString, UString, hashUStringCaseInsensitive> StringHashIMap;

/** A map to quickly match strings from a list. */
class StringListMap {
public:
	static const size_t kInvalidIndex = SIZE_MAX;

	/** Build a string map to match a list of strings against. */
	StringListMap(const char * const *strings, size_t count, bool onlyFirstWord = false);

	/** Match a string against the map.
	 *
	 *  @param  str The string to match against.
	 *  @param  match If != 0, the position after the match will be stored here.
	 *  @return The index of the matched string in the original list, or kInvalidIndex if not found.
	 */
	size_t find(const char *str, const char **match) const;

	/** Match a string against the map.
	 *
	 *  @param  str The string to match against.
	 *  @param  match If != 0, the position after the match will be stored here.
	 *  @return The index of the matched string in the original list, or kInvalidIndex if not found.
	 */
	size_t find(const UString &str, const char **match) const;

private:
	bool _onlyFirstWord;

	typedef boost::unordered_map<UString, size_t, hashUStringCaseInsensitive> StrMap;

	StrMap _map;

};

} // End of namespace Common

#endif // COMMON_STRWORDMAP_H
