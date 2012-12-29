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
