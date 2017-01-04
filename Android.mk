LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := wikiParser
LOCAL_SRC_FILES := wikiParser.c

include $(BUILD_SHARED_LIBRARY)