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
