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

#include "src/common/semaphore.h"

namespace Common {

Semaphore::Semaphore(uint value) {
	_semaphore = SDL_CreateSemaphore(value);
}

Semaphore::~Semaphore() {
	SDL_DestroySemaphore(_semaphore);
}

bool Semaphore::lock(uint32_t timeout) {
	int ret;

	if (timeout == 0)
		ret = SDL_SemWait(_semaphore);
	else
		ret = SDL_SemWaitTimeout(_semaphore, timeout);

	return ret == 0;
}

bool Semaphore::lockTry() {
	return SDL_SemTryWait(_semaphore) == 0;
}

void Semaphore::unlock() {
	SDL_SemPost(_semaphore);
}

uint32_t Semaphore::getValue() {
	return SDL_SemValue(_semaphore);
}

} // End of namespace Common
