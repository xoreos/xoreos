add_library(engines_nwn2 STATIC
  nwn2.cpp
  modelloader.cpp
)
target_link_libraries(engines_nwn2 engines)
