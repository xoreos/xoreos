/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/timerman.h
 *  The global timer manager.
 */

#ifndef EVENTS_TIMERMAN_H
#define EVENTS_TIMERMAN_H

#include <SDL_timer.h>

#include <list>

#include "boost/function.hpp"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

#include "events/types.h"

namespace Events {

/** A timer callback function.
 *
 *  Its argument is the current calling interval.
 *  The return value is the new interval. 0 means the timer is stopped.
 */
typedef boost::function<uint32 (uint32)> TimerFunc;

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

	/** Add a function to be called regularily.
	 *
	 *  @param interval The interval in ms. The granularity is
	 *                  platform-dependent. The most common number is 10ms.
	 *  @param handle The timer handle to use.
	 *  @param func The function to call.
	 */
	void addTimer(uint32 interval, TimerHandle &handle, const TimerFunc &func);

	/** Remove that timer function. */
	void removeTimer(TimerHandle &handle);

private:
	Common::Mutex _mutex;

	std::list<TimerID> _timers;

	void removeTimer(TimerID &id);

	static uint32 timerCallback(uint32 interval, void *data);
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
