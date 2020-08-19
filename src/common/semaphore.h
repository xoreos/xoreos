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
 *  Thread semaphore class.
 */

#ifndef COMMON_SEMAPHORE_H
#define COMMON_SEMAPHORE_H

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL_thread.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include <boost/noncopyable.hpp>

#include "src/common/types.h"

namespace Common {

/** A semaphore . */
class Semaphore : boost::noncopyable {
public:
	Semaphore(uint value = 0);
	~Semaphore();

	bool lock(uint32_t timeout = 0);
	bool lockTry();
	void unlock();

	uint32_t getValue();

private:
	SDL_sem *_semaphore;
};

} // End of namespace Common

#endif // COMMON_SEMAPHORE_H
