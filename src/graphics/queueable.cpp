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

#include "src/common/system.h"

#include "src/graphics/queueman.h"
#include "src/graphics/queueable.h"

namespace Graphics {

Queueable::Queueable() {
	for (int i = 0; i < kQueueMAX; i++)
		_isInQueue[i] = false;
}

Queueable::~Queueable() {
	removeFromAll();
}

bool Queueable::operator<(const Queueable &UNUSED(q)) const {
	return false;
}

void Queueable::addToQueue(QueueType queue) {
	QueueMan.lockQueue(queue);

	if (!_isInQueue[queue]) {
		_queueRef[queue] = QueueMan.addToQueue(queue, *this);
		_isInQueue[queue] = true;
	}

	QueueMan.unlockQueue(queue);
}

void Queueable::removeFromQueue(QueueType queue) {
	QueueMan.lockQueue(queue);

	if (_isInQueue[queue]) {
		QueueMan.removeFromQueue(queue, _queueRef[queue]);
		_isInQueue[queue] = false;
	}

	QueueMan.unlockQueue(queue);
}

void Queueable::lockQueue(QueueType queue) {
	QueueMan.lockQueue(queue);
}

void Queueable::unlockQueue(QueueType queue) {
	QueueMan.unlockQueue(queue);
}

void Queueable::sortQueue(QueueType queue) {
	QueueMan.lockQueue(queue);
	QueueMan.sortQueue(queue);
	QueueMan.unlockQueue(queue);
}

void Queueable::removeFromAll() {
	for (int i = 0; i < kQueueMAX; i++)
		removeFromQueue((QueueType) i);
}

void Queueable::kickedOut(QueueType queue) {
	QueueMan.lockQueue(queue);

	_isInQueue[queue] = false;

	QueueMan.unlockQueue(queue);
}

} // End of namespace Graphics
