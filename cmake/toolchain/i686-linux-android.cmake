
file(DOWNLOAD "https://raw.github.com/Itseez/opencv/master/platforms/android/android.toolchain.cmake" "${CMAKE_BINARY_DIR}/android-toolchain.cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

set(ANDROID_ABI "x86")
set(ANDROID_NATIVE_API_LEVEL "android-9")
set(ANDROID_TOOLCHAIN_NAME "x86-4.7")

include(android-toolchain)
