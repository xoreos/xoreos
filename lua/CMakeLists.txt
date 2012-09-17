project(lua C)

include(CheckIncludeFile)

check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(inttypes.h  HAVE_INTTYPES_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)

if(HAVE_STDINT_H)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_STDINT_H")
endif()
if(HAVE_INTTYPES_H)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_INTTYPES_H")
endif()
if(HAVE_SYS_TYPES_H)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_SYS_TYPES_H")
endif()

add_library(lua STATIC
  lapi.c
  lauxlib.c
  lbaselib.c
  lcode.c
  ldblib.c
  ldebug.c
  ldo.c
  ldump.c
  lfunc.c
  lgc.c
  liolib.c
  llex.c
  lmathlib.c
  lmem.c
  loadlib.c
  lobject.c
  lopcodes.c
  lparser.c
  lstate.c
  lstring.c
  lstrlib.c
  ltable.c
  ltablib.c
  ltests.c
  ltm.c
  lundump.c
  lvm.c
  lzio.c
)
