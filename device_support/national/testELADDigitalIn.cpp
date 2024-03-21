/*
 * dioex1.c
 *
 * dioex1 demonstrates software-timed, static DIO operation using DIO ports 0, 1, and 2.
 *
 */

#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include <xseries-lib.h>

int number_of_ports = 3;

int test(char * device_name, int silent)
{

	xseries_di_conf_t di_conf;
	xseries_do_conf_t do_conf;


	int retval = 0;

	int dio_fd = 0;
	int pfi_fd = 0;
	int trig_fd = 0;

	char str[100];

	//uint32_t dio_output_mask = 0x1A2B3C4D;//1 1010 0010 1011 0011 1100 0100 1101
	//uint32_t dio_output_mask = 0xF0F0F;         //		      1111 0000 1111 0000 1111
    uint32_t dio_output_mask = 0;         //		      all input
    uint32_t dio_input_mask = (~dio_output_mask) & 0xFFFFFFFF;

	

    //uint16_t pfi_output_mask = 0x5E6F;//101 1110 0110 1111
    //uint16_t pfi_output_mask = 0xF;	 //		        1111
    uint16_t pfi_output_mask = 0;	    //		        all input
    uint16_t pfi_input_mask = (~pfi_output_mask) & 0xFFFF;

    uint32_t do_value = 0x98765ADF;//1001 1000 0111 0110 0101 1010 1101 1111
    uint32_t di_value = 0;

    uint32_t pfi_write_value = 0x4665;//100 0110 0110 0101
    uint32_t pfi_read_value = 0;

	/* open DIO segment file descriptor */
	sprintf(str,"%s.dio",device_name);
	dio_fd = open(str, O_RDWR);
	if (dio_fd <= 0) {
		printf("Could not open AO segment!\n");
		retval = -1;
		goto out;
	}

	/* open PFI segment file descriptor */
	sprintf(str,"%s.pfi",device_name);
	pfi_fd = open(str, O_RDWR);
	if (pfi_fd <= 0) {
		printf("Could not open AO segment!\n");
		retval = -1;
		goto out;
	}

	/* open TRIG segment file descriptor */
	sprintf(str,"%s.trig",device_name);
	trig_fd = open(str, O_RDWR);
	if (trig_fd <= 0) {
		printf("Could not open AO segment!\n");
		retval = -1;
		goto out;
	}

	/* Stop the segment */
	xseries_stop_di(dio_fd);
	xseries_stop_do(dio_fd);

	/* reset DIO segment */
	retval = xseries_reset_dio(dio_fd);
	if (retval != 0) {
		printf("Error reseting DIO segment!\n");
		goto out;
	}

	/* free PFI lines */
    retval = xseries_free_pfi_lines(trig_fd,0xFFFF);
    if (retval != 0) {
		printf ("Err, cannot free all PFI lines: %s\n", strerror(errno));
		goto out;
    }

	/* reset PFI segment */
	retval = xseries_reset_pfi(trig_fd);
	if (retval != 0) {
		printf("Error reseting PFI segment!\n");
		goto out;
	}

	/*
	 * Configure DIO segment
	 */

	/* Create AO configuration */
	di_conf = xseries_static_di(dio_input_mask);
	do_conf = xseries_static_do(dio_output_mask);

	/* Configure PFI/TRIG segment */
    retval = xseries_set_pfi_for_input(trig_fd, pfi_input_mask);
	if (retval != 0) {
		printf("Error setting PFI input mask!\n");
		goto out;
	}
    retval = xseries_set_pfi_for_output(trig_fd, pfi_output_mask, XSERIES_PFI_OUTPUT_DO);
	if (retval != 0) {
		printf("Error setting PFI output mask!\n");
		goto out;
	}

	/* Load DIO configuration */
    retval = xseries_load_di_conf(dio_fd, di_conf);
	if (retval != 0) {
		printf("Error loading DI configuration!\n");
		goto out;
	}
        retval = xseries_load_do_conf(dio_fd, do_conf);
	if (retval != 0) {
		printf("Error loading DO configuration!\n");
		goto out;
	}

	/* start DIO*/
    retval = xseries_start_di(dio_fd);
	if (retval != 0) {
		printf("Error loading DI configuration!\n");
		goto out;
	}
    retval = xseries_start_do(dio_fd);
	if (retval != 0) {
		printf("Error loading DO configuration!\n");
		goto out;
	}

	printf("Read on P0 PFI0 ");
	scanf("%d", &do_value);

    while(1)
	{

/*
		printf("Write on DIO P0 [ con0 : 0 to 3 con1 : 8 to 11 ] ");
		scanf("%d", &do_value);

		do_value = 1 << do_value;

		// Write values to ports 
		retval = xseries_write_do(dio_fd,&do_value,1);
		if (retval != 1) {
			printf("Error writing DO value!\n");
			break;
		}

		printf("Write on DIO  [con 0 : PFI 0 to 3 con1 : PO 16 to 19 ] ");
		scanf("%d", &pfi_write_value);

		if( pfi_write_value < 16)
		{

			pfi_write_value = 1 << pfi_write_value;
			retval = xseries_write_pfi(pfi_fd, &pfi_write_value, 1);
			if (retval != 1) {
				printf("Error writing PFI value!\n");
				break;
			}
		} else {

			do_value = do_value | 1 << pfi_write_value;

			// Write values to ports 
			retval = xseries_write_do(dio_fd,&do_value,1);
			if (retval != 1) {
				printf("Error writing DO value!\n");
				break;
			}
		}
*/
		/* Read written values */
		
		retval = xseries_read_di(dio_fd,&di_value,1);
		if (retval != 1) {
			printf("Error reading DI value!\n");
			goto out;
		}
		//printf("Read P0 %d\n", di_value);
		if( di_value != 0)
		   printf("%d-", di_value);

		retval = xseries_read_pfi(pfi_fd,&pfi_read_value,1);
		if (retval != 1) {
			printf("Error reading PFI value!\n");
			goto out;
		}
		//printf("Read PFI %d\n", pfi_read_value);

		sleep(0.1);


    }
   

	/* Read written values */
	/*
	retval = xseries_read_di(dio_fd,&di_value,1);
	if (retval != 1) {
		printf("Error reading DI value!\n");
		goto out;
	}
	retval = xseries_read_pfi(pfi_fd,&pfi_read_value,1);
	if (retval != 1) {
		printf("Error reading PFI value!\n");
		goto out;
	}
	*/
	/* Read PFI and DIO port value */
	/*
	if((di_value & dio_output_mask) != (do_value & dio_output_mask)) {
		printf("Read DIO value not correct!\n");
		retval = -1;
	}
	if((pfi_read_value & pfi_output_mask )!= (pfi_write_value & pfi_output_mask)) {
		printf("Read PFI value not correct!\n");
		retval = -1;
	}
	*/
	/* Read and write functions return number of samples so retval must be reset to 0 */
	retval = 0;

out:

	/* stop the segment */
	xseries_stop_di(dio_fd);
	xseries_stop_do(dio_fd);

	/* Free PFI lines */
        xseries_free_pfi_lines(trig_fd, 0XFFFF);

	/* reset segment */
	xseries_reset_dio(dio_fd);
	xseries_reset_pfi(pfi_fd);

	/* close segment's file descriptor */
	close(dio_fd);
	close(pfi_fd);

	return retval;
}

void print_text(void){
	printf ("\n");

	printf("Test software-timed, static DIO and PFI operation using DIO and PFI ports.\n");
	printf ("\n");
}

int main (int argc, char** argv) {

	int retval = 0;
	char * device_name;
	int silent = false;

	if (argc == 2) {
		device_name = argv[1];
		print_text();
	} else if (argc == 3) {
		device_name = argv[1];
		if(strcmp(argv[2],"-silent") != 0) {
			print_text();
			silent = false;
	       } else {
			silent = true;
		}
	} else {
		printf ("Usage:  %s <device name>\n", argv[0]);
		printf ("Example:\n        %s /dev/pxie-6368.0\n", argv[0]);
		printf ("\n");
		exit(EXIT_FAILURE);
	}

	if (silent) {
		printf("%-10s%-100s",
				argv[0],
				"Test software-timed, static DIO and PFI operation using DIO and PFI ports.");
		fflush(stdout);
	}

	/* run test */
	retval = test(device_name,silent);

	if (!silent) {
		printf("Test:			");
		fflush(stdout);
	}

	printf("[%s]\n",retval!=0? "FAILED":"OK");

	if(!silent) { printf ("\n");}

	return retval;
}

