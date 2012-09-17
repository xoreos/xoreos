add_library(engines_jade STATIC
  jade.cpp
  gui/main/main.cpp
)
target_link_libraries(engines_jade engines)
