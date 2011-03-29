/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/queueable.cpp
 *  An object that can be stored in a queue.
 */

#include "graphics/queueman.h"
#include "graphics/queueable.h"

namespace Graphics {

Queueable::Queueable() {
	for (int i = 0; i < kQueueMAX; i++)
		_isInQueue[i] = false;
}

Queueable::~Queueable() {
	removeFromAll();
}

bool Queueable::operator<(const Queueable &q) const {
	return false;
}

void Queueable::addToQueue(QueueType queue) {
	if (_isInQueue[queue])
		return;

	_queueRef[queue] = QueueMan.addToQueue(queue, *this);
	_isInQueue[queue] = true;
}

void Queueable::removeFromQueue(QueueType queue) {
	if (!_isInQueue[queue])
		return;

	QueueMan.removeFromQueue(queue, _queueRef[queue]);
	_isInQueue[queue] = false;
}

bool Queueable::isInQueue(QueueType queue) const {
	return _isInQueue[queue];
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
	_isInQueue[queue] = false;
}

} // End of namespace Graphics
