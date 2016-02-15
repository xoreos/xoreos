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
 *  Simple utility template for searching through static const maps.
 */

#ifndef COMMON_BINSEARCH_H
#define COMMON_BINSEARCH_H

#include <cstddef>

namespace Common {

/** Struct template for a generic searchable key/value pair. */
template<typename TK, typename TV>
struct BinSearchValue {
	TK key;
	TV value;
};

/** Search through this sorted list of key/value pairs. */
template<typename TK, typename TV>
const BinSearchValue<TK, TV> *binarySearch(const BinSearchValue<TK, TV> *map, size_t size, const TK &value) {
	size_t low = 0, high = size - 1, midpoint = 0;

	while ((low <= high) && (high < size) && (low < size)) {
		midpoint = low + ((high - low) / 2);

		if      (value == map[midpoint].key)
			return &map[midpoint];
		else if (value <  map[midpoint].key)
			high = midpoint - 1;
		else
			low  = midpoint + 1;
	}

	return 0;
}

} // End of namespace Common

#endif // COMMON_BINSEARCH_H
