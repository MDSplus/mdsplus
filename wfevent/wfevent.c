#include <stdio.h>

/*
Program to wait for MDSPlus event from the command line.
*/

main(argc,argv)
int argc;
char *argv[];
{

    char data1[20];
    char data2[20];
    char data3[20];
    char data4[20];
		int len;
    if(argc <= 1){
        fprintf(stderr,"Usage: %s <event name>\n",argv[0]);
        exit(1);
    }


    ipcs_init(NULL);    /*  this is an anonymous process  */


		/*mdsq_event(argv[1]);*/
		/*mdsq_event(argv[1]);*/

		mdsq_event("junk1");
		mdsq_event("junk2");
		mdsq_event("junk3");
		mdsq_event("junk4");
    len = 20;
		mdswtevent("junk1",data1,&len);
		printf("data from event = %s\n",data1);
		mdswtevent("junk2",data2,&len);
		printf("data from event = %s\n",data2);
		mdswtevent("junk3",data3,&len);
		printf("data from event = %s\n",data3);
		mdswtevent("junk4",data4,&len);
		printf("data from event = %s\n",data4);
		mdsdeq_all();
		/*mdswtevent(argv[1],data,&len);*/


}

