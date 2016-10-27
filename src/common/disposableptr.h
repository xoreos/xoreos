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
 *  A smart pointer with a deletion flag.
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

#ifndef COMMON_DISPOSABLEPTR_H
#define COMMON_DISPOSABLEPTR_H

#include <boost/noncopyable.hpp>

#include "src/common/system.h"
#include "src/common/types.h"
#include "src/common/deallocator.h"

namespace Common {

/** Base template class for a disposable pointer.
 *
 *  Manages the pointer and allows for optional, automatic deletion
 *  through a deallocator template parameter.
 */
template<typename T, class Deallocator>
class DisposablePtrBase : boost::noncopyable {
public:
	typedef T ValueType;
	typedef T *PointerType;
	typedef T &ReferenceType;

	explicit DisposablePtrBase(PointerType o, bool d) : _pointer(o), _dispose(d) {}

	/** Implicit conversion operator to bool for convenience, to make
	 *  checks like "if (disposablePtr) ..." possible.
	 */
	XOREOS_EXPLICIT_OPERATOR_CONV operator bool() const { return _pointer != 0; }

	~DisposablePtrBase() {
		if (_dispose)
			Deallocator::destroy(_pointer);
	}

	/** Resets the pointer with the new value. The old object will be destroyed. */
	void reset(PointerType o = 0) {
		if (_dispose)
			Deallocator::destroy(_pointer);

		_pointer = o;
	}

	/** Returns the plain pointer value.
	 *
	 *  @return the pointer the DisposablePtr manages.
	 */
	PointerType get() const { return _pointer; }

	/** Change the disposable flag. */
	void setDisposable(bool d) { _dispose = d; }

	/** Unconditionally dispose of the pointer, destroying the old object. */
	void dispose() {
		Deallocator::destroy(_pointer);
		_pointer = 0;
	}

private:
	PointerType _pointer;

	bool _dispose;
};

/** A disposable plain pointer, allowing pointer-y access and normal deletion. */
template<typename T, class Deallocator = DeallocatorDefault>
class DisposablePtr : public DisposablePtrBase<T, Deallocator> {
public:
	explicit DisposablePtr(typename DisposablePtrBase<T, Deallocator>::PointerType o, bool d) :
		DisposablePtrBase<T, Deallocator>(o, d) {
	}

	typename DisposablePtrBase<T, Deallocator>::ReferenceType operator*() const {
		return *DisposablePtrBase<T, Deallocator>::get();
	}

	typename DisposablePtrBase<T, Deallocator>::PointerType operator->() const {
		return DisposablePtrBase<T, Deallocator>::get();
	}
};

/** A disposable array pointer, allowing array-y access and array deletion. */
template<typename T, class Deallocator = DeallocatorArray>
class DisposableArray : public DisposablePtrBase<T, Deallocator> {
public:
	explicit DisposableArray(typename DisposablePtrBase<T, Deallocator>::PointerType o, bool d) :
		DisposablePtrBase<T, Deallocator>(o, d) {
	}

	typename DisposablePtrBase<T, Deallocator>::ReferenceType operator[](size_t i) const {
		return DisposablePtrBase<T, Deallocator>::get()[i];
	}
};

} // End of namespace Common

#endif // COMMON_DISPOSABLEPTR_H
