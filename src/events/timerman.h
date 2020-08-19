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

#ifndef EVENTS_TIMERMAN_H
#define EVENTS_TIMERMAN_H

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL_timer.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include <list>
#include <functional>

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/events/types.h"

namespace Events {

/** A timer callback function.
 *
 *  Its argument is the current calling interval.
 *  The return value is the new interval. 0 means the timer is stopped.
 */
typedef std::function<uint32_t (uint32_t)> TimerFunc;

class TimerID;
class TimerHandle;

/** The global timer manager.
 *
 *  Allows registering functions to be called at specific intervals.
 */
class TimerManager : public Common::Singleton<TimerManager> {
public:
	TimerManager();
	~TimerManager();

	void init();

	/** Add a function to be called regularly.
	 *
	 *  @param interval The interval in ms. The granularity is
	 *                  platform-dependent. The most common number is 10ms.
	 *  @param handle The timer handle to use.
	 *  @param func The function to call.
	 */
	void addTimer(uint32_t interval, TimerHandle &handle, const TimerFunc &func);

	/** Remove that timer function. */
	void removeTimer(TimerHandle &handle);

private:
	std::recursive_mutex _mutex;

	std::list<TimerID> _timers;

	void removeTimer(TimerID &id);

	static uint32_t timerCallback(uint32_t interval, void *data);
};

class TimerID {
private:
	SDL_TimerID _id;
	TimerFunc _func;

	friend class TimerManager;
};

class TimerHandle {
public:
	TimerHandle();
	~TimerHandle();

private:
	bool _empty;

	std::list<TimerID>::iterator _iterator;

	friend class TimerManager;
};

} // End of namespace Events

/** Shortcut for accessing the timer manager. */
#define TimerMan Events::TimerManager::instance()

#endif // EVENTS_TIMERMAN_H
