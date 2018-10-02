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

# Unit tests for the Engines namespace.

engines_LIBS = \
    $(test_LIBS) \
    src/engines/libengines.la \
    src/graphics/libgraphics.la \
    src/aurora/libaurora.la \
    src/common/libcommon.la \
    src/events/libevents.la \
    tests/version/libversion.la \
    $(LDADD)

check_PROGRAMS                     += tests/engines/test_trigger
tests_engines_test_trigger_SOURCES  = tests/engines/trigger.cpp
tests_engines_test_trigger_LDADD    = $(engines_LIBS)
tests_engines_test_trigger_CXXFLAGS = $(test_CXXFLAGS)
