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

#ifndef GRAPHICS_QUEUEMAN_H
#define GRAPHICS_QUEUEMAN_H

#include <list>

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/graphics/types.h"

namespace Graphics {

class Queueable;

/** The graphics queue manager. */
class QueueManager : public Common::Singleton<QueueManager> {
public:
	QueueManager();
	~QueueManager();

	bool isQueueEmpty(QueueType queue);

	void lockQueue(QueueType queue);
	void unlockQueue(QueueType queue);

	const std::list<Queueable *> &getQueue(QueueType queue) const;

	void sortQueue(QueueType queue);
	void clearQueue(QueueType queue);

	void clearAllQueues();

private:
	Common::Mutex _queueMutex[kQueueMAX];
	std::list<Queueable *> _queue[kQueueMAX];

	std::list<Queueable *>::iterator addToQueue(QueueType queue, Queueable &q);
	void removeFromQueue(QueueType queue, const std::list<Queueable *>::iterator &ref);

	friend class Queueable;
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics queue manager. */
#define QueueMan QueueManager::instance()

#endif // GRAPHICS_QUEUEMAN_H
