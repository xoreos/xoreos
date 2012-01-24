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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file events/notifications.h
 *  The notification manager, handling all notifications.
 */

#ifndef EVENTS_NOTIFICATIONS_H
#define EVENTS_NOTIFICATIONS_H

#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

#include "events/types.h"

namespace Events {

class Notifyable;

/** The notification manager, handling all notifications.
 *
 *  Notifications are handed out to all classes of the Notifyable type for
 *  special backend code that need to be reacted to by the client game code.
 *
 *  One example would be a change in the display resolution: GUI elements with
 *  an affinity to a screen edge or corner need to told that this change
 *  occured, so that they can reposition themselves to the new coordinates.
 */
class NotificationManager : public Common::Singleton<NotificationManager> {
public:
	NotificationManager();
	~NotificationManager();

	void init();

	/** Notify all Notifyables that the screen size changed. */
	void resized(int oldWidth, int oldHeight, int newWidth, int newHeight);
	/** Notify all Notifyables that the camera moved. */
	void cameraMoved();

private:
	Common::Mutex _mutex;

	std::list<Notifyable *> _notifyables;

	std::list<Notifyable *>::iterator registerNotifyable(Notifyable &notifyable);
	void unregisterNotifyable(const std::list<Notifyable *>::iterator &it);

	friend class Notifyable;
};

} // End of namespace Events

/** Shortcut for accessing the notification manager. */
#define NotificationMan Events::NotificationManager::instance()

#endif // EVENTS_NOTIFICATIONS_H
