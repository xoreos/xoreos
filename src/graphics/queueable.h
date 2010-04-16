/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/queueable.h
 *  An object that can be stored in a queue.
 */

#ifndef GRAPHICS_QUEUEABLE_H
#define GRAPHICS_QUEUEABLE_H

#include <list>

#include "common/mutex.h"

namespace Graphics {

template<class T>
class Queueable {
public:
	struct Queue {
		typedef std::list<T *> QueueList;

		QueueList list;
		Common::Mutex mutex;
	};

	typedef typename Queue::QueueList::iterator QueueRef;

	Queueable(Queue &queue) : _inQueue(false), _queue(&queue) {
	}

	~Queueable() {
		removeFromQueue();
	}

	/** Notify the object that it has been kicked out of its queue. */
	void kickedOut() {
		_inQueue = false;
	}

protected:
	/** Add the object to its queue. */
	void addToQueue() {
		if (_inQueue)
			return;

		Common::StackLock lock(_queue->mutex);

		_queue->list.push_back((T *) this);
		_queueRef = --_queue->list.end();

		_inQueue = true;
	}

	/** Remove the object from its queue. */
	void removeFromQueue() {
		if (!_inQueue)
			return;

		Common::StackLock lock(_queue->mutex);

		_queue->list.erase(_queueRef);

		_inQueue = false;
	}

private:
	bool _inQueue;

	Queue *_queue;
	QueueRef _queueRef;
};

} // End of namespace Graphics

#endif // GRAPHICS_QUEUEABLE_H
