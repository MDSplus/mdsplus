#include <stdio.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <ipdesc.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <string.h>
#include "SystemSpecific.h"
extern char *TranslateLogical(char *);

extern void EventWait(char *name, char *buf, int size, int *retSize);
extern void EventReset();
extern void * EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, void *, char, int, char *, int), void *callbackArg);
extern void * EventAddListener(char *name,  void (*callback)(char *, char *, int, void *, char, int, char*, int), void *callbackArg);
extern void EventRemoveListener(void *eventHandler);
extern int EventTrigger(char *name, char *buf, int size);
extern int EventTriggerAndWait(char *name, char *buf, int size);
extern int EventTriggerAndTimedWait(char *name, char *buf, int size, int millisecs);
extern void EventClean();

extern char *EventGetSerializedState(int *size);
extern char *EventGetSerializedMemState(int *size);




////Structures used to record for each event the list of IP addresses for that event
#define MAX_IP 256
#define MAX_LINE 1024
struct EventDescr 
{
	char *eventName;
	int numIp;
	SOCKET ids[MAX_IP];
	struct EventDescr *nxt;
};

//mdseventip.host format:
//<event name> : <ip address> [, <ip address>]*

//Event descriptor head
static struct EventDescr *eventDescrHead = 0;


static void configure()
{
	char *confFilename;
	char line[MAX_LINE];
	FILE *confF;
	char *tok;
	char currAddr[512];
	char *addrPtr, *addrPtr1;
	struct EventDescr *currEventDescr;
	int i;

	static int initialized = 0;

	if(initialized) return;
	initialized = 1;

	confFilename = TranslateLogical("mdsevent_conf");
	confF = fopen(confFilename, "r");
	if(!confF) return;


	while(1)
	{
		if(!fgets(line, MAX_LINE, confF)) break;
		tok = strtok(line, ":");
		if(!tok) continue;
		currEventDescr = (struct EventDescr *)malloc(sizeof(struct EventDescr));
		currEventDescr->eventName = malloc(strlen(tok) + 1);
		strcpy(currEventDescr->eventName, tok);
		currEventDescr->numIp = 0;
		for(i = 0; i < MAX_IP; i++)
		{
			tok = strtok(NULL, ",");
			if(!tok) break;
			strcpy(currAddr, tok);
			for(addrPtr = currAddr;  *addrPtr == ' '; addrPtr++);
			if(!addrPtr) break;
			for(addrPtr1 = addrPtr; *addrPtr1 && *addrPtr1 != ' '; addrPtr1++);
			*addrPtr1 = 0;
			currEventDescr->ids[currEventDescr->numIp] = ConnectToMds(addrPtr);
			if(currEventDescr->ids[currEventDescr->numIp]) 
				currEventDescr->numIp++;
		}
		currEventDescr->nxt = eventDescrHead;
		eventDescrHead = currEventDescr;
	}
	fclose(confF);
}



static void propagateEvent(char *name, char *buf, int size, char wait)
{
	struct EventDescr *currEventDescr;
	int i;
	char *expr;
	struct descriptor candidateD = {0, DTYPE_T, CLASS_S, 0};
	struct descriptor patternD = {0, DTYPE_T, CLASS_S, 0};
	struct descrip ansarg;
	struct descrip bufD;

	configure();

	bufD.dtype = DTYPE_BU;
	bufD.ndims = 1;
	bufD.dims[0] = size;
	bufD.length = 1;
	bufD.ptr = buf;

	expr = malloc(strlen(name) + 512);
	if(wait)
		sprintf(expr, "RtEventsShr->EventTriggerAndWait(\"%s\", $1, val(%d))", name, size);
	else
		sprintf(expr, "RtEventsShr->EventTrigger(\"%s\", $1, val(%d))", name, size);

	candidateD.length = strlen(name);
	candidateD.pointer = name;
	for(currEventDescr = eventDescrHead; currEventDescr; currEventDescr = currEventDescr->nxt)
	{
		patternD.length = strlen(currEventDescr->eventName);
		patternD.pointer = currEventDescr->eventName;
		if(!StrMatchWild(&candidateD, &patternD))
			continue;
		for(i = 0; i < currEventDescr->numIp; i++)
		{
			memset(&ansarg, 0, sizeof(ansarg));
			MdsValue(currEventDescr->ids[i], expr, &bufD, &ansarg, NULL);
		}
	}
	free(expr);
}



EXPORT void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int, void *), void *callbackArg)
{
	return EventAddListener(name,  (void (*)(char *, char *, int, void *, char, int retSize, char *retData, int type))callback, callbackArg);
}

EXPORT int MdsEventTrigger(char *name, char *buf, int size)
{
	propagateEvent(name, buf, size, 0);
	return (EventTrigger(name, buf, size)==0)?1:0;
}

EXPORT int MdsEventTriggerAndWait(char *name, char *buf, int size)
{
	propagateEvent(name, buf, size, 1);
	return (EventTriggerAndWait(name, buf, size)==0)?1:0;
}

EXPORT extern void MdsEventWait(char *name, char *buf, int size, int *retSize)
{
	EventWait(name, buf, size, retSize);
}


EXPORT extern void MdsEventClean()
{
	EventClean();
}


EXPORT struct descriptor_xd *MdsEventGetState()
{
	static EMPTYXD(xd);
	DESCRIPTOR_A(stateD, 1, DTYPE_B, 0, 0); 
	char *state;
	int size;

	state = EventGetSerializedState(&size);
	stateD.pointer = state;
	stateD.arsize = size;

	MdsCopyDxXd((struct descriptor *)&stateD, &xd);
	free(state);
	return &xd;
}

EXPORT struct descriptor_xd *MdsEventGetMemState()
{
	static EMPTYXD(xd);
	DESCRIPTOR_A(stateD, 1, DTYPE_B, 0, 0); 
	char *state;
	int size;

	state = EventGetSerializedMemState(&size);
	stateD.pointer = state;
	stateD.arsize = size;

	MdsCopyDxXd((struct descriptor *)&stateD, &xd);
	free(state);
	return &xd;
}

