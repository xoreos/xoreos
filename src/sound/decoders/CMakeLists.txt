add_library(sound_decoders STATIC
  codec.cpp
  mp3.cpp
  vorbis.cpp
  adpcm.cpp
  wave.cpp
  pcm.cpp
  aac.cpp
  asf.cpp
  wma.cpp
)
target_link_libraries(sound_decoders sound)
