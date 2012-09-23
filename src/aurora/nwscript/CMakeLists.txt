add_library(aurora_nwscript STATIC
  util.cpp
  variable.cpp
  variablecontainer.cpp
  functioncontext.cpp
  object.cpp
  objectcontainer.cpp
  functionman.cpp
  ncsfile.cpp
)
target_link_libraries(aurora_nwscript aurora)
