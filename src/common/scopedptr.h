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
 *  A simple scoped smart pointer template.
 */

/* Based on Cabal (<https://github.com/project-cabal/cabal/>) code,
 * which in turn is based on ScummVM (<http://scummvm.org>) code,
 * which is released under the terms of version 2 or later of the
 * GNU General Public License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef COMMON_SCOPEDPTR_H
#define COMMON_SCOPEDPTR_H

#include <boost/noncopyable.hpp>

#include "src/common/system.h"
#include "src/common/types.h"
#include "src/common/deallocator.h"
#include "src/common/util.h"

namespace Common {

/** Base template class for a scoped pointer.
 *
 *  Manages the pointer and allows for automatic deletion throw a
 *  deallocator template parameter.
 */
template<typename T, class Deallocator>
class ScopedPtrBase : boost::noncopyable {
public:
	typedef T ValueType;
	typedef T *PointerType;
	typedef T &ReferenceType;

	explicit ScopedPtrBase(PointerType o = 0) : _pointer(o) {}

	/** Implicit conversion operator to bool for convenience, to make
	 *  checks like "if (scopedPtr) ..." possible.
	 */
	XOREOS_EXPLICIT_OPERATOR_CONV operator bool() const { return _pointer != 0; }

	~ScopedPtrBase() {
		Deallocator::destroy(_pointer);
	}

	/** Resets the pointer with the new value. The old object will be destroyed. */
	void reset(PointerType o = 0) {
		Deallocator::destroy(_pointer);
		_pointer = o;
	}

	/** Returns the plain pointer value.
	 *
	 *  @return the pointer the ScopedPtr manages.
	 */
	PointerType get() const { return _pointer; }

	/** Returns the plain pointer value and releases ScopedPtr.
	 *  After the release() call, you need to delete the object yourself.
	 *
	 *  @return the pointer the ScopedPtr manages.
	 */
	PointerType release() {
		PointerType r = _pointer;
		_pointer = 0;
		return r;
	}

	/** Swap the managed pointers of two ScopedPtrs of the same type. */
	void swap(ScopedPtrBase<T, Deallocator> &right) {
		SWAP(_pointer, right._pointer);
	}

private:
	PointerType _pointer;
};

/** A scoped plain pointer, allowing pointer-y access and normal deletion. */
template<typename T, class Deallocator = DeallocatorDefault>
class ScopedPtr : public ScopedPtrBase<T, Deallocator> {
public:
	explicit ScopedPtr(typename ScopedPtrBase<T, Deallocator>::PointerType o = 0) :
		ScopedPtrBase<T, Deallocator>(o) {
	}

	typename ScopedPtrBase<T, Deallocator>::ReferenceType operator*() const {
		return *ScopedPtrBase<T, Deallocator>::get();
	}

	typename ScopedPtrBase<T, Deallocator>::PointerType operator->() const {
		return ScopedPtrBase<T, Deallocator>::get();
	}
};

/** A scoped array pointer, allowing array-y access and array deletion. */
template<typename T, class Deallocator = DeallocatorArray>
class ScopedArray : public ScopedPtrBase<T, Deallocator> {
public:
	explicit ScopedArray(typename ScopedPtrBase<T, Deallocator>::PointerType o = 0) :
		ScopedPtrBase<T, Deallocator>(o) {
	}

	typename ScopedPtrBase<T, Deallocator>::ReferenceType operator[](size_t i) const {
		return ScopedPtrBase<T, Deallocator>::get()[i];
	}
};

} // End of namespace Common

#endif // COMMON_SCOPEDPTR_H
