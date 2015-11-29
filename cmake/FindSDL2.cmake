# Try to find SDL2 library and include path.
# Once done this will define
#
# SDL2_INCLUDE_DIRS - where to find SDL.h, etc.
# SDL2_LIBRARIES - List of libraries when using SDL2.
# SDL2_FOUND - True if SDL2 found.

if(WIN32)
  find_path(SDL2_INCLUDE_DIR SDL.h $ENV{PROGRAMFILES}/SDL2/include DOC "The directory where SDL.h resides")
  find_library(SDL2_LIBRARY NAMES SDL2 PATHS $ENV{PROGRAMFILES}/SDL2/lib DOC "The SDL2 library")

else(WIN32)
  find_path(SDL2_INCLUDE_DIR SDL.h /usr/include/SDL2 DOC "The directory where SDL.h resides")
  find_library(SDL2_LIBRARY NAMES SDL2 DOC "The SDL2 library")

endif(WIN32)

if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
  set(SDL2_FOUND 1)
  set(SDL2_LIBRARIES ${SDL2_LIBRARY})
  set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
else(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
  set(SDL2_FOUND 0)
  set(SDL2_LIBRARIES)
  set(SDL2_INCLUDE_DIRS)
endif(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)

mark_as_advanced(SDL2_INCLUDE_DIR)
mark_as_advanced(SDL2_LIBRARY)
mark_as_advanced(SDL2_FOUND)

if(NOT SDL2_FOUND)
  set(SDL2_DIR_MESSAGE "SDL2 was not found. Make sure SDL2_LIBRARY and SDL2_INCLUDE_DIR are set.")
  if(NOT SDL2_FIND_QUIETLY)
    message(STATUS "${SDL2_DIR_MESSAGE}")
  else(NOT SDL2_FIND_QUIETLY)
    if(SDL2_FIND_REQUIRED)
      message(FATAL_ERROR "${SDL2_DIR_MESSAGE}")
    endif(SDL2_FIND_REQUIRED)
  endif(NOT SDL2_FIND_QUIETLY)
else(NOT SDL2_FOUND)
  message(STATUS "Found libSDL2: ${SDL2_LIBRARY}")
endif(NOT SDL2_FOUND)
