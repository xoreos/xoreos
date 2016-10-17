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
 *  Simple deallocator concept.
 */

#ifndef COMMON_DEALLOCATOR_H
#define COMMON_DEALLOCATOR_H

#include <cstdlib>

namespace Common {

/* Based on Boost's checked_delete. Makes sure the type is complete
 * before deletion, becaused deleting an object with incomplete type
 * is undefined behaviour.
 */

/** Deallocate a normal pointer. */
struct DeallocatorDefault {
	template<typename T>
	static void destroy(T *x) {
		// intentionally complex - simplification causes regressions
		typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
		(void) sizeof(type_must_be_complete);
		delete x;
	}
};

/** Deallocate a pointer to an array. */
struct DeallocatorArray {
	template<typename T>
	static void destroy(T *x) {
		// intentionally complex - simplification causes regressions
		typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
		(void) sizeof(type_must_be_complete);
		delete[] x;
	}
};

/** Deallocate a pointer using free(). */
struct DeallocatorFree {
	template<typename T>
	static void destroy(T *x) {
		free(x);
	}
};

} // End of namespace Common

#endif // COMMON_DEALLOCATOR_H
