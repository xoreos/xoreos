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

# Try to find vpx library and include path.
# Once done this will define
#
# VPX_INCLUDE_DIRS - where to find vorbis/vorbisfile.h, etc.
# VPX_LIBRARIES - List of libraries when using libvorbisfile.
# VPX_FOUND - True if libvorbisfile found.

if(WIN32)
    find_path(VPX_INCLUDE_DIR vpx/vpx_decoder.h $ENV{PROGRAMFILES}/vpx/include DOC "The directory where vpx/vpx_decoder.h resides")
    find_library(VPX_LIBRARY NAMES vpx PATHS $ENV{PROGRAMFILES}/vpx/lib DOC "The libvpx library")

else(WIN32)
    find_path(VPX_INCLUDE_DIR vpx/vpx_decoder.h DOC "The directory where vpx/vpx_decoder.h resides")
    find_library(VPX_LIBRARY NAMES vpx DOC "The libvpx library")

endif(WIN32)

if(VPX_INCLUDE_DIR AND VPX_LIBRARY)
    set(VPX_FOUND 1)
    set(VPX_LIBRARIES ${VPX_LIBRARY} ${VPXFILE_LIBRARY})
    set(VPX_INCLUDE_DIRS ${VPX_INCLUDE_DIR})
else(VPX_INCLUDE_DIR AND VPX_LIBRARY AND VPXFILE_LIBRARY)
    set(VPX_FOUND 0)
    set(VPX_LIBRARIES)
    set(VPX_INCLUDE_DIRS)
endif(VPX_INCLUDE_DIR AND VPX_LIBRARY)

mark_as_advanced(VPX_INCLUDE_DIR)
mark_as_advanced(VPX_LIBRARY)
mark_as_advanced(VPX_FOUND)

if(NOT VPX_FOUND)
    set(VPX_DIR_MESSAGE "libvpx was not found. Make sure VPX_LIBRARY and VPX_INCLUDE_DIR are set.")
    if(NOT VPX_FIND_QUIETLY)
        message(STATUS "${VPX_DIR_MESSAGE}")
    else(NOT VPX_FIND_QUIETLY)
        if(VPX_FIND_REQUIRED)
            message(FATAL_ERROR "${VPX_DIR_MESSAGE}")
        endif(VPX_FIND_REQUIRED)
    endif(NOT VPX_FIND_QUIETLY)
else(NOT VPX_FOUND)
    message(STATUS "Found libvpx: ${VPX_LIBRARY}")
endif(NOT VPX_FOUND)
