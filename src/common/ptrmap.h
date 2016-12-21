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
 *  A map storing pointer to objects, with automatic deletion.
 */

#ifndef COMMON_PTRMAP_H
#define COMMON_PTRMAP_H

#include <functional>
#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/deallocator.h"

namespace Common {

/** A map of pointer to objects, with automatic deletion.
 *
 *  A PtrMap expects to hold sole ownership of its contents.
 */
template<typename Key, typename T, class Compare = std::less<Key>, class Deallocator = DeallocatorDefault>
class PtrMap : boost::noncopyable, public std::map<Key, T *, Compare> {
public:
	~PtrMap() {
		clear();
	}

	void clear() {
		for (typename std::map<Key, T *, Compare>::iterator it = std::map<Key, T *, Compare>::begin();
		     it != std::map<Key, T *, Compare>::end(); ++it)
			Deallocator::destroy(it->second);

		std::map<Key, T *, Compare>::clear();
	}

	void erase(typename std::map<Key, T *, Compare>::iterator position) {
		Deallocator::destroy(position->second);
		std::map<Key, T *, Compare>::erase(position);
	}

	typename std::map<Key, T *, Compare>::size_type
			erase(const typename std::map<Key, T *, Compare>::key_type &k) {

		typename std::map<Key, T *, Compare>::iterator it(std::map<Key, T *, Compare>::find(k));
		if (it == std::map<Key, T *, Compare>::end())
			return 0;

		erase(it);
		return 1;
	}

	void erase(typename std::map<Key, T *, Compare>::iterator first,
	           typename std::map<Key, T *, Compare>::iterator last) {

		for (typename std::map<Key, T *, Compare>::iterator it = std::map<Key, T *, Compare>::begin();
		     it != std::map<Key, T *, Compare>::end(); ++it)
			Deallocator::destroy(it->second);

		std::map<Key, T *, Compare>::erase(first, last);
	}
};

} // End of namespace Common

#endif // COMMON_PTRMAP_H
