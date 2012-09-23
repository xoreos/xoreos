add_library(graphics_images STATIC
  decoder.cpp
  dumptga.cpp
  screenshot.cpp
  surface.cpp
  tga.cpp
  dds.cpp
  tpc.cpp
  txb.cpp
  txitypes.cpp
  txi.cpp
  s3tc.cpp
  sbm.cpp
  winiconimage.cpp
)
target_link_libraries(graphics_images graphics)
