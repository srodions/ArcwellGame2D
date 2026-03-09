@echo off
"C:\\AndroidSDK\\ndk\\android-ndk-r27d\\ndk-build.cmd" ^
  "NDK_PROJECT_PATH=null" ^
  "APP_BUILD_SCRIPT=C:\\lib\\SDL2-2.32.10\\android-project\\app\\jni\\Android.mk" ^
  "NDK_APPLICATION_MK=C:\\lib\\SDL2-2.32.10\\android-project\\app\\jni\\Application.mk" ^
  "APP_ABI=x86" ^
  "NDK_ALL_ABIS=x86" ^
  "NDK_DEBUG=1" ^
  "NDK_OUT=C:\\lib\\SDL2-2.32.10\\android-project\\app\\build\\intermediates\\cxx\\Debug\\e4h3v4u1/obj" ^
  "NDK_LIBS_OUT=C:\\lib\\SDL2-2.32.10\\android-project\\app\\build\\intermediates\\cxx\\Debug\\e4h3v4u1/lib" ^
  "APP_PLATFORM=android-19" ^
  "APP_SHORT_COMMANDS=false" ^
  "LOCAL_SHORT_COMMANDS=false" ^
  -B ^
  -n
