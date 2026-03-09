LOCAL_PATH := $(call my-dir)

# 1. Imports SDL2 BEFORE defining the module
$(call import-add-path, $(LOCAL_PATH)/..)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# 2. The path must be correct relatively to the <jni/src> folder
SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/../SDL2_image \
                    $(LOCAL_PATH)/../SDL2_ttf

LOCAL_SRC_FILES := main.c cJSON.c

LOCAL_LDLIBS := -llog -landroid

# 3. Flag for function (SDL_main) visibility outside the library
LOCAL_CFLAGS += -fvisibility=default

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf

include $(BUILD_SHARED_LIBRARY)

$(call import-module,SDL2)
$(call import-module,SDL2_image)
$(call import-module,SDL2_ttf)
