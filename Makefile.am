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

# The main "controller" automake recipe. We're setting up all our
# global build variables here, then recursively include all other
# build recipe, creating a flat non-recursive automake setup.

# Use the m4 directory for extra m4 autoconf scripts.
ACLOCAL_AMFLAGS = -I m4 --install
# Build objects in the same directory as their source files.
AUTOMAKE_OPTIONS = subdir-objects

# Empty element to make the source lists easier editable.
EMPTY =

# Initialize the global lists.

noinst_HEADERS     =
noinst_LTLIBRARIES =

bin_PROGRAMS =

check_LTLIBRARIES =
check_PROGRAMS    =
TESTS             =

CLEANFILES =

EXTRA_DIST     =
dist_doc_DATA  =
dist_man6_MANS =

# Library compile flags

LIBSF_XOREOS  = $(XOREOS_CFLAGS)
LIBSF_GENERAL = $(ZLIB_CFLAGS) $(LZMA_FLAGS) $(XML2_CFLAGS)
LIBSF_GRAPHIC = $(SDL2_CFLAGS) $(GL_CFLAGS) $(GLEW_CFLAGS) \
                $(FT2_CFLAGS) $(XVID_CFLAGS) $(VPX_CFLAGS)
LIBSF_SOUND   = $(AL_CFLAGS) $(FAAD_CFLAGS) $(MAD_CFLAGS) \
                $(OGG_CFLAGS) $(VORBIS_CFLAGS)
LIBSF_BOOST   = $(BOOST_CPPFLAGS) $(BOOST_UUID_WINCRYPT)

LIBSF         = $(LIBSF_XOREOS) $(LIBSF_GENERAL) $(LIBSF_GRAPHIC) \
                $(LIBSF_SOUND) $(LIBSF_BOOST)

# Library linking flags

LIBSL_XOREOS  = $(XOREOS_LIBS)
LIBSL_GENERAL = $(LTLIBICONV) $(ZLIB_LIBS) $(LZMA_LIBS) $(XML2_LIBS)
LIBSL_GRAPHIC = $(SDL2_LIBS) $(GL_LIBS) $(GLEW_LIBS) \
                $(FT2_LIBS) $(XVID_LIBS) $(VPX_LIBS)
LIBSL_SOUND   = $(AL_LIBS) $(FAAD_LIBS) $(MAD_LIBS) \
                $(OGG_LIBS) $(VORBIS_LIBS)
LIBSL_BOOST   = $(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS) \
                $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_FILESYSTEM_LIBS) \
                $(BOOST_REGEX_LDFLAGS) $(BOOST_REGEX_LIBS) \
                $(BOOST_DATE_TIME_LDFLAGS) $(BOOST_DATE_TIME_LIBS) \
                $(BOOST_ATOMIC_LDFLAGS) $(BOOST_ATOMIC_LIBS) \
                $(BOOST_LOCALE_LDFLAGS) $(BOOST_LOCALE_LIBS) \
                $(BOOST_UUID_LDFLAGS) $(BOOST_UUID_LIBS)

LIBSL         = $(LIBSL_XOREOS) $(LIBSL_GENERAL) $(LIBSL_GRAPHIC) \
                $(LIBSL_SOUND) $(LIBSL_BOOST)

# Other compiler flags

FLAGS_DIR     = -I$(srcdir)
FLAGS_DEBUG   = $(GGDB)
FLAGS_STD     = $(STD)
FLAGS_OPT     = $(LTO) $(NATIVE)
FLAGS_WARN    = $(WARN) $(WERROR)

FLAGS         = $(FLAGS_DIR) $(FLAGS_DEBUG) $(FLAGS_STD) \
                $(FLAGS_OPT) $(FLAGS_WARN)

# Putting it all together...

AM_CXXFLAGS   = $(FLAGS) $(LIBSF)
AM_LDFLAGS    = $(FLAGS_OPT)
LDADD         = $(LIBSL)
LIBS          =

# Now include the top-level recipe file and through it, all the
# other build recipes.

include rules.mk
