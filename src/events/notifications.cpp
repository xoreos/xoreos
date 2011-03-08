/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/notifications.cpp
 *  The notification manager, handling all notifications.
 */

#include "events/notifications.h"
#include "events/notifyable.h"

DECLARE_SINGLETON(Events::NotificationManager)

namespace Events {

NotificationManager::NotificationManager() {
}

NotificationManager::~NotificationManager() {
}

void NotificationManager::init() {
}

std::list<Notifyable *>::iterator NotificationManager::registerNotifyable(Notifyable &notifyable) {
	Common::StackLock lock(_mutex);

	_notifyables.push_back(&notifyable);

	return --_notifyables.end();
}

void NotificationManager::unregisterNotifyable(const std::list<Notifyable *>::iterator &it) {
	Common::StackLock lock(_mutex);

	_notifyables.erase(it);
}

void NotificationManager::resized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	Common::StackLock lock(_mutex);

	for (std::list<Notifyable *>::iterator it = _notifyables.begin(); it != _notifyables.end(); ++it)
		(*it)->notifyResized(oldWidth, oldHeight, newWidth, newHeight);
}

} // End of namespace Events
