#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <stdint.h>

#include "hidapi.h"

#define VENDOR_ID ((unsigned short) 0x03eb)
#define PRODUCT_ID ((unsigned short) 0x2402)

int main(int argc, char *argv[])
{
    int res;
	unsigned char buf[65];
	hid_device *handle;
    struct hid_device_info *devs, *cur_dev;
    char *mtouch_path;

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
    devs = hid_enumerate(VENDOR_ID, PRODUCT_ID);
    if(devs == NULL){
		printf("Devices with type: %04hx:%04hx not found\n", VENDOR_ID, PRODUCT_ID);
        return -1;
    }
	cur_dev = devs;
	while (cur_dev) {
        if((cur_dev->vendor_id == VENDOR_ID)&&(cur_dev->product_id == PRODUCT_ID)&&(cur_dev->interface_number == 3)){
            printf("Device Found\n  type: %04hx:%04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            printf("\n");
            printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            printf("  Product:      %ls\n", cur_dev->product_string);
            printf("  Release:      %hx\n", cur_dev->release_number);
            printf("  Interface:    %d\n",  cur_dev->interface_number);
            printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
            printf("\n");
            mtouch_path = malloc(sizeof(cur_dev->path));
            if(mtouch_path == NULL)
                return -1;
            strcpy(mtouch_path, cur_dev->path);
        }
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	// Open the device using the VID, PID,
	// and optionally the Serial number.
    handle = hid_open_path(mtouch_path);
    free(mtouch_path);
	if(!handle) {
		printf("unable to open device\n");
		return 1;
	}

	memset(buf,0x00,sizeof(buf));

	buf[0] = 0x4;
	buf[1] = brightness;

	printf("writing %d to device\n",brightness);

	res = hid_write(handle, buf, 2);
	if (res < 0){
		printf("Unable to write()\n");
		printf("hid error: %ls\n", hid_error(handle));
	}	
    printf("success\n");
	// Close the device
	hid_close(handle);

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
