/**********************************************************
  USB sample interface code using libusb

  Written by Ankur Yadav (ankurayadav@gmail.com)

  This code is made available under the BSD license.
**********************************************************/

/**
 * @file usb.c
 * @author Ankur Yadav (ankurayadav@gmail.com)
 * @brief USB sample interface code using libusb
 */

#include "include/libusb/libusb/libusb.h"
#include "bbbandroidHAL.h"
#include <string.h>

static libusb_device **devs;        /**< pointer to pinter of device to get list of devices */
static int ret;                     /**< To store return values for some functions internally */
static libusb_context *ctx = NULL;  /**< To store libusb session */
static ssize_t count;               /**< To store number of devices in the list */

/**
 * It creates USB session and gets number of USB devices attached 
 * and then returns number of USB devices attached.
 * @return If successful then number of USB devices attached is returned and if it fails then -1 is returned.
 */

ssize_t usbInit()
{
	ret = libusb_init(&ctx); //creating session
	if(ret<0)
	{
		return -1;
	}

	count = libusb_get_device_list(ctx, &devs); //get the list of devices
	if(count < 0)
	{
		return -1;
	}

	return count;
}

/**
 * This function takes one 2D array to store IDs (bus, device, manufacturer and vendor) for each USB device
 * and one 3D array to store path, manufacturer descriptor and vendor descriptor.
 * It returns 0 if successful and -1 if it fails. It is only to demonstrate and develop lsusb kind of app.
 * Similar functions can be made using libusb for other functionalities of USB.
 * @param ids 2D integer array argument.
 * @param strings 3D unsigned character argument.
 * @return 0 if successful and -1 if it fails.
 */

int usbGetDevices(int ids[][8], unsigned char strings[][3][256])
{
	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8]; 

	struct libusb_device_handle *handle = NULL;
    unsigned char buff[256];
    struct libusb_device_descriptor desc;

	while ((dev = devs[i++])!= NULL) 
	{
		ret = libusb_get_device_descriptor(dev, &desc);
		if (ret < 0) {
			return -1;
		}

		//printf("%04x:%04x (bus %d, device %d)",desc.idVendor, desc.idProduct,
		//	libusb_get_bus_number(dev), libusb_get_device_address(dev));

		ids[i-1][0] = desc.idVendor;
		ids[i-1][1] = desc.idProduct;
		ids[i-1][2] = libusb_get_bus_number(dev);
		ids[i-1][3] = libusb_get_device_address(dev);

		ret = libusb_get_port_numbers(dev, path, sizeof(path));
		if (ret > 0) 
		{
			//printf(" path: %d", path[0]);
			ids[i-1][4] = path[0];
			for (j = 1; j < ret; j++)
			{
				//printf(".%d", path[j]);
				ids[i-1][4+j] = path[j];
			}

			ids[i-1][4+j] = -1;
		}
		else
		{
			ids[i-1][4] = -1;
		}

        ret = libusb_open(dev, &handle);
        //printf(" Error = %d\n", ret);
        if (ret != 0) 
        {
            //printf("Couldn't open device\n");
            strings[i-1][0][0] = '\0';
            strings[i-1][1][0] = '\0';
            strings[i-1][2][0] = '\0';
            strings[i-1][3][0] = '\0';
            continue;
        }
        else
        {
        	ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buff, sizeof(buff));
            if (ret < 0)
            {
                //printf("Coudn't get Manufacturer string\n");
                strings[i-1][0][0] = '\0';
            }
            else
            {
            	//printf("Manufacturer is: %s\n", buff);
            	memcpy(strings[i-1][0], buff, sizeof(buff));
            }

            ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, buff, sizeof(buff));
            if (ret < 0)
            {
                //printf("Coudn't get Product string\n");
                strings[i-1][1][0] = '\0';
            }
            else
            {
            	//printf("Product is: %s\n", buff);
            	memcpy(strings[i-1][1], buff, sizeof(buff));
            }

            ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buff, sizeof(buff));
            if (ret < 0)
            {
                //printf("Coudn't get SerialNumber string\n");
                strings[i-1][2][0] = '\0';
            }
            else
            {
            	//printf("SerialNumber is: %s\n", buff);
            	memcpy(strings[i-1][2], buff, sizeof(buff));
            }

            libusb_close(handle);
            //printf("\n");

        }
	}

	return 0;
}

/**
 * This function is used to close USB devices.
 */

void usbClose()
{
	libusb_free_device_list(devs, 1);
	libusb_exit(ctx);
}