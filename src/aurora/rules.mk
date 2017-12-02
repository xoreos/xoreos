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

# Aurora file loaders and related support code.

noinst_LTLIBRARIES += src/aurora/libaurora.la
src_aurora_libaurora_la_SOURCES =

src_aurora_libaurora_la_SOURCES += \
    src/aurora/types.h \
    src/aurora/util.h \
    src/aurora/language.h \
    src/aurora/language_strings.h \
    src/aurora/archive.h \
    src/aurora/aurorafile.h \
    src/aurora/keyfile.h \
    src/aurora/biffile.h \
    src/aurora/bzffile.h \
    src/aurora/erffile.h \
    src/aurora/rimfile.h \
    src/aurora/ndsrom.h \
    src/aurora/zipfile.h \
    src/aurora/resman.h \
    src/aurora/talktable.h \
    src/aurora/talktable_tlk.h \
    src/aurora/talktable_gff.h \
    src/aurora/talkman.h \
    src/aurora/ssffile.h \
    src/aurora/2dafile.h \
    src/aurora/gdafile.h \
    src/aurora/gdaheaders.h \
    src/aurora/2dareg.h \
    src/aurora/locstring.h \
    src/aurora/gff3file.h \
    src/aurora/gff4file.h \
    src/aurora/gff4fields.h \
    src/aurora/dlgfile.h \
    src/aurora/lytfile.h \
    src/aurora/visfile.h \
    src/aurora/ifofile.h \
    src/aurora/pefile.h \
    src/aurora/herffile.h \
    src/aurora/smallfile.h \
    src/aurora/nitrofile.h \
    src/aurora/nsbtxfile.h \
    src/aurora/cdpth.h \
    src/aurora/nfofile.h \
    $(EMPTY)

src_aurora_libaurora_la_SOURCES += \
    src/aurora/util.cpp \
    src/aurora/language.cpp \
    src/aurora/archive.cpp \
    src/aurora/aurorafile.cpp \
    src/aurora/keyfile.cpp \
    src/aurora/biffile.cpp \
    src/aurora/bzffile.cpp \
    src/aurora/erffile.cpp \
    src/aurora/rimfile.cpp \
    src/aurora/ndsrom.cpp \
    src/aurora/zipfile.cpp \
    src/aurora/resman.cpp \
    src/aurora/talktable.cpp \
    src/aurora/talktable_tlk.cpp \
    src/aurora/talktable_gff.cpp \
    src/aurora/talkman.cpp \
    src/aurora/ssffile.cpp \
    src/aurora/2dafile.cpp \
    src/aurora/gdafile.cpp \
    src/aurora/gdaheaders.cpp \
    src/aurora/2dareg.cpp \
    src/aurora/locstring.cpp \
    src/aurora/gff3file.cpp \
    src/aurora/gff4file.cpp \
    src/aurora/dlgfile.cpp \
    src/aurora/lytfile.cpp \
    src/aurora/visfile.cpp \
    src/aurora/ifofile.cpp \
    src/aurora/pefile.cpp \
    src/aurora/herffile.cpp \
    src/aurora/smallfile.cpp \
    src/aurora/nitrofile.cpp \
    src/aurora/nsbtxfile.cpp \
    src/aurora/cdpth.cpp \
    src/aurora/nfofile.cpp \
    $(EMPTY)

src_aurora_libaurora_la_LIBADD = \
    src/aurora/nwscript/libnwscript.la \
    src/aurora/lua/libluascript.la \
    $(EMPTY)

# Subdirectories

include src/aurora/nwscript/rules.mk
include src/aurora/lua/rules.mk
