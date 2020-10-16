#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdint.h>

#include "hidapi.h"

#define MAX_STR 255

int main(int argc, char *argv[])
{
    int res;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;

	if(argc!=2){
		printf("no argument\n");
		printf("write brightness in %%\n");
		exit(1);
	}
	uint8_t brightness = atoi(argv[1]);

	// Initialize the hidapi library
	res = hid_init();
	if (res)
		return -1;

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	// handle = hid_open(0x03eb, 0x2402, NULL);
	handle = hid_open_path("/dev/hidraw2");
	if(!handle) {
		printf("unable to open device\n");
		return 1;
	}

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	printf("Manufacturer String: %S\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	printf("Product String: %S\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	printf("Serial Number String: (%d) %S\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	printf("Indexed String 1: %S\n", wstr);

	memset(buf,0x00,sizeof(buf));

	buf[0] = 0x4;
	buf[1] = brightness;

	printf("writing %d to device\n",brightness);

	res = hid_write(handle, buf, 2);
	if (res < 0){
		printf("Unable to write()\n");
		printf("hid error: %ls\n", hid_error(handle));
	}	

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	//buf[0] = 0x0;
	//buf[1] = 0x80;
	//res = hid_write(handle, buf, 65);

	// Request state (cmd 0x81). The first byte is the report number (0x0).
	//buf[0] = 0x0;
	//buf[1] = 0x81;
	//res = hid_write(handle, buf, 65);

	// Read requested state
	//res = hid_read(handle, buf, 65);

	// Print out the returned buffer.
	//for (i = 0; i < 4; i++)
		//printf("buf[%d]: %d\n", i, buf[i]);

	// Close the device
	hid_close(handle);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
