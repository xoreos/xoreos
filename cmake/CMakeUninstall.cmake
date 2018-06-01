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

# Code taken from https://gitlab.kitware.com/cmake/community/wikis/FAQ#can-i-do-make-uninstall-with-cmake.

if(NOT EXISTS "@CMAKE_BINARY_DIR@/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: @CMAKE_BINARY_DIR@/install_manifest.txt")
endif(NOT EXISTS "@CMAKE_BINARY_DIR@/install_manifest.txt")

file(READ "@CMAKE_BINARY_DIR@/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
    message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
    if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
        exec_program(
                "@CMAKE_COMMAND@" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                OUTPUT_VARIABLE rm_out
                RETURN_VALUE rm_retval
        )
        if(NOT "${rm_retval}" STREQUAL 0)
            message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
        endif(NOT "${rm_retval}" STREQUAL 0)
    else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
        message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
    endif(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")

    get_filename_component(file_dir "$ENV{DESTDIR}${file}" DIRECTORY)
    file(GLOB_RECURSE DIR_FILES ${file_dir}/*)
    list(LENGTH DIR_FILES DIR_FILES_LEN)
    if(DIR_FILES_LEN EQUAL 0 AND EXISTS ${file_dir} AND IS_DIRECTORY ${file_dir})
        message(STATUS "Removing Directory \"${file_dir}\"")
        exec_program(
                "/usr/bin/cmake" ARGS "-E remove_directory \"${file_dir}\""
                OUTPUT_VARIABLE rm_out
                RETURN_VALUE rm_retval
        )
        if(NOT "${rm_retval}" STREQUAL 0)
            message(FATAL_ERROR "Problem when removing ${file_dir}")
        endif()
    endif()
endforeach(file)
