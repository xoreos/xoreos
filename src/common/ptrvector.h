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
 *  A vector storing pointer to objects, with automatic deletion.
 */

#ifndef COMMON_PTRVECTOR_H
#define COMMON_PTRVECTOR_H

#include <vector>

#include <boost/noncopyable.hpp>

#include "src/common/deallocator.h"

namespace Common {

/** A vector of pointer to objects, with automatic deletion.
 *
 *  A PtrVector expects to hold sole ownership of its contents.
 *
 *  Note: when overwriting elements in a PtrVector using operator[],
 *  no automatic deletion can take place!
 */
template<typename T, class Deallocator = DeallocatorDefault>
class PtrVector : boost::noncopyable, public std::vector<T *> {
public:
	~PtrVector() {
		clear();
	}

	void clear() {
		for (typename std::vector<T *>::iterator it = std::vector<T *>::begin(); it != std::vector<T *>::end(); ++it)
			Deallocator::destroy(*it);

		std::vector<T *>::clear();
	}

	void pop_back() {
		Deallocator::destroy(std::vector<T *>::back());
		std::vector<T *>::pop_back();
	}

	typename std::vector<T *>::iterator erase(typename std::vector<T *>::iterator position) {
		Deallocator::destroy(*position);
		return std::vector<T *>::erase(position);
	}

	typename std::vector<T *>::iterator erase(typename std::vector<T *>::iterator first,
	                                        typename std::vector<T *>::iterator last) {

		for (typename std::vector<T *>::iterator it = std::vector<T *>::begin(); it != std::vector<T *>::end(); ++it)
			Deallocator::destroy(*it);

		return std::vector<T *>::erase(first, last);
	}

	void resize(typename std::vector<T *>::size_type n,
	            typename std::vector<T *>::value_type val = typename std::vector<T *>::value_type()) {

		typename std::vector<T *>::size_type s = std::vector<T *>::size();

		if (s < n) {
			while (s++ < n)
				std::vector<T *>::push_back(val);

			return;
		}

		while (s-- > n)
			pop_back();
	}

	template<class InputIterator>
	void assign(InputIterator first, InputIterator last) {
		clear();
		std::vector<T *>::assign(first, last);
	}

	void assign(typename std::vector<T *>::size_type n, const typename std::vector<T *>::value_type &val) {
		clear();
		std::vector<T *>::assign(n, val);
	}
};

} // End of namespace Common

#endif // COMMON_PTRVECTOR_H
