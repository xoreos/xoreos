/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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
	for (int i = 0; i < kNotificationMAX; i++)
		_notifications[i].store(false);
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

void NotificationManager::notify(Notification notification) {
	assert((notification >= 0) && (notification < kNotificationMAX));

	_notifications[notification].store(true);
}

void NotificationManager::handle() {
	Common::StackLock lock(_mutex);

	for (int i = 0; i < kNotificationMAX; i++)
		if (_notifications[i].exchange(false))
			for (std::list<Notifyable *>::iterator it = _notifyables.begin(); it != _notifyables.end(); ++it)
				(*it)->notify((Notification) i);
}

} // End of namespace Events
