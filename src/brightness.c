/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#define VENDOR_ID ((unsigned short) 0xadca)
#define PRODUCT_ID ((unsigned short) 0x8000)
#define PATH_SZ 15

int main(int argc, char *argv[])
{
	int fd;
	int i, res, desc_size = 0;
	char buf[256];
	struct hidraw_report_descriptor rpt_desc;
	struct hidraw_devinfo info;
	char *hidraw = "/dev/hidraw";
	char path[PATH_SZ];

	if(argc!=2){
		printf("no argument\n");
		printf("write brightness in %%\n");
		exit(1);
	}
	uint8_t brightness = atoi(argv[1]);
	memset(&rpt_desc, 0x0, sizeof(rpt_desc));
	memset(&info, 0x0, sizeof(info));
	memset(buf, 0x0, sizeof(buf));

	for (i=0; i<20; i++){
		snprintf(path, PATH_SZ, "%s%d", hidraw, i);
		fd = open(path, O_RDWR|O_NONBLOCK);

		if (fd < 0) {
			perror("Unable to open device");
			return 1;
		}

		/* Get Raw Info */
		res = ioctl(fd, HIDIOCGRAWINFO, &info);
		if (res < 0) {
			perror("HIDIOCGRAWINFO");
		}	
		
		/* Check if its mfpou */
		if (((unsigned short)info.vendor == VENDOR_ID) & ((unsigned short)info.product == PRODUCT_ID)){
			/* Get Report Descriptor Size */
			res = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
			if (res < 0)
				perror("HIDIOCGRDESCSIZE");
			/* Check for touchscreen desc */
			if (desc_size < 120){
				close(fd);
			} else {
				//printf("mfpou touch found\n");
				break;
			}
		} else {
			close(fd);
		}
	}



	/* Get Raw Name */
	res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWNAME");
	else
		printf("Raw Name: %s\n", buf);

	/* Get Physical Location */
	res = ioctl(fd, HIDIOCGRAWPHYS(256), buf);
	if (res < 0)
		perror("HIDIOCGRAWPHYS");
	else
		printf("Raw Phys: %s\n", buf);

	/* Send a Report to the Device */
	buf[0] = 0x4; /* Report Number */
	buf[1] = brightness > 100 ? 100 : brightness;
	res = write(fd, buf, 2);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	} else {
		printf("write() wrote %d bytes\n", res);
		printf("brightness set %d \n", brightness);
	}

	close(fd);
	return 0;

}

