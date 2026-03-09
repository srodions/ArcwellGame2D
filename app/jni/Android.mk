LOCAL_PATH := $(call my-dir)

# 1. Добавляем текущую директорию (jni) в пути поиска модулей
$(call import-add-path, $(LOCAL_PATH))

# 2. Включаем твой проект из папки src
include $(LOCAL_PATH)/src/Android.mk

# 3. Импортируем библиотеки (имена папок в jni должны совпадать с тегами)
$(call import-module,SDL2)
$(call import-module,SDL2_image)
$(call import-module,SDL2_ttf)
