bbbandroidHAL
===============================

This contains native library for devoloping java code for BBBAndroid 
--------------------------------

Base native code has been downloaded from [packpub](https://www.packtpub.com/books/content/support) website. This base code was developed by [Andrew Henderson](https://icculus.org/~hendersa/).

You can refer [wiki](https://github.com/ankurayadav/bbbandroidHAL/wiki) for documentation.
Or you can also refer to [doxygen documentation](ankurayadav.github.io/bbbandroidHAL) for more references.

###Simple steps for making Android App using BBBAndroidHAL
1. Create a new Android project in eclipse.
2. Create a jni folder in the project directory.
3. Copy all the files from [bbbandroidHAL](https://github.com/ankurayadav/bbbandroidHAL) to you jni folder.
4. Make proper Android.mk and Application.mk file since the given [Android.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/Android.mk) and [Application.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/Application.mk) files are configured to generate binary code form C program so that we can test our logic before actually making the Android App. For simplicity I have already created [Android.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/temp/Android.mk) and [Application.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/temp/Application.mk) files that can be directly used if you do not want to create one by yourself.
5. Call ndk-build of Android SDK from you jni directory.
6. Now you will be able use JNI interfaces in your Android java programs.
7. You can see [demo applications](https://github.com/ankurayadav/demoapps-bbbandroid) for you reference.

**Note :** If you want to use only few features of BBBAndroidHAL and do not want to compile all the necessary code then you will have to delete those files, and delete there references from Android.mk, bbbandroidHAL.h and jni_wrapper.h