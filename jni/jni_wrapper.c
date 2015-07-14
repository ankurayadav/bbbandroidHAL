#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>

#include "bbbandroidHAL.h"

#define JAVA_CLASS_PATH(funtion_name) Java_com_packt_gpio_MainActivity_##funtion_name

#define BBBANDROID_NATIVE_TAG "NDK_BBBAndroidApplication"
#define BUFFER_SIZE 64

/* Begin the JNI wrapper functions for the GPIO app */
jboolean JAVA_CLASS_PATH(openGPIO)(JNIEnv *env, jobject this)
{
	jboolean ret = JNI_TRUE;
	if ( openGPIO(0) == 0 ) {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "GPIO Opened.");
	} else {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "openGPIO() failed!");
		ret = JNI_FALSE;
	}
	return ret;
}

void JAVA_CLASS_PATH(closeGPIO)(JNIEnv *env, jobject this)
{
	__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "GPIO closing...");
	closeGPIO();
}

jboolean JAVA_CLASS_PATH(readGPIO)(JNIEnv *env, jobject this, jint header, jint pin)
{
	int ret = readGPIO((unsigned int) header, (unsigned int) pin);
	__android_log_print(ANDROID_LOG_DEBUG,
			BBBANDROID_NATIVE_TAG,
			"readGPIO(%d, %d) == %d", (unsigned int) header, (unsigned int) pin, ret);
	return ret == 0 ? JNI_FALSE : JNI_TRUE;
}

void JAVA_CLASS_PATH(writeGPIO)(JNIEnv *env, jobject this, jint header, jint pin, jint val)
{
	int ret = writeGPIO((unsigned int) header, (unsigned int) pin, (unsigned int) val);
	__android_log_print(ANDROID_LOG_DEBUG,
			BBBANDROID_NATIVE_TAG,
			"writeGPIO(%d, %d, %d) == %d", (unsigned int) header, (unsigned int) pin, (unsigned int) val, ret);
}
/* End the JNI wrapper functions for the GPIO app */

/* Begin the JNI wrapper functions for the ADC app */

jint JAVA_CLASS_PATH(readADC)(JNIEnv *env, jobject this, jint channel)
{
	jint ret;
	ret = readADC(channel) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "readADC(%d) failed!", (unsigned int) channel);
		ret = -1;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "readADC(%d) succeeded", (unsigned int) channel);
	}

	return ret;
}

/* End the JNI wrapper funtions for the ADC app */

/* Begin the JNI wrapper functions for the I2C app */

jint JAVA_CLASS_PATH(i2cOpenAdaptor)(JNIEnv *env, jobject this, jint adaptorNumber)
{
	jint ret;
	ret = i2cOpenAdaptor(adaptorNumber) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cOpenAdaptor(%d) failed!", (unsigned int) adaptorNumber);
		ret = -1;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cOpenAdaptor(%d) succeeded", (unsigned int) adaptorNumber);
	}

	return ret;
}

jboolean JAVA_CLASS_PATH(i2cSetSlave)(JNIEnv *env, jobject this, jint i2cFD, jint address)
{
	jint ret;
	ret = i2cSetSlave(i2cFD, address) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cSetSlave(%d, %d) failed!", (unsigned int) i2cFD, (unsigned int) address);
		return JNI_FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cSetSlave(%d, %d) succeeded", (unsigned int) i2cFD, (unsigned int) address);
	}

	return JNI_TRUE;
}

jboolean JAVA_CLASS_PATH(i2cWriteByte)(JNIEnv *env, jobject this, jint i2cFD, jbyte add, jbyte byte)
{
	jint ret;
	ret = i2cWriteByte(i2cFD, add, byte) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cWriteByte(%d, %d) failed!", (unsigned int) i2cFD, (unsigned int) byte);
		return JNI_FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cWriteByte(%d, %d) succeeded", (unsigned int) i2cFD, (unsigned int) byte);
	}

	return JNI_TRUE;
}

jboolean JAVA_CLASS_PATH(i2cWriteBytes)(JNIEnv *env, jobject this, jint i2cFD,jbyte add, jint length, jbyteArray barray)
{
	jint ret;
	int i;

	jbyte* bufferPtr = (*env)->GetByteArrayElements(env, barray, NULL);

	uint8_t bytes[length] ;

	for(i=0; i<length; i++)
	{
		bytes[i] = bufferPtr[i];
	}

	(*env)->ReleaseByteArrayElements(env, barray, bufferPtr, 0);

	ret = i2cWriteBytes(i2cFD, add, length, bytes) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cWriteBytes(%d, %d, bytearray) failed!", (unsigned int) i2cFD, (unsigned int) length);
		return JNI_FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cWriteBytes(%d, %d, bytearray) succeeded", (unsigned int) i2cFD, (unsigned int) length);
	}

	return JNI_TRUE;
}

jint JAVA_CLASS_PATH(i2cReadByte)(JNIEnv *env, jobject this, jint i2cFD, jbyte add)
{
	jint ret;
	ret = i2cReadByte(i2cFD, add) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cReadByte(%d) failed!", (unsigned int) i2cFD);
		return -1;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cReadByte(%d) succeeded", (unsigned int) i2cFD);
	}

	return ret;
}

jboolean JAVA_CLASS_PATH(i2cReadBytes)(JNIEnv *env, jobject this, jint i2cFD, jbyte add, jint length, jintArray barray)
{
	jint ret;
	int i;
	uint8_t bytes[length] ;

	ret = i2cReadBytes(i2cFD, add, length, bytes) ;

	if ( ret == -1 ) {
		__android_log_print(ANDROID_LOG_ERROR, BBBANDROID_NATIVE_TAG, "i2cReadBytes(%d, %d, bytearray) failed!", (unsigned int) i2cFD, (unsigned int) length);
		return JNI_FALSE;
	} else {
		__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cReadBytes(%d, %d, bytearray) succeeded", (unsigned int) i2cFD, (unsigned int) length);
	}

	jint* bufferPtr = (*env)->GetIntArrayElements(env, barray, NULL);

	for(i=0; i<length; i++)
	{
		bufferPtr[i] = bytes[i];
	}

	(*env)->ReleaseIntArrayElements(env, barray, bufferPtr, 0);

	return JNI_TRUE;
}

void JAVA_CLASS_PATH(i2cClose)(JNIEnv *env, jobject this, jint i2cFD)
{
	i2cClose(i2cFD) ;

	__android_log_print(ANDROID_LOG_DEBUG, BBBANDROID_NATIVE_TAG, "i2cClose(%d, bytearray) succeeded", (unsigned int) i2cFD);

}

/* End the JNI wrapper funtions for the I2C app */

/* End the JNI wrapper functions for the Complete app */