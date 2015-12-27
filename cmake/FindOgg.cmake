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

# Try to find Ogg library and include path.
# Once done this will define
#
# OGG_INCLUDE_DIRS - where to find ogg/ogg.h, etc.
# OGG_LIBRARIES - List of libraries when using libogg.
# OGG_FOUND - True if libogg found.

if(WIN32)
  find_path(OGG_INCLUDE_DIR ogg/ogg.h $ENV{PROGRAMFILES}/libogg/include DOC "The directory where ogg/ogg.h resides")
  find_library(OGG_LIBRARY NAMES ogg libogg PATHS $ENV{PROGRAMFILES}/libogg/lib DOC "The libogg library")

else(WIN32)
  find_path(OGG_INCLUDE_DIR ogg/ogg.h DOC "The directory where ogg/ogg.h resides")
  find_library(OGG_LIBRARY NAMES ogg DOC "The libogg library")

endif(WIN32)

if(OGG_INCLUDE_DIR AND OGG_LIBRARY)
  set(OGG_FOUND 1)
  set(OGG_LIBRARIES ${OGG_LIBRARY})
  set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR})
else(OGG_INCLUDE_DIR AND OGG_LIBRARY)
  set(OGG_FOUND 0)
  set(OGG_LIBRARIES)
  set(OGG_INCLUDE_DIRS)
endif(OGG_INCLUDE_DIR AND OGG_LIBRARY)

mark_as_advanced(OGG_INCLUDE_DIR)
mark_as_advanced(OGG_LIBRARY)
mark_as_advanced(OGG_FOUND)

if(NOT OGG_FOUND)
  set(OGG_DIR_MESSAGE "libogg was not found. Make sure OGG_LIBRARY and OGG_INCLUDE_DIR are set.")
  if(NOT OGG_FIND_QUIETLY)
    message(STATUS "${OGG_DIR_MESSAGE}")
  else(NOT OGG_FIND_QUIETLY)
    if(OGG_FIND_REQUIRED)
      message(FATAL_ERROR "${OGG_DIR_MESSAGE}")
    endif(OGG_FIND_REQUIRED)
  endif(NOT OGG_FIND_QUIETLY)
else(NOT OGG_FOUND)
  message(STATUS "Found libogg: ${OGG_LIBRARY}")
endif(NOT OGG_FOUND)
