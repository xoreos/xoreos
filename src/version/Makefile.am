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

# Common support code used all over the codebase.

include $(top_srcdir)/Makefile.common

# Pass the git version information to the compiler

VER_REVDESC = $(shell cat $(top_builddir)/gitstamp/gitstamp)
VERSION_FLAGS = $(shell if test x$(VER_REVDESC) != "x"; then echo "-DXOREOS_REVDESC=\\\"$(VER_REVDESC)\\\""; fi)

VER_REV = $(shell cat $(top_builddir)/gitstamp/gitstamp | cut -d '+' -f 2-)
VERSION_FLAGS += $(shell if test x$(VER_REV) != "x"; then echo "-DXOREOS_REV=\\\"$(VER_REV)\\\""; fi)

VER_DATE = $(shell date -u '+%Y-%m-%dT%H:%M:%SZ')
VERSION_FLAGS += $(shell if test x$(VER_DATE) != "x"; then echo "-DXOREOS_BUILDDATE=\\\"$(VER_DATE)\\\""; fi)

AM_CXXFLAGS += $(VERSION_FLAGS)

# Rebuild the version.lo when the git version changed
version.lo: $(top_builddir)/gitstamp/gitstamp

noinst_LTLIBRARIES = libversion.la

noinst_HEADERS = \
                 version.h \
                 $(EMPTY)

libversion_la_SOURCES = \
                        version.cpp \
                        $(EMPTY)
