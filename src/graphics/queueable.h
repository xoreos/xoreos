/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "graphics/types.h"

namespace Graphics {

class Queueable {
public:
	Queueable();
	virtual ~Queueable();

	virtual bool operator<(const Queueable &q) const;

protected:
	void addToQueue(QueueType queue);
	void removeFromQueue(QueueType queue);
	bool isInQueue(QueueType queue) const;

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
