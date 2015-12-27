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

# Try to find XviD library and include path.
# Once done this will define
#
# XVID_INCLUDE_DIRS - where to find xvid.h, etc.
# XVID_LIBRARIES - List of libraries when using libxvidcore.
# XVID_FOUND - True if libxvidcore found.

if(WIN32)
  find_path(XVID_INCLUDE_DIR xvid.h $ENV{PROGRAMFILES}/XviD/include DOC "The directory where xvid.h resides")
  find_library(XVID_LIBRARY NAMES xvidcore PATHS $ENV{PROGRAMFILES}/XviD/lib DOC "The libxvidcore library")

else(WIN32)
  find_path(XVID_INCLUDE_DIR xvid.h DOC "The directory where xvid.h resides")
  find_library(XVID_LIBRARY NAMES xvidcore DOC "The libxvidcore library")

endif(WIN32)

if(XVID_INCLUDE_DIR AND XVID_LIBRARY)
  set(XVID_FOUND 1)
  set(XVID_LIBRARIES ${XVID_LIBRARY})
  set(XVID_INCLUDE_DIRS ${XVID_INCLUDE_DIR})
else(XVID_INCLUDE_DIR AND XVID_LIBRARY)
  set(XVID_FOUND 0)
  set(XVID_LIBRARIES)
  set(XVID_INCLUDE_DIRS)
endif(XVID_INCLUDE_DIR AND XVID_LIBRARY)

mark_as_advanced(XVID_INCLUDE_DIR)
mark_as_advanced(XVID_LIBRARY)
mark_as_advanced(XVID_FOUND)

if(NOT XVID_FOUND)
  set(XVID_DIR_MESSAGE "libxvidcore was not found. Make sure XVID_LIBRARY and XVID_INCLUDE_DIR are set.")
  if(NOT XVID_FIND_QUIETLY)
    message(STATUS "${XVID_DIR_MESSAGE}")
  else(NOT XVID_FIND_QUIETLY)
    if(XVID_FIND_REQUIRED)
      message(FATAL_ERROR "${XVID_DIR_MESSAGE}")
    endif(XVID_FIND_REQUIRED)
  endif(NOT XVID_FIND_QUIETLY)
else(NOT XVID_FOUND)
  message(STATUS "Found libxvid: ${XVID_LIBRARY}")
endif(NOT XVID_FOUND)
