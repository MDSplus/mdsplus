#include <stdio.h>

/*
Program to wait for MDSPlus event from the command line.
*/

main(argc,argv)
int argc;
char *argv[];
{

    char data[20];
		int len;
    if(argc <= 1){
        fprintf(stderr,"Usage: %s <event name>\n",argv[0]);
        exit(1);
    }


    ipcs_init(NULL);    /*  this is an anonymous process  */


		/*mdsq_event(argv[1]);*/
		/*mdsq_event(argv[1]);*/

    len = 20;
		mdswtevent(argv[1],data,&len);
		printf("data from event = %s\n",data);


}

