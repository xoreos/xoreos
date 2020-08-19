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
 *  The global timer manager.
 */

#include "src/common/error.h"

#include "src/events/timerman.h"

DECLARE_SINGLETON(Events::TimerManager)

namespace Events {

TimerHandle::TimerHandle() : _empty(true) {
}

TimerHandle::~TimerHandle() {
	TimerMan.removeTimer(*this);
}


TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
	for (std::list<TimerID>::iterator t = _timers.begin(); t != _timers.end(); ++t)
		removeTimer(*t);
}

void TimerManager::init() {
}

void TimerManager::addTimer(uint32_t interval, TimerHandle &handle, const TimerFunc &func) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	removeTimer(handle);

	_timers.push_back(TimerID());

	std::list<TimerID>::iterator id = --_timers.end();

	handle._iterator = id;
	handle._empty    = false;

	id->_func = func;
	id->_id   = SDL_AddTimer(interval, &TimerManager::timerCallback, static_cast<void *>(&handle));
	if (id->_id)
		return;

	handle._iterator = _timers.end();
	handle._empty    = true;

	_timers.erase(id);

	throw Common::Exception("Failed to add timer");
}

void TimerManager::removeTimer(TimerHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (handle._empty)
		return;

	removeTimer(*handle._iterator);

	_timers.erase(handle._iterator);

	handle._iterator = _timers.end();
	handle._empty    = true;
}

void TimerManager::removeTimer(TimerID &id) {
	if (id._id == 0)
		return;

	SDL_RemoveTimer(id._id);
}

uint32_t TimerManager::timerCallback(uint32_t interval, void *data) {
	TimerHandle &handle = *static_cast<TimerHandle *>(data);

	if (handle._empty)
		return 0;

	uint32_t newInterval = handle._iterator->_func(interval);
	if (newInterval == 0) {
		handle._iterator->_id = 0;
		TimerMan.removeTimer(handle);
		return 0;
	}

	return newInterval;
}

} // End of namespace Events
