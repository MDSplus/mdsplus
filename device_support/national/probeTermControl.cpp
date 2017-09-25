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
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <semaphore.h>

#include <mdsobjects.h>
using namespace MDSplus;


#define DEVICE_FILE "/dev/pxi6259"

void usage(char *name) {
        printf(" Usage: %s <device number> <input channel 1 > <input channel 2 > <frequency> <output channel ref> <ouput channel on/off>\n"
                "Example: %s 0 10 11 10 0 1\n", name, name);
}


int configureOutput(int *chanOutFD, uint32_t deviceNum, uint32_t outChanRef , uint32_t outChanOnOff)
{
        char filename[256];
        int i;
        pxi6259_ao_conf_t aoConfig;
        int devFD;

        // open AO file descriptor
        sprintf(filename, "%s.%u.ao", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AO configuration
        aoConfig = pxi6259_create_ao_conf();

        // configure AO channel reference
        if (pxi6259_add_ao_channel(&aoConfig, outChanRef, AO_DAC_POLARITY_BIPOLAR)) {
                fprintf(stderr, "Failed to configure channel %d reference! %s\n", outChanRef, strerror(errno));
                return -1;
        }

        // configure AO channel on/off
        if (pxi6259_add_ao_channel(&aoConfig, outChanOnOff, AO_DAC_POLARITY_BIPOLAR)) {
                fprintf(stderr, "Failed to configure channel %d reference! : %s\n", outChanOnOff, strerror(errno));
                return -1;
        }

        // enable signal generation
        if (pxi6259_set_ao_attribute(&aoConfig, AO_SIGNAL_GENERATION, AO_SIGNAL_GENERATION_STATIC)) {
                fprintf(stderr, "Failed to enable generating static signal!: %s\n", strerror(errno));
                return -1;
        }

        // set continuous mode
        if (pxi6259_set_ao_attribute(&aoConfig, AO_CONTINUOUS, 0)) {
                fprintf(stderr, "Failed to set continuous mode!: %s\n", strerror(errno));
                return -1;
        }

        // load AO configuration and let it apply
        if (pxi6259_load_ao_conf(devFD, &aoConfig)) {
                fprintf(stderr, "Failed to load output configuration! : %s\n", strerror(errno));
                return -1;
        }

        // open file descriptor for each AO channel
        sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanRef);
        chanOutFD[0] = open(filename, O_RDWR | O_NONBLOCK);
        if ( *chanOutFD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", outChanRef, strerror(errno));
                return -1;
        }

        sprintf(filename, "%s.%u.ao.%u", DEVICE_FILE, deviceNum, outChanOnOff);
        chanOutFD[1] = open(filename, O_RDWR | O_NONBLOCK);
        if ( *chanOutFD < 0) {
                fprintf(stderr, "Failed to open channel %u: %s\n", outChanOnOff , strerror(errno));
                return -1;
        }

        // start AO segment (signal generation)
        if (pxi6259_start_ao(devFD)) {
                fprintf(stderr, "Failed to start segment!n");
                return -1;
        }

	return devFD;
}


int configureInput(int *chanInFd, uint32_t deviceNum, uint32_t inChan[], double frequency, int numChan) 
{
	int diffMapChannel[16] = {-1,0,1,2,3,4,5,6,7,16,17,18,19,20.21,22,23};
        char filename[256];
        int i;
        pxi6259_ai_conf_t aiConfig;
        int devFD;
	int period;

        // open AI file descriptor
        sprintf(filename, "%s.%u.ai", DEVICE_FILE, deviceNum);
        devFD = open(filename, O_RDWR);
        if (devFD < 0) {
                fprintf(stderr, "Failed to open device: %s\n", strerror(errno));
                return -1;
        }

        // initialize AI configuration
        aiConfig = pxi6259_create_ai_conf();

        // configure AI channels 0 - 5 V differential
	for( int i = 0; i < numChan; i++)
	{

		if (pxi6259_add_ai_channel(&aiConfig, diffMapChannel[inChan[i]], AI_POLARITY_BIPOLAR, 2, AI_CHANNEL_TYPE_DIFFERENTIAL, 0))
/*
		printf("Channel conf %d\n", diffMapChannel[inChan[i]] );
		if (pxi6259_add_ai_channel(&aiConfig, diffMapChannel[inChan[i]], AI_POLARITY_BIPOLAR, 2, AI_CHANNEL_TYPE_RSE, 0))
 
*/		{

		        fprintf(stderr, "Failed to configure channel %u\n", inChan[i]);
		        return -1;
		}
	}

        // configure AI sampling clock to sample with 2 samples/second
	period = (int) ( 20000000. / frequency );
        if (pxi6259_set_ai_sample_clk(&aiConfig, period, 3, AI_SAMPLE_SELECT_SI_TC, AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE)) {
                fprintf(stderr, "Failed to configure AI sampling clock!\n");
                return -1;
        }

        // load AI configuration and let it apply
        if (pxi6259_load_ai_conf(devFD, &aiConfig)) {
                fprintf(stderr, "Failed to load input configuration!\n");
                return -1;
        }

        // open file descriptor for each AI channel
	for( int i = 0; i < numChan; i++)
	{
		sprintf(filename, "%s.%u.ai.%u", DEVICE_FILE, deviceNum, diffMapChannel[inChan[i]]);
		printf("%s\n", filename);
		chanInFd[i] = open(filename, O_RDWR | O_NONBLOCK);
		if (chanInFd[i] < 0) {
		        fprintf(stderr, "Failed to open channel %u: %s\n", inChan[i], strerror(errno));
		        return -1;
		}
	}

	// start AI segment (data acquisition)
	if (pxi6259_start_ai(devFD)) {
	        fprintf(stderr, "Failed to start data acquisition!\n");
	        return -1;
	}

        usleep(100);

	return devFD;


}

int main(int argc, char** argv) 
{
	static char log[5] = "|/-\\";
	static int count = 0;


        uint32_t deviceNum;
	int numChan = 2;
        uint32_t inChan[2];
        double    frequency;
        uint32_t outChanRef;
        uint32_t outChanOnOff;
	uint32_t rc;
	

	int chanInFD[2];
	int devInFD;

	int chanOutFD[2];
	int devOutFD;

	TreeNode *node[16];
	int wakeState = 1;
	int error = 0;

	float ZERO_VOLT = 0;
	float FIVE_VOLT = 5.;


        if (argc != 7) {
                usage(argv[0]);
                return 1;
        }

        deviceNum = strtoul(argv[1], NULL, 10);
        inChan[0] = strtoul(argv[2], NULL, 10);
	printf("ch %d\n", inChan[0] );
        inChan[1] = strtoul(argv[3], NULL, 10);
	printf("ch %d\n", inChan[1] );
        frequency = atof(argv[4]);
        outChanRef = strtoul(argv[5], NULL, 10);
        outChanOnOff = strtoul(argv[6], NULL, 10);


	sem_t * pauseSem_id;
	sem_t * wakeSem_id;

	pauseSem_id = sem_open("PauseControl", O_CREAT, 0666, 0);
	if(pauseSem_id == SEM_FAILED ) {
		perror("pause Control sem_open");
		exit(1);
	}

	wakeSem_id = sem_open("WakeControl", O_CREAT, 0666, 1);
	if(pauseSem_id == SEM_FAILED ) {
		perror("wake Control sem_open");
		exit(1);
	}
	

	try {
  		time_t rawtime;
  		struct tm * timeinfo;
		char strShot[256];
		char dataFile[256];
		unsigned long shot;
		Tree *t;

  		time (&rawtime);
  		timeinfo = localtime (&rawtime); 

		sprintf(strShot, "%d%0.2d%d", 1900+timeinfo->tm_year, timeinfo->tm_mon+1, timeinfo->tm_mday);
		shot = strtoul(strShot, NULL, 0);
		sprintf(dataFile, "%s/ipp_tc_trend_%d.datafile", getenv("ipp_tc_trend_path"), shot);

		//printf("date %s shot %d str shot %s file %s\n", asctime(timeinfo), shot, strShot, dataFile);

		FILE *fd = fopen(dataFile, "r");
		if( !fd  )
		{
			t = new Tree((char *)"ipp_tc_trend", -1);
			t->createPulse(shot);
		}
		else
		{
			fclose(fd);
		}

		t = new Tree((char *)"ipp_tc_trend", shot);

		for( int i = 0; i < numChan; i++ )
		{
			char path[256];
			sprintf(path, "\\IPP_TC_TREND::TC%d", inChan[i] );
			printf("NODO %s \n", path );		
			node[i] = t->getNode(path);
		}
	}
	catch(MdsException *exc) 
	{	
		printf("%s\n", exc->what());
		exit(1);
	}



	while(!error)
	{

		if( !wakeState )
		{
			printf("WAIT\n");
			if( sem_wait( pauseSem_id ) < 0 )
			{
				perror("Control sem_wait");
				return -1;
			}
			wakeState = 1;
		}

		if ( ( devInFD = configureInput( chanInFD, deviceNum, inChan, frequency, 2 ) ) < 0 ) {
		        fprintf(stderr, "Error configure input channel!\n");
		        return -1;
		}

		if ( ( devOutFD = configureOutput( chanOutFD, deviceNum, outChanRef, outChanOnOff ) ) < 0 ) {
		        fprintf(stderr, "Error configure ouput channel!\n");
		        return -1;
		}

		printf("START controll\n");

		// Control tc
		while (wakeState) 
		{
		        int n = 0;
			int i = 0;
			int nChRead;
			int scans_read[16];
		        float value[16];
		        float voltage;


			rc = pxi6259_write_ao(chanOutFD[1], &FIVE_VOLT, 1);
			if (rc < 0) {
			    fprintf(stderr, "Failed to write to AO channel ON/OFF: %u\n", outChanOnOff);
			    error = 1;
			    goto out;
			}

		
			memset( scans_read, 0, sizeof(scans_read) );
			nChRead = 0;
			while( nChRead < numChan )
			{
				if( scans_read[i] <= 0 )
				{
					//printf("read channel %d n chan %d \n", i, nChRead);
					scans_read[i] = pxi6259_read_ai(chanInFD[i], &value[i], 1);

					if (scans_read[i] < 0 ) 
					{
						if( errno != EAGAIN )
						{
				                	fprintf(stderr, "Failed while reading channel: %u: %s %d\n", inChan[i], strerror(errno), errno);
							error = 1;
				                	goto out;
						}
				        }
					else
					      nChRead++;
				}
				i = (i+1) % numChan;
			}


		        // Start Control algorithm
			voltage = value[0];
			// End control algoritm

			rc = pxi6259_write_ao(chanOutFD[0], &voltage, 1);
			if (rc < 0) {
			    fprintf(stderr, "Failed to write to AO channel reference: %u\n", outChanRef);
			    error = 1;
			    goto out;
			}

			struct timeb tb; 
			int64_t currTime;
			try
			{
				ftime( &tb);
		   		currTime =  (int64_t)(tb.time * 1000 + tb.millitm); 
				for(int i = 0; i < numChan; i++)
				{
		  			Float32 *currData = new Float32(value[i]);
		   			node[i]->putRow(currData, &currTime);
				}
			}
			catch(MdsException *exc) 
			{	
				printf("%s\n", exc->what());
				error = 1;
			    	goto out;
			}

			if( sem_getvalue( wakeSem_id, &wakeState ) < 0 )
			{
				perror("Control sem_open");
				error = 1;
		 		goto out;
			}
/*
			struct tm * timeinfo;
			time_t pp =  (time_t)( tb.time );
	  		timeinfo = localtime ( &pp );
		        printf(" %x Value[0] %f %s:%d\n\n Wake state %d \n", currTime, value[0], asctime(timeinfo), tb.millitm, wakeState);
		        printf("Value[0] = %f  \n", value[0]);
*/
			printf("\r%c", log[count], count);
		        fflush( stdout );
			count = (count+1)%4;

		}

	out:

		rc = pxi6259_write_ao(chanOutFD[0], &ZERO_VOLT, 1);
		if (rc < 0) {
		    fprintf(stderr, "Failed to write to AO channel reference: %u\n", outChanRef);
		    error = 1;
		    goto out;
		}

		rc = pxi6259_write_ao(chanOutFD[1], &ZERO_VOLT, 1);
		if (rc < 0) {
		    fprintf(stderr, "Failed to write to AO channel ON/OFF: %u\n", outChanOnOff);
		    error = 1;
		    goto out;
		}


		printf("Close open ADC fd\n");

		// stop AI segment
		if (pxi6259_stop_ai(devInFD)) {
		        fprintf(stderr, "Failed to stop data acquisition!\n");
		}

		// close file descriptors
		for(int i = 0; i < numChan; i++)
			close(chanInFD[i]);

		close(devInFD);

		// stop AO segment
		if (pxi6259_stop_ao(devOutFD)) {
		        fprintf(stderr, "Failed to stop generating signal!\n");
		        return -1;
		}

		
		close(chanOutFD[0]);
		close(chanOutFD[1]);
		close(devOutFD);
	}

        return 0;
}




