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

# Try to find LibLZMA library and include path.
# Once done this will define
#
# LIBLZMA_INCLUDE_DIRS - where to find lzma/version.h, etc.
# LIBLZMA_LIBRARIES - List of libraries when using liblzma.
# LIBLZMA_FOUND - True if liblzma found.

if(WIN32)
  find_path(LIBLZMA_INCLUDE_DIR lzma/version.h $ENV{PROGRAMFILES}/xz/include DOC "The directory where lzma/version.h resides")
  find_library(LIBLZMA_LIBRARY NAMES lzma liblzma PATHS $ENV{PROGRAMFILES}/xz/lib DOC "The liblzma library")

else(WIN32)
  find_path(LIBLZMA_INCLUDE_DIR lzma/version.h DOC "The directory where lzma/version.h resides")
  find_library(LIBLZMA_LIBRARY NAMES lzma DOC "The liblzma library")

endif(WIN32)

if(LIBLZMA_INCLUDE_DIR AND LIBLZMA_LIBRARY)
  set(LIBLZMA_FOUND 1)
  set(LIBLZMA_LIBRARIES ${LIBLZMA_LIBRARY})
  set(LIBLZMA_INCLUDE_DIRS ${LIBLZMA_INCLUDE_DIR})
else(LIBLZMA_INCLUDE_DIR AND LIBLZMA_LIBRARY)
    set(LIBLZMA_FOUND 0)
    set(LIBLZMA_LIBRARIES)
    set(LIBLZMA_INCLUDE_DIRS)
endif(LIBLZMA_INCLUDE_DIR AND LIBLZMA_LIBRARY)

mark_as_advanced(LIBLZMA_INCLUDE_DIR)
mark_as_advanced(LIBLZMA_LIBRARY)
mark_as_advanced(LIBLZMA_FOUND)

if(NOT LIBLZMA_FOUND)
  set(LIBLZMA_DIR_MESSAGE "liblzma was not found. Make sure LIBLZMA_LIBRARY and LIBLZMA_INCLUDE_DIR are set.")
  if(NOT LIBLZMA_FIND_QUIETLY)
    message(STATUS "${LIBLZMA_DIR_MESSAGE}")
  else(NOT LIBLZMA_FIND_QUIETLY)
    if(LIBLZMA_FIND_REQUIRED)
      message(FATAL_ERROR "${LIBLZMA_DIR_MESSAGE}")
    endif(LIBLZMA_FIND_REQUIRED)
  endif(NOT LIBLZMA_FIND_QUIETLY)
else(NOT LIBLZMA_FOUND)
  message(STATUS "Found liblzma: ${LIBLZMA_LIBRARY}")
endif(NOT LIBLZMA_FOUND)
