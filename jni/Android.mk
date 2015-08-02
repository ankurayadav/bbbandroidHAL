ifneq ($(TARGET_SIMULATOR),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -Wall
LOCAL_LDLIBS := -L$(LOCAL_PATH)/lib -llog -g
LOCAL_C_INCLUDES := bionic
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LIBUSB_ROOT_ABS)
LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_SRC_FILES:= gpio.c adc.c pwm.c i2c.c spi.c can.c uart.c usb.c main.c
LOCAL_MODULE := testHal
include $(BUILD_EXECUTABLE)
endif  # TARGET_SIMULATOR != true

include include/libusb/android/jni/libusb.mk