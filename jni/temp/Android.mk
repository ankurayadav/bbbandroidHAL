LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_LDLIBS := -llog
LOCAL_MODULE := BBBAndroidHAL
LOCAL_SRC_FILES := jni_wrapper.c gpio.c adc.c pwm.c i2c.c spi.c can.c uart.c
include $(BUILD_SHARED_LIBRARY)