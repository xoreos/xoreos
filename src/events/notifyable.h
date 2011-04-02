/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file events/notifyable.h
 *  A class that can be notified by the NotificationManager.
 */

#ifndef EVENTS_NOTIFYABLE_H
#define EVENTS_NOTIFYABLE_H

#include <list>

#include "events/notifications.h"

namespace Events {

class Notifyable {
private:
	std::list<Notifyable *>::iterator _listIterator;

public:
	Notifyable() {
		_listIterator = NotificationMan.registerNotifyable(*this);
	}

	virtual ~Notifyable() {
		NotificationMan.unregisterNotifyable(_listIterator);
	}

private:
	virtual void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	}
	virtual void notifyCameraMoved() {
	}

	friend class NotificationManager;
};

} // End of namespace Events

#endif // EVENTS_NOTIFYABLE_H
