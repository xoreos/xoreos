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

# https://github.com/onyx-intl/cmake_modules/blob/master/FindIconv.cmake
#
# - Try to find Iconv
# Once done this will define
#
# ICONV_FOUND - system has Iconv
# ICONV_INCLUDE_DIR - the Iconv include directory
# ICONV_LIBRARIES - Link these to use Iconv
# ICONV_SECOND_ARGUMENT_IS_CONST - the second argument for iconv() is const
#
include(CheckCXXSourceCompiles)

IF (ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)
  # Already in cache, be silent
  SET(ICONV_FIND_QUIETLY TRUE)
ENDIF (ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)

FIND_PATH(ICONV_INCLUDE_DIR iconv.h)

FIND_LIBRARY(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)

IF(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)
  SET(ICONV_FOUND TRUE)
  SET(ICONV_INCLUDE_DIRS ${ICONV_INCLUDE_DIR})
ENDIF(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)

set(CMAKE_REQUIRED_INCLUDES ${ICONV_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${ICONV_LIBRARIES})
IF(ICONV_FOUND)
  check_cxx_source_compiles("
  #include <iconv.h>
    int main(){
    iconv_t conv = 0;
    const char* in = 0;
    size_t ilen = 0;
    char* out = 0;
    size_t olen = 0;
    iconv(conv, &in, &ilen, &out, &olen);
    return 0;
  }
" ICONV_SECOND_ARGUMENT_IS_CONST )
ENDIF(ICONV_FOUND)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

IF(ICONV_FOUND)
  IF(NOT ICONV_FIND_QUIETLY)
    MESSAGE(STATUS "Found Iconv: ${ICONV_LIBRARIES}")
  ENDIF(NOT ICONV_FIND_QUIETLY)
ELSE(ICONV_FOUND)
  IF(Iconv_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Iconv")
  ENDIF(Iconv_FIND_REQUIRED)
ENDIF(ICONV_FOUND)

MARK_AS_ADVANCED(
  ICONV_INCLUDE_DIR
  ICONV_LIBRARIES
  ICONV_SECOND_ARGUMENT_IS_CONST
)
