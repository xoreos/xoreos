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
 *  An object that can be stored in a queue.
 */

#ifndef GRAPHICS_QUEUEABLE_H
#define GRAPHICS_QUEUEABLE_H

#include <list>

#include "src/graphics/types.h"

namespace Graphics {

class Queueable {
public:
	Queueable();
	virtual ~Queueable();

	virtual bool operator<(const Queueable &q) const;

protected:
	bool isInQueue(QueueType queue) const {
		return _isInQueue[queue];
	}

	void addToQueue(QueueType queue);
	void removeFromQueue(QueueType queue);

	void lockQueue(QueueType queue);
	void unlockQueue(QueueType queue);
	void sortQueue(QueueType queue);

private:
	bool _isInQueue[kQueueMAX];
	std::list<Queueable *>::iterator _queueRef[kQueueMAX];

	void removeFromAll();
	void kickedOut(QueueType queue);

	friend class QueueManager;
};

} // End of namespace Graphics

#endif // GRAPHICS_QUEUEABLE_H
