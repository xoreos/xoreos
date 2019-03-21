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

# Unit tests for the NWN2 namespace.

nwn2_LIBS = \
    $(test_LIBS) \
    src/engines/nwn2/libnwn2.la \
    src/engines/libengines.la \
    src/events/libevents.la \
    src/video/libvideo.la \
    src/sound/libsound.la \
    src/graphics/libgraphics.la \
    src/aurora/libaurora.la \
    src/common/libcommon.la \
    tests/version/libversion.la \
    $(LDADD)

check_PROGRAMS                           += tests/engines/nwn2/test_creature
tests_engines_nwn2_test_creature_SOURCES  = tests/engines/nwn2/creature.cpp
tests_engines_nwn2_test_creature_LDADD    = $(nwn2_LIBS)
tests_engines_nwn2_test_creature_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/engines/nwn2/test_door
tests_engines_nwn2_test_door_SOURCES      = tests/engines/nwn2/door.cpp
tests_engines_nwn2_test_door_LDADD        = $(nwn2_LIBS)
tests_engines_nwn2_test_door_CXXFLAGS     = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/engines/nwn2/test_roster
tests_engines_nwn2_test_roster_SOURCES    = tests/engines/nwn2/roster.cpp
tests_engines_nwn2_test_roster_LDADD      = $(nwn2_LIBS)
tests_engines_nwn2_test_roster_CXXFLAGS   = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/engines/nwn2/test_waypoint
tests_engines_nwn2_test_waypoint_SOURCES  = tests/engines/nwn2/waypoint.cpp
tests_engines_nwn2_test_waypoint_LDADD    = $(nwn2_LIBS)
tests_engines_nwn2_test_waypoint_CXXFLAGS = $(test_CXXFLAGS)
