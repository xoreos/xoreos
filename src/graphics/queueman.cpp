/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/queueman.cpp
 *  The graphics queue manager.
 */

#include "graphics/queueman.h"
#include "graphics/queueable.h"

DECLARE_SINGLETON(Graphics::QueueManager)

namespace Graphics {

static bool queueComp(Queueable *a, Queueable *b) {
	return *a < *b;
}


QueueManager::QueueManager() {
}

QueueManager::~QueueManager() {
	clearAllQueues();
}

void QueueManager::lockQueue(QueueType queue) {
	_queueMutex[queue].lock();
}

void QueueManager::unlockQueue(QueueType queue) {
	_queueMutex[queue].unlock();
}

const std::list<Queueable *> &QueueManager::getQueue(QueueType queue) {
	return _queue[queue];
}

void QueueManager::sortQueue(QueueType queue) {
	lockQueue(queue);

	_queue[queue].sort(queueComp);

	unlockQueue(queue);
}

std::list<Queueable *>::iterator QueueManager::addToQueue(QueueType queue, Queueable &q) {
	lockQueue(queue);

	_queue[queue].push_back(&q);
	std::list<Queueable *>::iterator ref = --_queue[queue].end();

	unlockQueue(queue);

	return ref;
}

void QueueManager::removeFromQueue(QueueType queue,
		const std::list<Queueable *>::iterator &ref) {

	lockQueue(queue);

	_queue[queue].erase(ref);

	unlockQueue(queue);
}

void QueueManager::clearQueue(QueueType queue) {
	lockQueue(queue);

	for (std::list<Queueable *>::iterator q = _queue[queue].begin();
	     q != _queue[queue].end(); ++q)
		(*q)->kickedOut(queue);

	_queue[queue].clear();

	unlockQueue(queue);
}

void QueueManager::clearAllQueues() {
	for (int i = 0; i < kQueueMAX; i++)
		clearQueue((QueueType) i);
}

} // End of namespace Graphics
