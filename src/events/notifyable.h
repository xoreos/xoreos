/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A class that can be notified by the NotificationManager.
 */

#ifndef EVENTS_NOTIFYABLE_H
#define EVENTS_NOTIFYABLE_H

#include <list>

#include "src/common/system.h"
#include "src/events/notifications.h"

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
	virtual void notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                             int UNUSED(newWidth), int UNUSED(newHeight)) {
	}
	virtual void notifyCameraMoved() {
	}

	friend class NotificationManager;
};

} // End of namespace Events

#endif // EVENTS_NOTIFYABLE_H
