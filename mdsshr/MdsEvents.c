#ifdef vxWorks
int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid) {}
int MDSEventCan(void *eventid) {}
int MDSEvent(char *evname){}
#else
#ifdef WIN32
int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid) {}
int MDSEventCan(int eventid) {}
int MDSEvent(char *evname){}
#else

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
/* Just to avoid compiler complains */
#undef DTYPE_DOUBLE
#undef DTYPE_EVENT
#undef DTYPE_FLOAT

#include <ipdesc.h>

/* MDsEvent: UNIX and Win32 implementation of MDS Events */


#define SEM_ID 12
#define SHMEM_ID 75
#define MSG_ID 500

/* Shared memory organization: 

1) array of MAX_EVENTNAMES SharedEventNames structs
2) array of MAX_ACTIVE_EVENTS SharedEventInfo structs
*/


#define MAX_EVENTNAME_LEN 64 	 /* Maximum number of characters in event name */
#define MAX_ACTIVE_EVENTS 5000   /* Maximum number events concurrently dealt with by processes */
#define MAX_EVENTNAMES 	  1000   /* Maximum number of different event names */

#define MAX_DATA_LEN 64		 /* Maximum number of bytes to be broadcasted by events */


/* Description of event names */
struct SharedEventName {
    short refcount;  /* number of active event connections making reference to this name */
    char name[MAX_EVENTNAME_LEN];  /* name of the event */
};

struct SharedEventInfo{
    int nameid; /* Index of the corresponding name descriptor in the SharedEventName array */
		 /* if == -1 this item is not currently used */
    int msgkey;  /* message ID of the message queue connecting to the target process */
};

/* Description of the message sent ove msg queues: name of the event and up to 64 bytes of data */
struct EventMessage {
	long int mtype; 
	char name[MAX_EVENTNAME_LEN];
	char length;
	char data[MAX_DATA_LEN];
};




/* Process private part: Array of MAX_EVENTNAMES PrivateEventInfo structs */
struct PrivateEventInfo {
    char active;  /* indicates wether this descriptor is active, i.e. describing an event declared by the process */
    char name[MAX_EVENTNAME_LEN]; /* name of the event */
    void (*astadr)(void *, int, char *);	  /* ast routine address */
    void *astprm;		  /* ast routine parameter */
};


static int is_exiting = 0;

static int shmId = 0, semId = 0;
static int  msgId = 0, msgKey = MSG_ID;
static struct PrivateEventInfo private_info[MAX_ACTIVE_EVENTS];
static struct SharedEventInfo *shared_info = 0;
static struct SharedEventName *shared_name = 0;

static void initializeShared();
static void *handleMessage(void * dummy);


static int remote_local_initialized = 0;
static int remote_id = 0;

/************* OS dependent part ******************/

static char *getEnvironmentVar(char *name)
{
    char *trans =  getenv(name);
    if(!trans || !*trans) return NULL;
    return trans;
}


static void cleanup(int);

static int getLock()
{
    struct sembuf psembuf;

    if(!semId) /* Acquire semaphore id if not done*/
    {
	semId = semget(SEM_ID, 1, IPC_CREAT);
	if(semId == -1) return 0;
    }
    psembuf.sem_num = 0;
    psembuf.sem_op = -1;
    psembuf.sem_flg = SEM_UNDO;
    return semop(semId, &psembuf, 1);
}

static int releaseLock()
{
    struct sembuf vsembuf;

    if(!semId) return -1; /* Semaphore id must alredy be acquired*/
    {
	semId = semget(SEM_ID, 1, IPC_CREAT);
	if(semId == -1) return 0;
    }
    vsembuf.sem_num = 0;
    vsembuf.sem_op = 1;
    vsembuf.sem_flg = SEM_UNDO;
    return semop(semId, &vsembuf, 1);
}

     
static int attachSharedInfo()
{
    int i;
    int size = sizeof(struct SharedEventInfo) * MAX_ACTIVE_EVENTS + 
	sizeof(struct SharedEventName) * MAX_EVENTNAMES + 2 * sizeof(int);
    shmId = shmget(SHMEM_ID, size, 0777 | IPC_CREAT |IPC_EXCL);
    if(shmId != -1) /* If shared memory memory region not created yet, it must be intialized */
    { 
	shmId = shmget(SHMEM_ID, size, 0777|IPC_CREAT);
	if(shmId == -1) return -1;
	    /* Initialize shared memory structure */
	shared_name = shmat(shmId, 0, 0);
	if(shared_name == (void *)-1) 
	{
	    perror("shmat");
	    return -1;
	}
	shared_info = (struct SharedEventInfo *)&shared_name[MAX_EVENTNAMES];
	initializeShared();
    }
    else
    {
	shmId = shmget(SHMEM_ID, size, 0777 );
	shared_name = shmat(shmId, 0, 0);	    	    		
	if(shared_info == (void *)-1)
	{
	    perror("shmat");
	    return -1;
	}
	shared_info = (struct SharedEventInfo *)&shared_name[MAX_EVENTNAMES];
     }
    return 0;
}


static int readMessage(char *event_name, int *data_len, char *data)
{
    struct EventMessage message;
    int status; 
    if((status = msgrcv(msgId, &message, sizeof(message), 1, 0)) != -1)
    {
	strncpy(event_name, message.name, MAX_EVENTNAME_LEN - 1);
	*data_len = message.length;
	if(message.length > 0)
	    memcpy(data, message.data, message.length);
    }
    return status;
}

static pthread_t thread;

static void setHandle()
{
   pthread_t thread;
   if(!msgId)
    {
	/* get first unused message queue id */
	while((msgId = msgget(msgKey, 0777 | IPC_CREAT | IPC_EXCL)) == -1)
	    msgKey++;

#ifdef _DECTHREADS_
#if _DECTHREADS_ != 1
#define pthread_attr_default NULL
#endif
#else
#define pthread_attr_default NULL
#endif

	if(pthread_create(&thread, pthread_attr_default, handleMessage, 0) !=  0)
	    perror("pthread_create");
	
    } 
}

static int sendMessage(char *evname, int key, int data_len, char *data)
{
    struct EventMessage message;
    int status, msgid;

    strncpy(message.name, evname, MAX_EVENTNAME_LEN - 1);
    message.mtype = 1;
    message.length = data_len;
    if(data_len > 0)
    {
	if(data_len > MAX_DATA_LEN) data_len = MAX_DATA_LEN;
	memcpy(message.data, data, data_len);
    }

    status = msgid = msgget(key, 0777);
    if(msgid == -1)
	perror("msgget");
    else
    {
    	if((status = msgsnd(msgid, &message, sizeof(message), IPC_NOWAIT)) == -1)
	    perror("msgsend");
    }
    return status;
}

static void attachExitHandler(void (*handler)())
{
    if(atexit(handler) == -1)
	perror("atexit");
    signal(SIGTERM,handler);
}



static void releaseMessages()
{
    void *dummy;
    struct msqid_ds buf;
    if(!msgId) return;

    pthread_join(thread, &dummy);
    
    if(msgctl(msgId, IPC_RMID, &buf) == -1)
	    perror("msgsend");

}

/***************** End of OS dependent part ****************/




static void initializeShared()
{	
    int i;
    for(i = 0; i < MAX_EVENTNAMES; i++)
	shared_name[i].refcount = 0;
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
	shared_info[i].nameid = -1;
}


static void *handleMessage(void * dummy)
{
    int i;
    char event_name[MAX_EVENTNAME_LEN];
    int data_len;
    char data[MAX_DATA_LEN];
    while(1)
    {	
	if(readMessage(event_name, &data_len, data) != -1)
	{
    	    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    	    {
	        if(private_info[i].active && !strcmp(event_name, private_info[i].name))
		{
		    (*(private_info[i].astadr))(private_info[i].astprm, data_len, data);
		}
	    }
	}
	else
	    return NULL;
    }
    return NULL;
}



static void cleanup(int dummy)
{
/* remove all events declared by the process and not removed by MdsEventCan */
    int i, j;


    if(is_exiting) return;

    is_exiting = 1;
    if(!shared_info) return;

    getLock();
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	if(shared_info[i].msgkey == msgKey && shared_info[i].nameid >= 0)
	{
	    if(shared_name[shared_info[i].nameid].refcount > 0)
	    	shared_name[shared_info[i].nameid].refcount--;
	    shared_info[i].nameid = -1;
	}
    }
    releaseLock();

    releaseMessages();

}


/*    status = MDSEventAst(c->descrip[1]->pointer,(void (*)())ClientEventAst,newe,&newe->eventid); */


static void initializeLocalRemote()
{
    char *env_name;
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 12, "ConnectToMds"};
    int status;
    int (*rtn)();


    if(!remote_local_initialized)
    {
	remote_local_initialized = 1;
	env_name = getEnvironmentVar("mds_event_server");
	if(env_name != NULL)
	{
    	    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    	    if (status & 1)
	    {
	 	remote_id = (*rtn) (env_name);
	    	if(remote_id == -1)
	    	{
		    perror("ConnectToMds");
	    	    remote_id = 0;
		}
	    }
	    else printf(MdsGetMsg(status));
	}
    }
}
	    
static int eventAstRemote(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventAst"};
    int status;
    int (*rtn)();

    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
	status = (*rtn) (remote_id, eventnam, astadr, astprm, eventid);
    if(!(status & 1)) printf(MdsGetMsg(status));
    return status;
}


int MDSEventAst(char *eventnam, void (*astadr)(), void *astprm, int *eventid)
{
    int status;
    int i, j;    
    int name_already_in_use;
    
    if(!eventnam || !*eventnam) return 1;

    initializeLocalRemote();
    if(remote_id)
	return eventAstRemote(eventnam, astadr, astprm, eventid);


    /* First check wether the same name is already in use */
    for(i = 0; i < MAX_EVENTNAMES; i++)
	if(private_info[i].active && !strcmp(eventnam, private_info[i].name))
	    break;
    if(i < MAX_EVENTNAMES)
	name_already_in_use = 1;
    else
	name_already_in_use = 0;


    /* define internal event dispatching structure */ 
    for(i = 0; i < MAX_EVENTNAMES; i++)
	//if(private_info[i].active && !strcmp(eventnam, private_info[i].name))
	if(private_info[i].active && !strcmp(eventnam, private_info[i].name) &&
		private_info[i].astadr == astadr && private_info[i].astprm == astprm)
	     break;
    if(i == MAX_EVENTNAMES) /* if no previous MdsEventAst to that event made by the process */
    {
	for(i = 0; i < MAX_EVENTNAMES && private_info[i].active; i++);
	if(i == MAX_EVENTNAMES) /* if no free private event slot found */
	    return 0;
	strncpy(private_info[i].name, eventnam, MAX_EVENTNAME_LEN - 1);
	private_info[i].active = 1;
    }
    private_info[i].astadr = astadr;
    private_info[i].astprm = (void *)(void *)astprm;


    *eventid = i; 

    if(name_already_in_use)
	return 1;

    setHandle();

    /* lock shared memory region */ 
    getLock();

    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	    return 0;
	}
    }
/* Find first free SharedEventInfo slot */
    for(i = 0; i < MAX_ACTIVE_EVENTS && shared_info[i].nameid != -1; i++);
    if(i == MAX_ACTIVE_EVENTS) /* If no free SharedEventInfo slot foud */
    {
	releaseLock();
	return 0;
    }
    shared_info[i].msgkey = msgKey;
/* Check wether the same name previously used */
    for(j = 0; j < MAX_EVENTNAMES; j++)
	if(shared_name[j].refcount > 0 && !strcmp(shared_name[j].name, eventnam))
	    break;
    if(j == MAX_EVENTNAMES) /* if the name is not yet active */
    {
	for(j = 0; j < MAX_EVENTNAMES && shared_name[j].refcount > 0; j++);
	if(j == MAX_EVENTNAMES) /* If event names slot full */
	{
	    releaseLock();
	    return 0;
	}
	strncpy(shared_name[j].name, eventnam, MAX_EVENTNAME_LEN - 1);
    }
    shared_name[j].refcount++;
    shared_info[i].nameid = j;

    releaseLock();

    attachExitHandler(cleanup);
    return 1;
}

static int canEventRemote(int eventid)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 11, "MdsEventCan"};
    int status;
    int (*rtn)();

    status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
	  status = (*rtn) (remote_id, eventid);
    return status;
}

	
int MDSEventCan(int eventid)
{
    int i, j, k;
    struct PrivateEventInfo *evinfo;

    initializeLocalRemote();
    if(remote_id)
	return canEventRemote(eventid);

    evinfo = &private_info[eventid];
 
    if(!shared_info) return 0; /*must follow MdsEventAst */

    getLock();

/* deactivate corresponding SharedEventInfo slot */
    for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
    {
	if(shared_info[i].msgkey == msgKey && shared_info[i].nameid >= 0 
		&& !strncmp(evinfo->name, shared_name[shared_info[i].nameid].name))
	    break;
    }
    if(i < MAX_ACTIVE_EVENTS) /* if corresponding slot found */
    {
	shared_name[shared_info[i].nameid].refcount--;
	shared_info[i].nameid = -1;
    }

    releaseLock();
    evinfo->active = 0;
}



static int sendRemoteEvent(char *evname, int data_len, char *data)
{
    struct descriptor 
	library_d = {DTYPE_T, CLASS_S, 8, "MdsIpShr"},
	routine_d = {DTYPE_T, CLASS_S, 8, "MdsValue"};
    char expression[128];
    int status;
    int (*rtn)();
    EMPTYXD(ansarg);
    struct descrip dsc;

    dsc.dtype = DTYPE_T;
    dsc.ptr = data;
    dsc.ndims = 0;
    dsc.length = data_len;

    sprintf(expression, "setevent(\"%s\", $)", evname);
   status = LibFindImageSymbol(&library_d, &routine_d, &rtn);
    if (status & 1)
	  status = (*rtn) (remote_id, expression, &dsc, &ansarg, NULL);
    return status;
}

int MDSEvent(char *evname, int data_len, char *data)
{
    int i, j, name_idx;
    
    initializeLocalRemote();
    if(remote_id)
	return sendRemoteEvent(evname, data_len, data);


    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	    return 0;
	}
    }

    /* Search event name in the event name list */
    for(name_idx = 0; name_idx < MAX_EVENTNAMES && (shared_name[name_idx].refcount == 0 
	|| strcmp(shared_name[name_idx].name, evname)); name_idx++);
    if(name_idx < MAX_EVENTNAMES)
    {
    	for(i = 0; i < MAX_ACTIVE_EVENTS; i++)
		//if(shared_info[i].nameid >= 0 && !strcmp(evname, shared_name[shared_info[i].nameid].name))
	    if(shared_info[i].nameid == name_idx)
	    	sendMessage(evname, shared_info[i].msgkey, data_len, data);
    }

    releaseLock();
}
		    

/** For debugging only **/
void DumpSharedEventInfo()
{
    int i, j;

    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	}
    }
		    		
    for(i = 0; i < MAX_EVENTNAMES; i++)
    {
	if(shared_name[i].refcount > 0)
	{
	    printf("\n%s\t", shared_name[i].name);
	    for(j = 0; j < MAX_ACTIVE_EVENTS; j++)
	    {
	    	if(shared_info[j].nameid == i)
		    printf("%d  ", shared_info[j].msgkey);
	    }
	}
    }
    releaseLock();
    printf("\n");
}

void InitializeSharedInfo()
{

    getLock();
    if(!shared_info)
    {
	if(attachSharedInfo() == -1)
	{
	    releaseLock();
	}
    }
    initializeShared();
}
		    		
void RemoveMessages()
{
    int i, msgid, status;
    struct msqid_ds buf;

    for(i = 1; i < 1000; i++)
    {
   	msgid = msgget(i, 0777);
    	if(msgid != -1)
	{
    	    status = msgctl(msgid, IPC_RMID, &buf);
	    printf("\nArato queue %d status %d", i, status);
	    if(status == -1) perror("Error in msgctl");
	}
    }
}
	
#endif
#endif



