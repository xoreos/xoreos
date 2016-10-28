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
 *  A list storing pointer to objects, with automatic deletion.
 */

#ifndef COMMON_PTRLIST_H
#define COMMON_PTRLIST_H

#include <list>

#include <boost/noncopyable.hpp>

#include "src/common/deallocator.h"

namespace Common {

/** A list of pointer to objects, with automatic deletion.
 *
 *  A PtrList expects to hold sole ownership of its contents.
 */
template<typename T, class Deallocator = DeallocatorDefault>
class PtrList : boost::noncopyable, public std::list<T *> {
public:
	~PtrList() {
		clear();
	}

	void clear() {
		for (typename std::list<T *>::iterator it = std::list<T *>::begin(); it != std::list<T *>::end(); ++it)
			Deallocator::destroy(*it);

		std::list<T *>::clear();
	}

	void pop_front() {
		Deallocator::destroy(std::list<T *>::front());
		std::list<T *>::pop_front();
	}

	void pop_back() {
		Deallocator::destroy(std::list<T *>::back());
		std::list<T *>::pop_back();
	}

	typename std::list<T *>::iterator erase(typename std::list<T *>::iterator position) {
		Deallocator::destroy(*position);
		return std::list<T *>::erase(position);
	}

	typename std::list<T *>::iterator erase(typename std::list<T *>::iterator first,
	                                        typename std::list<T *>::iterator last) {

		for (typename std::list<T *>::iterator it = std::list<T *>::begin(); it != std::list<T *>::end(); ++it)
			Deallocator::destroy(*it);

		return std::list<T *>::erase(first, last);
	}

	void resize(typename std::list<T *>::size_type n,
	            typename std::list<T *>::value_type val = typename std::list<T *>::value_type()) {

		typename std::list<T *>::size_type s = std::list<T *>::size();

		if (s < n) {
			while (s++ < n)
				std::list<T *>::push_back(val);

			return;
		}

		while (s-- > n)
			pop_back();
	}

	void remove(const typename std::list<T *>::value_type &val) {
		for (typename std::list<T *>::iterator it = std::list<T *>::begin(); it != std::list<T *>::end(); ) {
			if (*it == val)
				it = erase(it);
			else
				++it;
		}
	}

	template<class Predicate>
	void remove_if(Predicate pred) {
		for (typename std::list<T *>::iterator it = std::list<T *>::begin(); it != std::list<T *>::end(); ) {
			if (pred(*it))
				it = erase(it);
			else
				++it;
		}
	}

	template<class InputIterator>
	void assign(InputIterator first, InputIterator last) {
		clear();
		std::list<T *>::assign(first, last);
	}

	void assign(typename std::list<T *>::size_type n, const typename std::list<T *>::value_type &val) {
		clear();
		std::list<T *>::assign(n, val);
	}
};

} // End of namespace Common

#endif // COMMON_PTRLIST_H
