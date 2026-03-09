LOCAL_PATH := $(call my-dir)

# 1. Imports current directory (jni) in module search path
$(call import-add-path, $(LOCAL_PATH))

# 2. Includes project from folder <src>
include $(LOCAL_PATH)/src/Android.mk

# 3. Imports libraries (folder's names in jni must be the same as the tags)
$(call import-module,SDL2)
$(call import-module,SDL2_image)
$(call import-module,SDL2_ttf)
