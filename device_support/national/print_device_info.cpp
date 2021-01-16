
/*
 * This example reads the device information and prints it out.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <xseries-lib.h>
#include <pxi-6259-lib.h>

int main(int argc, char **argv)
{
        int retval, fd;
        xseries_device_info_t devinfo;
	uint32_t serial_number;

        if (argc < 2) {
                printf ("Usage:  %s <device file descriptor> \n", argv[0]);
                printf ("Example:\n        %s /dev/pxie-6368.0 \n", argv[0]);
                printf ("Example:\n        %s /dev/pxi6259.0 \n", argv[0]);
                exit(EXIT_FAILURE);
        }

        /* Open device file descriptor */
        fd = open(argv[1], O_RDWR);
        if (fd < 0) {
                printf ("Err, opening device node: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

	if( strstr( argv[1], "6368" ) != 0 )
	{
		/* Get device info */
		retval = xseries_get_device_info(fd, &devinfo);
		if (retval) {
			printf ("Err, cannot read device info: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		printf ("DEVICE INFO\n");
		printf (" Name: %s\n", devinfo.device_name);
		printf (" Product ID: %#X\n", devinfo.product_id);
		printf (" Serial number: %#X\n", devinfo.serial_number);
		printf (" Number of ADCs: %d\n", devinfo.adc_number);
		printf (" Number of DACs: %d\n", devinfo.dac_number);
		printf (" Is simultaneous: %s\n", devinfo.is_simultaneous? "yes":"no");
		printf (" Number of DIO lines: %d\n", devinfo.port0_length);
	}

	if( strstr( argv[1], "6259" ) != 0 )
	{
		/* Get device info */
		retval = xseries_get_device_info(fd, &devinfo);
		retval = pxi6259_get_board_serial_number(fd, &serial_number);
		if (retval) {
			printf ("Err, cannot read device info: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		printf ("DEVICE INFO\n");
		printf (" Serial number: %#X\n", serial_number);
	}

        close(fd);

        exit(EXIT_SUCCESS);
}
