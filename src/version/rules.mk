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

# Project version information, including git repository information.

# Query the git repository (if available) and ask for:
# - The last description tag
# - The current revision hash (shortened)
# - The number of commits between the last tag and HEAD
# - Is the repository checkout dirty?
#
# Then put all this information into the gitstamp file, but only if
# if the current contents of the file does not different from what
# we have just read from the repository. This guarantees that the
# version file is only recompiled if the repository information
# changed.

GITSTAMP_FILE = src/version/gitstamp

.PHONY: gitstamp
gitstamp:
	@-$(eval $@_REVLINE := $(shell cd $(srcdir); git describe --long --match desc/\* 2>/dev/null | cut -d '/' -f 2- | sed -e 's/\(.*\)-\([^-]*\)-\([^-]*\)/\1+\2.\3/'))
	@-$(eval $@_REVDIRT := $(shell cd $(srcdir); git update-index --refresh --unmerged 1>/dev/null 2>&1; git diff-index --quiet HEAD 2>/dev/null || echo ".dirty"))
	@-$(eval $@_REVDESC := $(shell if test x$($@_REVLINE) != "x"; then echo $($@_REVLINE)$($@_REVDIRT); fi ))
	@[ -f $(GITSTAMP_FILE) ] || touch $(GITSTAMP_FILE)
	@echo $($@_REVDESC) | cmp -s $(GITSTAMP_FILE) - || ( echo "  GEN      $(GITSTAMP_FILE) ($($@_REVDESC))" ; echo $($@_REVDESC) > $(GITSTAMP_FILE) )

CLEANFILES += $(GITSTAMP_FILE)

# Make sure the gitstamp file is always generated. This is a bit of
# a hack and applies to all targets, including clean. There is
# unfortunately no other way that guarantees that the file is
# generated before any other rule is executed.
-include gitstamp

# Now get the information out of the gitstamp file again and store
# it in the these variables.

VERSION_REVDESC = $(shell cat $(GITSTAMP_FILE))
VERSION_REV     = $(shell cat $(GITSTAMP_FILE) | cut -d '+' -f 2-)

# Get the current date and time. But we also heed SOURCE_DATE_EPOCH
# for reproducible builds.

DATE_FMT           = %Y-%m-%dT%H:%M:%SZ
SOURCE_DATE_EPOCH ?= $(shell date "+%s")

VERSION_DATE = $(shell date -u -d "@$(SOURCE_DATE_EPOCH)" "+$(DATE_FMT)"  2>/dev/null || date -u -r "$(SOURCE_DATE_EPOCH)" "+$(DATE_FMT)" 2>/dev/null || date -u "+$(DATE_FMT)")

# And assemble this version data into a compiler command line.

VERSION_FLAGS  =
VERSION_FLAGS += $(shell if test x$(VERSION_REVDESC) != "x"; then echo "-DXOREOS_REVDESC=\\\"$(VERSION_REVDESC)\\\""; fi)
VERSION_FLAGS += $(shell if test x$(VERSION_REV) != "x"; then echo "-DXOREOS_REV=\\\"$(VERSION_REV)\\\""; fi)
VERSION_FLAGS += $(shell if test x$(VERSION_DATE) != "x"; then echo "-DXOREOS_BUILDDATE=\\\"$(VERSION_DATE)\\\""; fi)

# Add the version flags to the build flags.
# This is the global build flags variable, though. Usually, this
# should go into src_version_libversion_la_CXXFLAGS, i.e. the build
# flags variable for this library. However, when there are library
# build flags, automake will rename the object files to include the
# library name (so prevent linker issues due to mismatched flags)
# and for some reason, the following dependency override rule doesn't
# work for the renamed src_version_libversion-version.lo.
AM_CXXFLAGS += $(VERSION_FLAGS)

# Make sure version.cpp is recompiled when the gitstamp file changes.
# This is a bit janky as well, because it redefines an automake rule
# without providing a recipe. Interestingly, it still works...
src/version/version.lo: $(GITSTAMP_FILE) src/version/version.cpp src/version/version.h

noinst_LTLIBRARIES += src/version/libversion.la
src_version_libversion_la_SOURCES =

src_version_libversion_la_SOURCES += \
    src/version/version.h \
    $(EMPTY)

src_version_libversion_la_SOURCES += \
    src/version/version.cpp \
    $(EMPTY)
