# Try to find FAAD library and include path.
# Once done this will define
#
# FAAD_INCLUDE_DIRS - where to find faad.h, etc.
# FAAD_LIBRARIES - List of libraries when using libfaad.
# FAAD_FOUND - True if libfaad found.

if(WIN32)
  find_path(FAAD_INCLUDE_DIR faad.h $ENV{PROGRAMFILES}/FAAD/include DOC "The directory where faad.h resides")
  find_library(FAAD_LIBRARY NAMES faad PATHS $ENV{PROGRAMFILES}/FAAD/lib DOC "The libfaad library")

else(WIN32)
  find_path(FAAD_INCLUDE_DIR faad.h DOC "The directory where faad.h resides")
  find_library(FAAD_LIBRARY NAMES faad DOC "The libfaad library")

endif(WIN32)

if(FAAD_INCLUDE_DIR AND FAAD_LIBRARY)
  set(FAAD_FOUND 1)
  set(FAAD_LIBRARIES ${FAAD_LIBRARY})
  set(FAAD_INCLUDE_DIRS ${FAAD_INCLUDE_DIR})
else(FAAD_INCLUDE_DIR AND FAAD_LIBRARY)
  set(FAAD_FOUND 0)
  set(FAAD_LIBRARIES)
  set(FAAD_INCLUDE_DIRS)
endif(FAAD_INCLUDE_DIR AND FAAD_LIBRARY)

mark_as_advanced(FAAD_INCLUDE_DIR)
mark_as_advanced(FAAD_LIBRARY)
mark_as_advanced(FAAD_FOUND)

if(NOT FAAD_FOUND)
  set(FAAD_DIR_MESSAGE "libfaad was not found. Make sure FAAD_LIBRARY and FAAD_INCLUDE_DIR are set.")
  if(NOT FAAD_FIND_QUIETLY)
    message(STATUS "${FAAD_DIR_MESSAGE}")
  else(NOT FAAD_FIND_QUIETLY)
    if(FAAD_FIND_REQUIRED)
      message(FATAL_ERROR "${FAAD_DIR_MESSAGE}")
    endif(FAAD_FIND_REQUIRED)
  endif(NOT FAAD_FIND_QUIETLY)
endif(NOT FAAD_FOUND)
