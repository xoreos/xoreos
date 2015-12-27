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

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

function(set_check_compiler_flag_c FLAG)
  CHECK_C_COMPILER_FLAG(${FLAG} hasC${FLAG})
  if(hasC${FLAG})
    if(ARGN)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARGN}" PARENT_SCOPE)
    else()
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(set_check_compiler_flag_cxx FLAG)
  CHECK_CXX_COMPILER_FLAG(${FLAG} hasCXX${FLAG})
  if(hasCXX${FLAG})
    if(ARGN)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARGN}" PARENT_SCOPE)
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}" PARENT_SCOPE)
    endif()
  endif()
endfunction()

macro(set_check_compiler_flag_c_cxx FLAG)
  set_check_compiler_flag_c(${FLAG} ${ARGN})
  set_check_compiler_flag_cxx(${FLAG} ${ARGN})
endmacro()
