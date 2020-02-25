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

# Try to find Cairo library and include path.
# Once done this will define
#
# CAIRO_INCLUDE_DIRS - where to find faad.h, etc.
# CAIRO_LIBRARIES - List of libraries when using libfaad.
# CAIRO_FOUND - True if libfaad found.

if(WIN32)
    find_path(CAIRO_INCLUDE_DIR cairo/cairo.h $ENV{PROGRAMFILES}/CAIRO/include DOC "The directory where cairo.h resides")
    find_library(CAIRO_LIBRARY NAMES cairo PATHS $ENV{PROGRAMFILES}/CAIRO/lib DOC "The Cairo library")

else(WIN32)
    find_path(CAIRO_INCLUDE_DIR cairo/cairo.h DOC "The directory where cairo.h resides")
    find_library(CAIRO_LIBRARY NAMES cairo DOC "The Cairo library")

endif(WIN32)

if(CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)
    set(CAIRO_FOUND 1)
    set(CAIRO_LIBRARIES ${CAIRO_LIBRARY})
    set(CAIRO_INCLUDE_DIRS ${CAIRO_INCLUDE_DIR})
else(CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)
    set(CAIRO_FOUND 0)
    set(CAIRO_LIBRARIES)
    set(CAIRO_INCLUDE_DIRS)
endif(CAIRO_INCLUDE_DIR AND CAIRO_LIBRARY)

if(NOT CAIRO_FOUND)
    set(CAIRO_DIR_MESSAGE "libcairo was not found. Make sure CAIRO_LIBRARY and CAIRO_INCLUDE_DIR are set.")
    if(NOT CAIRO_FIND_QUIETLY)
        message(STATUS "${CAIRO_DIR_MESSAGE}")
    else(NOT CAIRO_FIND_QUIETLY)
        if(CAIRO_FIND_REQUIRED)
            message(FATAL_ERROR "${CAIRO_DIR_MESSAGE}")
        endif(CAIRO_FIND_REQUIRED)
    endif(NOT CAIRO_FIND_QUIETLY)
else(NOT CAIRO_FOUND)
    message(STATUS "Found libcairo: ${CAIRO_LIBRARY}")
endif(NOT CAIRO_FOUND)
