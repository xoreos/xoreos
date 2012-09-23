add_library(sound STATIC
  sound.cpp
  audiostream.cpp
  interleaver.cpp
)

add_subdirectory(decoders)
target_link_libraries(sound
  common
  sound_decoders
)
