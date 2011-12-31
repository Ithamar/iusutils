LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := iusunpack.c

LOCAL_MODULE := iusunpack
LOCAL_MODULE_TAGS := eng

include $(BUILD_HOST_EXECUTABLE)

$(call dist-for-goals,droid,$(LOCAL_BUILT_MODULE))

include $(CLEAR_VARS)

LOCAL_SRC_FILES := iuspack.c
LOCAL_LDLIBS += -lz

LOCAL_MODULE := iuspack
LOCAL_MODULE_TAGS := eng

include $(BUILD_HOST_EXECUTABLE)

$(call dist-for-goals,droid,$(LOCAL_BUILT_MODULE))
