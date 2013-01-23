cmake_minimum_required(VERSION 2.8)

project(xoreos C CXX)
set(xoreos_VERSION 0.0.1)


# -------------------------------------------------------------------------
# load some cmake modules from cmake/ subfolder
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(CheckIncludeFile)
include(CMakeAM)


# -------------------------------------------------------------------------
# platform specific flags
if(CMAKE_HOST_APPLE)
  add_definitions(-DUNIX -DMACOSX)

elseif(CMAKE_HOST_UNIX)
  add_definitions(-DUNIX)

  # Wrap libraries in --start-group and --end-group to easily support static linking and symbol resolution, maybe useful on APPLE also, but I don't know
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_CREATE_SHARED_MODULE "${CMAKE_C_CREATE_SHARED_MODULE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE}")
elseif(CMAKE_HOST_WIN32)

else()
  message(STATUS "Unknown platform, maybe not supported")
endif()

# platform specific warning settings
if(CMAKE_HOST_WIN32)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
else()
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wno-multichar -Wpointer-arith -Wshadow")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-multichar -Wpointer-arith -Wshadow -Wnon-virtual-dtor")
endif()


# header detection, could get rid of it, but it is used by lua as well
include(CheckIncludeFile)
function(check_has_header FILE_NAME PP_NAME)
  check_include_file(${FILE_NAME} ${PP_NAME})
  if(${PP_NAME})
    add_definitions(-D${PP_NAME}=1)
  endif()
endfunction()

check_has_header("stdint.h"    HAVE_STDINT_H)
check_has_header("inttypes.h"  HAVE_INTTYPES_H)
check_has_header("sys/types.h" HAVE_SYS_TYPES_H)


# endianess detection, could be replaced by including Boost.Config 
include(TestBigEndian)
test_big_endian(XOREOS_BIG_ENDIAN)
if(XOREOS_BIG_ENDIAN)
  add_definitions(-DXOREOS_BIG_ENDIAN=1)
else()
  add_definitions(-DXOREOS_LITTLE_ENDIAN=1)
endif()


# -------------------------------------------------------------------------
# subfolders where built binaries and libraries will be located, relative to the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)


# -------------------------------------------------------------------------
# find the required libraries
set(XOREOS_LIBRARIES "")

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${ZLIB_LIBRARIES})

find_package(Boost COMPONENTS filesystem system regex date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${Boost_LIBRARIES})

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
list(APPEND XOREOS_LIBRARIES ${SDL_LIBRARY})

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})
list(APPEND XOREOS_LIBRARIES ${OPENAL_LIBRARY})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
list(APPEND XOREOS_LIBRARIES ${OPENGL_LIBRARIES})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})
list(APPEND XOREOS_LIBRARIES ${FREETYPE_LIBRARIES})

find_package(Mad REQUIRED)
include_directories(${MAD_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${MAD_LIBRARIES})

find_package(Faad REQUIRED)
include_directories(${FAAD_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${FAAD_LIBRARIES})

find_package(Vorbis REQUIRED)
include_directories(${VORBIS_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${VORBIS_LIBRARIES})

find_package(XviD REQUIRED)
include_directories(${XVID_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${XVID_LIBRARIES})


find_package(GLEW)
if(NOT GLEW_FOUND)
  add_definitions(-DGLEW_STATIC)
  set(GLEW_LIBRARIES    glew)
  set(GLEW_INCLUDE_DIRS glew)

  parse_configure(configure.ac glew)
endif()
include_directories(${GLEW_INCLUDE_DIRS})
list(APPEND XOREOS_LIBRARIES ${GLEW_LIBRARIES})


find_package(Lua51)
if(NOT LUA51_FOUND)
  set(LUA_LIBRARIES   lua)
  set(LUA_INCLUDE_DIR lua)

  parse_configure(configure.ac lua)
endif()
include_directories(${LUA_INCLUDE_DIR})
list(APPEND XOREOS_LIBRARIES ${LUA_LIBRARIES})


# -------------------------------------------------------------------------
# xoreos main targets, parsed from configure.ac and */Makefile.am
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
add_definitions(-DPACKAGE_STRING="xoreos ${xoreos_VERSION}")
parse_configure(configure.ac src)
target_link_libraries(xoreos ${XOREOS_LIBRARIES})

# -------------------------------------------------------------------------
# documentation
option(XOREOS_BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(XOREOS_BUILD_DOCUMENTATION)
  find_package(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()

  add_custom_target(doxygen
    COMMAND ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    SOURCES ${PROJECT_SOURCE_DIR}/Doxyfile
  )
  add_custom_target(doxygen-clean
    COMMAND rm -rf doc/doxygen/html/
    COMMAND rm -rf doc/doxygen/latex/
    COMMAND rm -rf doc/doxygen/man/
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()


# -------------------------------------------------------------------------
# packaging
# see: http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
set(CPACK_GENERATOR "TGZ")

set(CPACK_PACKAGE_NAME    ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${xoreos_VERSION})
set(CPACK_PACKAGE_CONTACT "Sven Hesse <drmccoy@drmccoy.de>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xoreos Engine: A new implementation of BioWare's Aurora engine.")
set(CPACK_PACKAGE_DESCRIPTION "Xoreos Engine: A new implementation of BioWare's Aurora engine.")
set(CPACK_PACKAGE_VENDOR "Xoreos Project")

if(CMAKE_HOST_APPLE)
  # MacOSX bundle, unchecked packaging
  # list(APPEND CPACK_GENERATOR "BUNDLE")
  # set(CPACK_BUNDLE_NAME "Xoreos")
  # set(CPACK_BUNDLE_PLIST dists/macosx/Info.plist)

elseif(CMAKE_HOST_UNIX)
  find_program(DPKG_CMD dpkg)
  if(DPKG_CMD)
    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgcc1, libz1, libboost-all-dev, libsdl1.2-dev, libopenal1, libgl1, libfreetype6, libglew1.7, libmad0, libfaad2, libvorbis0a, libxvidcore4")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")

  else()
    message(WARNING "dpkg was not found, DEB packages will NOT be built.")
  endif()

  # For RPM, unchecked packaging, especially the dependency list
  # find_program(RPMBUILD_CMD rpmbuild)
  # if(RPMBUILD_CMD)
  #   list(APPEND CPACK_GENERATOR "RPM")
  #   set(CPACK_RPM_PACKAGE_LICENSE "GPL3")
  #   set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
  #   set(CPACK_RPM_PACKAGE_REQUIRES "libc6, libgcc1, libz1, libboost-all-dev, libsdl1.2, libopenal1, libgl1, libfreetype6, libglew1.7, libmad0, libfaad2, libvorbis0a, libxvidcore4")
  #
  # else()
  #   message(WARNING "rpmbuild was not found, RPM packages will NOT be built.")
  # endif()

elseif(CMAKE_HOST_WIN32)
  # For NSIS installed, unchecked packaging
  # list(APPEND CPACK_GENERATOR "NSIS")

endif()

include(CPack)
