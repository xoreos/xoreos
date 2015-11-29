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
 *  The graphics queue manager.
 */

#include "src/graphics/queueman.h"
#include "src/graphics/queueable.h"

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

bool QueueManager::isQueueEmpty(QueueType queue) {
	Common::StackLock lock(_queueMutex[queue]);

	return _queue[queue].empty();
}

const std::list<Queueable *> &QueueManager::getQueue(QueueType queue) const {
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
