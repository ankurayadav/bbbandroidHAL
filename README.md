bbbandroidHAL
===============================

This contains native library for devoloping java code for BBBAndroid 
--------------------------------

Base native code has been downloaded from [packpub](https://www.packtpub.com/books/content/support) website. This base code was developed by [Andrew Henderson](https://icculus.org/~hendersa/).

You can refer [wiki](https://github.com/ankurayadav/bbbandroidHAL/wiki) for documentation.
Or you can also refer to [doxygen documentation](http://ankurayadav.github.io/bbbandroidHAL) for more references.

###Simple steps for making Android App using BBBAndroidHAL
1. Create a new Android project in eclipse.
2. Create a jni folder in the project directory.
3. Copy all the files from [bbbandroidHAL](https://github.com/ankurayadav/bbbandroidHAL) to you jni folder.
4. Make proper Android.mk and Application.mk file since the given [Android.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/Android.mk) and [Application.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/Application.mk) files are configured to generate binary code form C program so that we can test our logic before actually making the Android App. For simplicity I have already created [Android.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/temp/Android.mk) and [Application.mk](https://github.com/ankurayadav/bbbandroidHAL/blob/master/jni/temp/Application.mk) files that can be directly used if you do not want to create one by yourself.
5. Call ndk-build of Android SDK from you jni directory.
6. Now you will be able use JNI interfaces in your Android java programs.
7. You can see [demo applications](https://github.com/ankurayadav/demoapps-bbbandroid) for you reference.

**Note :** If you want to use only few features of BBBAndroidHAL and do not want to compile all the necessary code then you will have to delete those files, and delete there references from Android.mk, bbbandroidHAL.h and jni_wrapper.h

###Some useful commands.
1. **Command to login into bbbandroid shell** :
<pre><code>
adb shell
</pre></code>
2. **Command to send file into bbbandroid** :
<pre><code>
adb push file /path_in_bbbandroid
</pre></code>
3. **Compiling Device Tree Overlay** :
<pre><code>
dtc -O dtb -o DEVICE-TREE-00A0.dtbo -b 0 -@ DEVICE-TREE-00A0.dts
</pre></code>
3. **Installing Device Tree Overlay** :
<pre><code>
adb push DEVICE-TREE-00A0.dtbo /system/vendor/firmware/
echo DEVICE-TREE > /sys/devices/bone_capemgr.9/slots 
</pre></code>
After installing device tree overlay we can export pins and provide proper permissions.
4. **Exporting GPIO** :
<pre><code>
cd /sys/class/gpio/
echo 22 > export
cd gpio22
chmod 0777 value
</pre></code>
5. **Exporting PWM** :
<pre><code>
cd /sys/class/pwm/
echo 0 > export
cd pwm0
chmod 0777 duty_ns period_ns run polarity
</pre></code>
6. **Accessing ADC** :
<pre><code>
cd /sys/bus/iio/devices/iio:device0
cat in_voltage5_raw
chmod 0777 in_voltage5_raw
</pre></code>
7. **Accessing I2C** :
<pre><code>
chmod 0777 /dev/i2c-1
</pre></code>
8. **Accessing SPI** :
<pre><code>
chmod 0777 /dev/spidev1.0
</pre></code>
9. **Accessing UART** :
<pre><code>
chmod 0777 /dev/ttyO4
</pre></code>
10. **Accessing CAN** :
At first we have to install missing kernel modules for CAN.
<pre><code>
cd /system/vendor/lib/modules/3.8.13+/kernel/drivers/net/can/
insmod can-dev.ko
insmod c_can/c_can.ko 
insmod c_can/c_can_platform.ko
insmod vcan.ko
cd /system/vendor/lib/modules/3.8.13+/kernel/net/can/
insmod can.ko
insmod can-raw.ko
insmod can-bcm.ko
</pre></code>
Then we have to add vcan0 for virtual can.
<pre><code>
ip link add dev vcan0 type vcan
ip link set up vcan0
ip link show vcan0
</pre></code>
10. **Accessing USB*** :
<pre><code>
cd /dev/bus/usb/
chmod -R 0777 .
</pre></code>