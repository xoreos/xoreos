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

# Automake hack to extract git repository versioning information.

AM_LDFLAGS =
LIBS =

# Get git version information
.PHONY: gitstamp
gitstamp:
	@-$(eval $@_REVLINE := $(shell cd $(top_srcdir); git describe --long --match desc/\* | cut -d '/' -f 2- | sed -e 's/\(.*\)-\([^-]*\)-\([^-]*\)/\1+\2.\3/'))
	@-$(eval $@_REVDIRT := $(shell cd $(top_srcdir); git update-index --refresh --unmerged 1>/dev/null 2>&1; git diff-index --quiet HEAD || echo ".dirty"))
	@-$(eval $@_REVDESC := $(shell if test x$($@_REVLINE) != "x"; then echo $($@_REVLINE)$($@_REVDIRT); fi ))
	@[ -f $@ ] || touch $@
	$(AM_V_GEN)echo $($@_REVDESC) | cmp -s $@ - || echo $($@_REVDESC) > $@

clean-local:
	rm -f gitstamp

noinst_LTLIBRARIES = libgitstamp.la

libgitstamp_la_SOURCES =

libgitstamp_la_DEPENDENCIES = gitstamp

libgitstamp_la_LINK = true
