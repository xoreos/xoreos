add_library(video_codecs STATIC
  codec.cpp
  h263.cpp
  wmv2data.cpp
  xmvwmv2.cpp
)
target_link_libraries(video_codecs video)
