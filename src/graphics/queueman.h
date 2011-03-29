/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/queueman.h
 *  The graphics queue manager.
 */

#ifndef QUEUEMANAGER_QUEUEMANAGER_H
#define QUEUEMANAGER_QUEUEMANAGER_H

#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

#include "graphics/types.h"

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

	const std::list<Queueable *> &getQueue(QueueType queue);

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

#endif // QUEUEMANAGER_QUEUEMANAGER_H
