add_library(video STATIC
  decoder.cpp
  bink.cpp
  fader.cpp
  quicktime.cpp
  xmv.cpp
  actimagine.cpp
)

add_subdirectory(aurora)
add_subdirectory(codecs)
target_link_libraries(video
  common
  graphics
  sound
  video_aurora
  video_codecs
)
