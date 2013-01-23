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
endif(NOT XVID_FOUND)
