add_library(graphics_aurora STATIC
  texture.cpp
  textureman.cpp
  pltfile.cpp
  cursor.cpp
  cursorman.cpp
  texturefont.cpp
  abcfont.cpp
  ttffont.cpp
  fontman.cpp
  text.cpp
  fps.cpp
  cube.cpp
  guiquad.cpp
  modelnode.cpp
  model.cpp
  animnode.cpp
  animation.cpp
  model_nwn.cpp
  model_nwn2.cpp
  model_kotor.cpp
  model_witcher.cpp
)
target_link_libraries(graphics_aurora graphics)
