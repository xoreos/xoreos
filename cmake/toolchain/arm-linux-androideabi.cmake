
file(DOWNLOAD "https://raw.github.com/Itseez/opencv/master/platforms/android/android.toolchain.cmake" "${CMAKE_BINARY_DIR}/android-toolchain.cmake")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

set(ANDROID_ABI "armeabi")
set(ANDROID_NATIVE_API_LEVEL "android-9")
set(ANDROID_TOOLCHAIN_NAME "arm-linux-androideabi-4.7")

include(android-toolchain)
