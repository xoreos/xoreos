# xoreos - A reimplementation of BioWare's Aurora engine
#
# xoreos is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# xoreos is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# xoreos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xoreos. If not, see <http://www.gnu.org/licenses/>.

# Events (keyboard, mouse, joystick, timer, ...) subsystem.

noinst_LTLIBRARIES += src/events/libevents.la
src_events_libevents_la_SOURCES =

src_events_libevents_la_SOURCES += \
    src/events/events.h \
    src/events/types.h \
    src/events/requesttypes.h \
    src/events/requests.h \
    src/events/notifyable.h \
    src/events/notifications.h \
    src/events/timerman.h \
    src/events/joystick.h \
    $(EMPTY)

src_events_libevents_la_SOURCES += \
    src/events/events.cpp \
    src/events/requesttypes.cpp \
    src/events/requests.cpp \
    src/events/notifications.cpp \
    src/events/timerman.cpp \
    src/events/joystick.cpp \
    $(EMPTY)
