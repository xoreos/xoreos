add_library(graphics STATIC
  graphics.cpp
  fpscounter.cpp
  cursor.cpp
  queueman.cpp
  queueable.cpp
  glcontainer.cpp
  texture.cpp
  font.cpp
  camera.cpp
  renderable.cpp
  object.cpp
  guifrontelement.cpp
  yuv_to_rgb.cpp
  ttf.cpp
)

add_subdirectory(aurora)
add_subdirectory(images)
target_link_libraries(graphics
  common
  graphics_aurora
  graphics_images
)

