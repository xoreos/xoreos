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

# mingw-std-threads (<https://github.com/meganz/mingw-std-threads/>).

noinst_HEADERS += \
    external/mingw-std-threads/mingw.condition_variable.h \
    external/mingw-std-threads/mingw.future.h \
    external/mingw-std-threads/mingw.mutex.h \
    external/mingw-std-threads/mingw.shared_mutex.h \
    external/mingw-std-threads/mingw.thread.h \
    external/mingw-std-threads/mingw.invoke.h \
    $(EMPTY)

EXTRA_DIST += \
    external/mingw-std-threads/README.md \
    external/mingw-std-threads/LICENSE \
    $(EMPTY)
