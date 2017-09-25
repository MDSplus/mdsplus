/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <pxi-6259-lib.h>
#include <xseries-lib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#define RESOURCE_NAME_DAQ "/dev/pxi6259"

#define TRIANGULAR 1
#define SQUARE 2
#define SINUSOIDAL 3


static void createTriangularWaveform(int number_of_samples, double offset, double level, float *buf)
{
    int i;

    // create one complete triangular period in volts

    double m = 4./number_of_samples;

    for(i = 0; i <  number_of_samples/4; i++)
            buf[i] = (float)(offset + level * m * i);

    for(i = number_of_samples/4; i <  3 * number_of_samples / 4; i++)
            buf[i] = (float)(offset + level * (-m * i + 2) );

    for(i = 3 * number_of_samples/ 4; i <  number_of_samples ; i++)
            buf[i] = (float)(offset + level * (m * i - 4 ));

}

static void createSquareWaveform(int number_of_samples, double offset, double level, float *buf)
{
    int i;

    // create one complete square period in volts

    double m = 4./number_of_samples;

    for(i = 0; i <  number_of_samples/2; i++)
            buf[i] = (float)(offset + level);

    for(i = number_of_samples/2; i <  number_of_samples ; i++)
            buf[i] = (float)(offset - level);

}

static void createSinusoidalWaveform(int number_of_samples, double offset, double level, float *buf)
{
    int i;

    // create one complete sinus period in volts

    double k = (2 * 3.14)/number_of_samples ;

    for(i = 0; i <  number_of_samples; i++)
            buf[i] = (float)offset + (float)(level) * sin( k * i );

}

uint32_t generateWaveformOnOneChannel_6368(uint8_t selectedCard, uint8_t channel, double offset, double level, uint32_t waverate, int waveType)
{
    int silent = 0;
    int retval = 0;
    int number_of_channels = 4;
    int number_of_samples = 4000;
    const char *device_name = "/dev/pxie-6368";

    uint32_t sampleRate;
    uint32_t update_period_divisor;


    sampleRate = number_of_samples * waverate;        
    update_period_divisor = 100000000 / sampleRate;


	int ao_fd = 0;
	int dev_fd = 0;
	int ao_chan_fd[number_of_channels];

	char str[100];
	int i = 0, k = 0;

        double pi;
        double radianPart;

	float *write_array[number_of_channels];
	xseries_ao_conf_t ao_conf;

        for( i = 0; i < number_of_channels; i++ )
        {
        	write_array[i] = (float *)calloc( 1, sizeof(float) * number_of_samples );
    	}

	//memset (write_array, 0, sizeof(write_array));

	/* open AO segment file descriptor */
	sprintf(str,"%s.%u.ao",device_name,selectedCard);
	ao_fd = open(str, O_RDWR);
	if (ao_fd <= 0) {
		if(!silent) printf("Could not open AO segment! %s \n", strerror(errno));
		retval = -1;
		goto out_6368;
	}

	/* Stop the segment */
	xseries_stop_ao(ao_fd);

	/* reset AO segment */
	retval = xseries_reset_ao(ao_fd);
	if(retval) {
		if(!silent) printf("Error reseting card!\n");
		goto out_6368;
	}

	/*
	* Configure AO segment
	*/

	/* Create AO configuration */


	ao_conf = xseries_continuous_regenerated_ao(number_of_samples);

	/* Disable external gating of the sample clock */
	retval = xseries_set_ao_external_gate(&ao_conf,
			XSERIES_AO_EXTERNAL_GATE_DISABLED,	// No external pause signal
			XSERIES_AO_POLARITY_RISING_EDGE,	// Don't care
			0);					// Disable
	if(retval) {
		if(!silent) printf("Error setting external gate!\n");
		goto out_6368;
	}

	/* Program the START1 signal (start trigger) to assert from a software rising edge */

	retval = xseries_set_ao_start_trigger(&ao_conf,
			XSERIES_AO_START_TRIGGER_SW_PULSE,		// Set the line to software-driven
			XSERIES_AO_POLARITY_RISING_EDGE,	// Make line active on rising...
			1);					//   ...edge (not high level)
	if(retval) {
		if(!silent) printf("Error setting start trigger!\n");
		goto out_6368;    
	}


	/* Program the START1 signal (start trigger) to assert from a software rising edge */
/*
	retval = xseries_set_ao_start_trigger(&ao_conf,
			XSERIES_AO_START_TRIGGER_PFI1,		// Set the line to PFI1-driven
			XSERIES_AO_POLARITY_RISING_EDGE,	// Make line active on rising...
			1);					//   ...edge (not high level)
	if(retval) {
		if(!silent) printf("Error setting start trigger!\n");
		goto out_6368;    
	}
*/


	/* Program the Update source */
	retval = xseries_set_ao_update_counter(&ao_conf,
			XSERIES_AO_UPDATE_COUNTER_UI_TC,	// Derive the clock line from the Update Interval Terminal Count
			XSERIES_AO_POLARITY_RISING_EDGE);	// Make the line active on rising edge
	if(retval) 
        {    
		if(!silent) printf("Error setting update counter!\n");
		goto out_6368;    
	}

	/* Program the Update Interval counter */
	retval = xseries_set_ao_update_interval_counter(&ao_conf,
			XSERIES_OUTTIMER_UPDATE_INTERVAL_COUNTER_TB3,	// Source the Update Interval from the internal timebase
			XSERIES_OUTTIMER_POLARITY_RISING_EDGE,		// Make the line active on rising edge
			update_period_divisor, 				// Number of clock intervals between successive updates
			2						// Number of clock intervals after the start trigger before the first update
			);
	if(retval) {
		if(!silent) printf("Error setting update interval counter!\n");
		goto out_6368;
	}    


	/* Add channels */
	for (i=0; i<number_of_channels; i++) {
		if( i == channel )
		{
			retval = xseries_add_ao_channel(&ao_conf, i, XSERIES_OUTPUT_RANGE_10V);
			if(retval) {
				if(!silent) printf("Cannot add AI channel %d to configuration!\n",i);
				goto out_6368;
			}
		}
	}

	/* load configuration to the device */
        retval = xseries_load_ao_conf(ao_fd, ao_conf);
	if(retval) {
		if(!silent) printf("Cannot load AO configuration! %s (%d)\n",strerror(errno),errno);
		goto out_6368;
	}

	/* wait for the AO devices */
	sleep(1);

	for (i=0; i<number_of_channels; i++) {

        	switch (waveType)
        	{        
    	    		case TRIANGULAR : createTriangularWaveform(number_of_samples, offset, level,  write_array[i]); break;
    	    		case SQUARE : createSquareWaveform(number_of_samples, offset, level,  write_array[i]); break;
    	    		case SINUSOIDAL : createSinusoidalWaveform(number_of_samples, offset, level,  write_array[i]); break;
        	}
    	}

	/* Open channels */
	for (i=0; i<number_of_channels; i++) {
		sprintf(str,"%s.%u.ao.%u",device_name, selectedCard ,i);
		if( i == channel )
		{

        		printf("%s\n", str);
			ao_chan_fd[i] = open(str, O_RDWR | O_NONBLOCK);
			if(ao_chan_fd[i] < 0) {
				if(!silent) printf("Cannot add AO channel %d to configuration!\n",i);
				goto out_6368;
			}
		}
	}

	/* Write samples */
       for (i=0; i<number_of_samples; i++) {
		for(k=0; k<number_of_channels; k++) {
			if( k == channel )
			{
				// Write is not blocking !!!
				retval = xseries_write_ao(ao_chan_fd[k], &write_array[k][i], 1);
            			//if( i < 10 )
				//    printf("write_array[%d][%d] = %f\n", k, i, write_array[k][i]);
				if(retval == -1 && errno != EAGAIN) {
					if(!silent) printf("Error writing samples to FIFO buffer!\n");
					goto out_6368;
				}
			}
		
		}
        }

	/* put segment in started state */
	retval = xseries_start_ao(ao_fd);
	if(retval) {
		printf("Cannot start AO segment! %s (%d)\n",strerror(errno),errno);
		goto out_6368;
	}

	/* pulse start trigger */
	retval = xseries_pulse_ao(ao_fd, XSERIES_START_TRIGGER);
	if(retval) {
		printf("Error generating start pulse!\n");
		goto out_6368;
	}

    	printf ("Press ENTER to stop signal generation!\n");
    	getchar();

//  sleep(10);

out_6368:

	/* stop the segment */
	xseries_stop_ao(ao_fd);

	/* close all used file descriptors */
	for (i=0; i<number_of_channels; i++) {
		if( i == channel )
			close(ao_chan_fd[i]);
	}

	/* reset segment */
	xseries_reset_ao(ao_fd);

	/* close segment's file descriptor */
	close(ao_fd);

	/* close card's file descriptor */
	close(dev_fd);

        for( i = 0; i < number_of_channels; i++ )
        {
             free( write_array[i] );
        }

	return retval;



}


uint32_t generateWaveformOnOneChannel_6259 (uint8_t selectedCard, uint8_t channel, double offset, double level, uint32_t waverate, int waveType)
{
        uint32_t retval = 0;
        pxi6259_ao_conf_t ao_conf;

        int fdConfig = 0;
        int fdChannel = 0;

        uint32_t sampleRate;

        uint32_t periodDivisor;

        uint32_t number_of_samples = 1000;
        uint32_t i;
        char str[32];

        double pi;
        double radianPart;

        float scaledWriteArray[number_of_samples];

        sampleRate = number_of_samples * waverate;        

        periodDivisor = 20000000 / sampleRate;

        // get configuration file descriptor
        sprintf(str,"%s.%d.ao",RESOURCE_NAME_DAQ, selectedCard);
        fdConfig = open(str, O_RDWR | O_NONBLOCK);
        if (fdConfig < 0) {
                printf ("Error Opening Device! fd: %d\n",fdConfig);
                return -1;
        }


/*
        pi = 4.0 * atan(1.0);
        radianPart = 2*pi / number_of_samples;
        for (i=0; i<number_of_samples; i++) {
                scaledWriteArray[i] = (float)(7 * sin((double) radianPart*i));
        }
*/


        switch (waveType)
        {        
    	    case TRIANGULAR : createTriangularWaveform(number_of_samples, offset, level,  scaledWriteArray); break;
    	    case SQUARE : createSquareWaveform(number_of_samples, offset, level,  scaledWriteArray); break;
        }


        ao_conf = pxi6259_create_ao_conf();

        if (pxi6259_set_ao_waveform_generation(&ao_conf, 1))
                return -1;

        if (pxi6259_add_ao_channel(&ao_conf, channel, AO_DAC_POLARITY_BIPOLAR))
                return retval;

        if (pxi6259_set_ao_count(&ao_conf, number_of_samples, 1, 1))
                return -1;

        if (pxi6259_set_ao_update_clk(&ao_conf, AO_UPDATE_SOURCE_SELECT_UI_TC,
                        AO_UPDATE_SOURCE_POLARITY_RISING_EDGE, periodDivisor))
                return -1;

        retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
        if (retval) {
                printf ("err: load task. retval: %x\n",retval * -1);
                goto out_6259;
        }

        sleep(1);

        // Open channels
        sprintf(str,"%s.%d.ao.%d",RESOURCE_NAME_DAQ,selectedCard,channel);
        fdChannel = open(str, O_RDWR | O_NONBLOCK);
        if (fdChannel < 0) {
                printf ("Error Opening Channel! FD: %d\n",fdChannel);
                return -1;
        }

        retval = pxi6259_write_ao(fdChannel, scaledWriteArray, number_of_samples);
        if (retval != number_of_samples) {
                printf ("err: writing. retval: %d\n",retval);
                goto out_6259;
        }

        retval = pxi6259_start_ao(fdConfig);
        if (retval) {
                printf ("err: Starting task. retval: %d\n",retval);
                return -1;
        }

        printf ("Press ENTER to stop signal generation!\n");
        getchar();

out_6259:
        if (pxi6259_stop_ao(fdConfig))
                printf ("err: Stoping task. retval: %x\n",retval * -1);

        close(fdChannel);
        //sleep(1);
/*
        retval = pxi6259_load_ao_conf(fdConfig, &ao_conf);
        if (retval) {
                printf ("err: load task. retval: %x\n",retval * -1);
                goto out_6259;
        }

        // Open channels
        sprintf(str,"%s.%d.ao.%d",RESOURCE_NAME_DAQ,selectedCard,channel);
        fdChannel = open(str, O_RDWR | O_NONBLOCK);
        if (fdChannel < 0) {
                printf ("Error Opening Channel! FD: %d\n",fdChannel);
        }

        for( i = 0; i <  number_of_samples; i++)
            scaledWriteArray[i] = (float)0.;

        retval = pxi6259_write_ao(fdChannel, scaledWriteArray, number_of_samples);
        if (retval != number_of_samples) {
                printf ("err: writing. retval: %d\n",retval);
        }

        retval = pxi6259_start_ao(fdConfig);
        if (retval) {
                printf ("err: Starting task. retval: %d\n",retval);
        }

        sleep(1);

        if (pxi6259_stop_ao(fdConfig))
                printf ("err: Stoping task. retval: %x\n",retval * -1);
*/


        close(fdChannel);
        close(fdConfig);

        return 0;
}

int main (int argc, char** argv){
        int retval = 0;
        uint8_t selectedCard = 0;
        uint8_t channel = 0;
        uint32_t frequency = 0;
        double offset;
        double level;
        double minValue;
        double maxValue;
        const char *device;
        int   waveType; 


        if (argc == 8 && ( !strcmp( argv[1], "6368") || !strcmp( argv[1], "6259") ) )   
        {
                device = argv[1];
                selectedCard = atoi(argv[2]);
                channel = atoi(argv[3]);
                minValue = atof(argv[4]);
                maxValue = atof(argv[5]);
                frequency = atoi(argv[6]);
                if( strcmp( argv[7], "T" ) == 0 )
                    waveType = TRIANGULAR;
                else  if( strcmp( argv[7], "Sq" ) == 0)
                    waveType = SQUARE;
                else  if( strcmp( argv[7], "Si" ) == 0 )
                    waveType = SINUSOIDAL;

        } 
        else if ( argc == 1 ) 
        {
                device = "6368";
                selectedCard = 0;
                channel = 0;
                minValue = 0.0;
                maxValue = 5.0;
                frequency = 100;
                waveType = TRIANGULAR;
                printf ("Default values: device: 6368 card: 0, channels: 0, MinValue : 0.0, MaxValue : 5.0, ferquency : 100, Wave : Triangular\n\n");

        } else {
                printf ("\nPlease define arguments for AO waveform signal generation example! Run as:\n");
                printf ("\n./generateTriangularWave [device {6368/6259}] [card] [channel (0..3)] [Min value] [Max value] [frequency] [<T>riangular | <Sq>uare | <Si>nusoidal ])\n");
                printf ("\nIf only one argument is provided, preset settings will be chosen!\n");
                printf ("Preset values: device: 6368 card: 0, channels: 0, MinValue: 0, MaxValue : 5, ferquency : 100, waveType : T(riangular)\n\n");
                return 0;
        }

	level = ( maxValue - minValue ) /2.;
	offset = ( maxValue + minValue ) / 2.;

	printf("Offset %f level %f\n", offset, level);

        if(!strcmp( device, "6368"))
        {
            retval = generateWaveformOnOneChannel_6368(selectedCard, channel, offset, level, frequency, waveType);
        }
        if(!strcmp( device, "6259"))
        {
            retval = generateWaveformOnOneChannel_6259(selectedCard, channel, offset, level, frequency, waveType);
        }
        if (retval) {
                printf ("Error generating waveform signal\n");
                return -1;
        }

        return 0;
}
