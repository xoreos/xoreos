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

if(CMAKE_TOOLCHAIN_FILE) # touch toolchain variable to suppress "unused variable" warning
endif()

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Choose an appropriate compiler prefix
set(CMAKE_TOOLCHAIN_PREFIX "x86_64-w64-mingw32")

# which compilers to use for C and C++
find_program(CMAKE_RC_COMPILER  NAMES ${CMAKE_TOOLCHAIN_PREFIX}-windres)
find_program(CMAKE_C_COMPILER   NAMES ${CMAKE_TOOLCHAIN_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${CMAKE_TOOLCHAIN_PREFIX}-g++)
find_program(CMAKE_ASM_COMPILER NAMES ${CMAKE_TOOLCHAIN_PREFIX}-as)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/${CMAKE_TOOLCHAIN_PREFIX} /usr/local/${CMAKE_TOOLCHAIN_PREFIX})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_CROSS_COMPILING TRUE)
