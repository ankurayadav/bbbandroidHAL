LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_LDLIBS := -llog
LOCAL_MODULE := BBBAndroidHAL
LOCAL_C_INCLUDES += $(LIBUSB_ROOT_ABS)
LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_SRC_FILES := jni_wrapper.c gpio.c adc.c pwm.c i2c.c spi.c can.c uart.c usb.c
include $(BUILD_SHARED_LIBRARY)

include include/libusb/android/jni/libusb.mk