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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>



int main(int argc, char** argv) 
{
    sem_t * semPause_id;
    sem_t * semWake_id;
    int wakeState;
    int i;

    semPause_id = sem_open("PauseControl", O_CREAT, 0666, 0);
    if(semPause_id == SEM_FAILED) {
        perror("child sem_open");
        return 0;
    }
    
    semWake_id = sem_open("WakeControl", O_CREAT, 0666, 0);
    if(semWake_id== SEM_FAILED) {
        perror("child sem_open");
        return 0;
    }


    if(strcmp(argv[1], "stop") == 0 )
    {
	if( sem_getvalue( semWake_id, &wakeState ) < 0 )
	{
		perror("Control sem_getvalue");
		return 0;
	}

	printf("STOP wake state %d\n", wakeState);
	for( i = 0; i < wakeState; i++)
	{
		if( sem_wait( semWake_id  ) < 0 )
		{
			perror("sem_wait WakeControl");
			return 0;
		}
/*
		if( sem_post( semPause_id  ) < 0 )
		{
			perror("sem_post PauseControl");
			return 0;
		}
*/
	}   
    }
    
    if(strcmp(argv[1], "start") == 0 )
    {
	if( sem_getvalue( semWake_id, &wakeState ) < 0 )
	{
		perror("Control sem_getvalue");
		return 0;
	}

	printf("START wake state %d\n", wakeState);
	if ( wakeState == 0 )
	{
		printf("START pause state %d\n", wakeState);

		if( sem_post( semPause_id  ) < 0 )
		{
			perror("sem_post PauseControl");
			return 0;
		}
		if( sem_post( semWake_id  ) < 0 )
		{
			perror("sem_post WakeControl");
			return 0;
		}
	}   
    }

    printf("Posting for parent\n");
    return 0;
}
