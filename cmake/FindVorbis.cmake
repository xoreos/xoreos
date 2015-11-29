# Try to find vorbis library and include path.
# Once done this will define
#
# VORBIS_INCLUDE_DIRS - where to find vorbis/vorbisfile.h, etc.
# VORBIS_LIBRARIES - List of libraries when using libvorbisfile.
# VORBIS_FOUND - True if libvorbisfile found.

if(WIN32)
  find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h $ENV{PROGRAMFILES}/vorbis/include DOC "The directory where vorbis/vorbisfile.h resides")
  find_library(VORBIS_LIBRARY NAMES vorbisfile PATHS $ENV{PROGRAMFILES}/vorbis/lib DOC "The libvorbisfile library")

else(WIN32)
  find_path(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h DOC "The directory where vorbis/vorbisfile.h resides")
  find_library(VORBIS_LIBRARY NAMES vorbisfile DOC "The libvorbisfile library")

endif(WIN32)

if(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY)
  set(VORBIS_FOUND 1)
  set(VORBIS_LIBRARIES ${VORBIS_LIBRARY})
  set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR})
else(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY)
  set(VORBIS_FOUND 0)
  set(VORBIS_LIBRARIES)
  set(VORBIS_INCLUDE_DIRS)
endif(VORBIS_INCLUDE_DIR AND VORBIS_LIBRARY)

mark_as_advanced(VORBIS_INCLUDE_DIR)
mark_as_advanced(VORBIS_LIBRARY)
mark_as_advanced(VORBIS_FOUND)

if(NOT VORBIS_FOUND)
  set(VORBIS_DIR_MESSAGE "libvorbisfile was not found. Make sure VORBIS_LIBRARY and VORBIS_INCLUDE_DIR are set.")
  if(NOT VORBIS_FIND_QUIETLY)
    message(STATUS "${VORBIS_DIR_MESSAGE}")
  else(NOT VORBIS_FIND_QUIETLY)
    if(VORBIS_FIND_REQUIRED)
      message(FATAL_ERROR "${VORBIS_DIR_MESSAGE}")
    endif(VORBIS_FIND_REQUIRED)
  endif(NOT VORBIS_FIND_QUIETLY)
else(NOT VORBIS_FOUND)
  message(STATUS "Found libvorbis: ${VORBIS_LIBRARY}")
endif(NOT VORBIS_FOUND)
