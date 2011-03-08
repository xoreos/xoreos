/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
