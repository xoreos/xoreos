set(PACKAGE_STRING "\"xoreos ${xoreos_VERSION}\"")
configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)
add_definitions(-DHAVE_CONFIG_H)

add_executable(xoreos
  cline.cpp
  xoreos.cpp
)

add_subdirectory(common)
add_subdirectory(aurora)
add_subdirectory(engines)
add_subdirectory(events)
add_subdirectory(graphics)
add_subdirectory(sound)
add_subdirectory(video)
target_link_libraries(xoreos
  aurora common engines events graphics sound video
  ${ZLIB_LIBRARIES}
  ${Boost_LIBRARIES}
  ${SDL_LIBRARY}
  ${OPENAL_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${MAD_LIBRARIES}
  ${VORBIS_LIBRARIES}
  ${FAAD_LIBRARIES}
  ${XVID_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${LUA_LIBRARIES}
)

# -------------------------------------------------------------------------
# installation
install(TARGETS xoreos
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
