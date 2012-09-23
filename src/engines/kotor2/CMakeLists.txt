add_library(engines_kotor2 STATIC
  kotor2.cpp
  modelloader.cpp
  module.cpp
  area.cpp
  gui/main/main.cpp
)
target_link_libraries(engines_kotor2 engines)
