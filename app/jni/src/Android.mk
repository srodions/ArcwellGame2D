LOCAL_PATH := $(call my-dir)

# Обязательно импортируем SDL2 ПЕРЕД объявлением нашего модуля
$(call import-add-path, $(LOCAL_PATH)/..)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Пути теперь корректны относительно папки jni/src
SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/../SDL2_image \
                    $(LOCAL_PATH)/../SDL2_ttf

# Если файл лежит в той же папке, что и этот Android.mk, пишем БЕЗ src/
LOCAL_SRC_FILES := main.c cJSON.c

LOCAL_LDLIBS := -llog -landroid

# Флаг, чтобы функции (SDL_main) были видны снаружи библиотеки
LOCAL_CFLAGS += -fvisibility=default

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf

include $(BUILD_SHARED_LIBRARY)

$(call import-module,SDL2)
$(call import-module,SDL2_image)
$(call import-module,SDL2_ttf)
