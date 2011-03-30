/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/timerman.cpp
 *  The global timer manager.
 */

#include "common/error.h"

#include "events/timerman.h"

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

void TimerManager::addTimer(uint32 interval, TimerHandle &handle, const TimerFunc &func) {
	Common::StackLock lock(_mutex);

	removeTimer(handle);

	_timers.push_back(TimerID());

	std::list<TimerID>::iterator id = --_timers.end();

	handle._iterator = id;
	handle._empty    = false;

	id->_func = func;
	id->_id   = SDL_AddTimer(interval, &TimerManager::timerCallback, (void *) &handle);
	if (id->_id)
		return;

	handle._iterator = _timers.end();
	handle._empty    = true;

	_timers.erase(id);

	throw Common::Exception("Failed to add timer");
}

void TimerManager::removeTimer(TimerHandle &handle) {
	Common::StackLock lock(_mutex);

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

uint32 TimerManager::timerCallback(uint32 interval, void *data) {
	TimerHandle &handle = *((TimerHandle *) data);

	if (handle._empty)
		return 0;

	uint32 newInterval = handle._iterator->_func(interval);
	if (newInterval == 0) {
		handle._iterator->_id = 0;
		TimerMan.removeTimer(handle);
		return 0;
	}

	return newInterval;
}

} // End of namespace Events
