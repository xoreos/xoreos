cmake_minimum_required(VERSION 2.8)

project(xoreos C CXX)
set(xoreos_VERSION 0.0.1)

# Subfolders where built binaries and libraries will be located, relative to the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(CheckIncludeFile)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-multichar -Wpointer-arith -Wshadow -Wnon-virtual-dtor")
endif()

find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS filesystem system regex date_time REQUIRED)
find_package(SDL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(Mad REQUIRED)
find_package(Faad REQUIRED)
find_package(Vorbis REQUIRED)
find_package(XviD REQUIRED)

find_package(GLEW)
if(NOT GLEW_FOUND)
  add_definitions(-DGLEW_STATIC)
  add_subdirectory(glew)
  set(GLEW_LIBRARIES    glew)
  set(GLEW_INCLUDE_DIRS glew)
endif()

find_package(Lua51)
if(NOT LUA51_FOUND)
  add_subdirectory(lua)
  set(LUA_LIBRARIES   lua)
  set(LUA_INCLUDE_DIR lua)
endif()

include_directories(
  ${ZLIB_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${SDL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIRS}
  ${MAD_INCLUDE_DIRS}
  ${VORBIS_INCLUDE_DIRS}
  ${FAAD_INCLUDE_DIRS}
  ${XVID_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIR}
  src ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}
)
link_directories(
  ${FREETYPE_LIBRARY_DIRS}
)

# -------------------------------------------------------------------------
# xoreos
add_subdirectory(src)

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

if(APPLE)
  # MacOSX bundle, unchecked packaging
  # list(APPEND CPACK_GENERATOR "BUNDLE")
  # set(CPACK_BUNDLE_NAME "Xoreos")
  # set(CPACK_BUNDLE_PLIST dists/macosx/Info.plist)

elseif(UNIX)
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

elseif(WIN32)
  # For NSIS installed, unchecked packaging
  # list(APPEND CPACK_GENERATOR "NSIS")

endif()

include(CPack)
