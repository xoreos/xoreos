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

# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_INCLUDE_DIRS - where to find GL/glew.h, etc.
# GLEW_LIBRARIES - List of libraries when using libGLEW.
# GLEW_FOUND - True if libGLEW found.

if(WIN32)
  find_path(GLEW_INCLUDE_DIR GL/glew.h $ENV{PROGRAMFILES}/GLEW/include DOC "The directory where GL/glew.h resides")
  find_library(GLEW_LIBRARY NAMES glew GLEW glew32 glew32s PATHS $ENV{PROGRAMFILES}/GLEW/lib DOC "The GLEW library")

else(WIN32)
  find_path(GLEW_INCLUDE_DIR GL/glew.h DOC "The directory where GL/glew.h resides")
  find_library(GLEW_LIBRARY NAMES GLEW glew DOC "The GLEW library")

endif(WIN32)

if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
  set(GLEW_FOUND 1)
  set(GLEW_LIBRARIES ${GLEW_LIBRARY})
  set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
else(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
  set(GLEW_FOUND 0)
  set(GLEW_LIBRARIES)
  set(GLEW_INCLUDE_DIRS)
endif(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR)
mark_as_advanced(GLEW_LIBRARY)
mark_as_advanced(GLEW_FOUND)

if(NOT GLEW_FOUND)
  set(GLEW_DIR_MESSAGE "libGLEW was not found. Make sure GLEW_LIBRARY and GLEW_INCLUDE_DIR are set.")
  if(NOT GLEW_FIND_QUIETLY)
    message(STATUS "${GLEW_DIR_MESSAGE}")
  else(NOT GLEW_FIND_QUIETLY)
    if(GLEW_FIND_REQUIRED)
      message(FATAL_ERROR "${GLEW_DIR_MESSAGE}")
    endif(GLEW_FIND_REQUIRED)
  endif(NOT GLEW_FIND_QUIETLY)
else(NOT GLEW_FOUND)
  message(STATUS "Found libGLEW: ${GLEW_LIBRARY}")
endif(NOT GLEW_FOUND)
