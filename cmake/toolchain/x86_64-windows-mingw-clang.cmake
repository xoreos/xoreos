# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Choose an appropriate compiler prefix
set(CMAKE_TOOLCHAIN_PREFIX "x86_64-w64-mingw32")


# which compilers to use for C and C++
find_program(CMAKE_RC_COMPILER  NAMES ${CMAKE_TOOLCHAIN_PREFIX}-windres)
find_program(CMAKE_C_COMPILER   NAMES ${CMAKE_TOOLCHAIN_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${CMAKE_TOOLCHAIN_PREFIX}-g++)
find_program(CMAKE_ASM_COMPILER NAMES ${CMAKE_TOOLCHAIN_PREFIX}-as)

set(CMAKE_C_COMPILER   "clang"   "-target ${CMAKE_TOOLCHAIN_PREFIX} -isystem /usr/${CMAKE_TOOLCHAIN_PREFIX}/include -isystem /usr/local/${CMAKE_TOOLCHAIN_PREFIX}/include")
set(CMAKE_CXX_COMPILER "clang++" "-target ${CMAKE_TOOLCHAIN_PREFIX} -isystem /usr/${CMAKE_TOOLCHAIN_PREFIX}/include -isystem /usr/local/${CMAKE_TOOLCHAIN_PREFIX}/include")


# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/${CMAKE_TOOLCHAIN_PREFIX} /usr/local/${CMAKE_TOOLCHAIN_PREFIX})


# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_CROSS_COMPILING TRUE)
