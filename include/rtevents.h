extern void *MdsEventAddListener(char *name,  void (*callback)(char *, char *, int));
extern int MdsEventTrigger(char *name, char *buf, int size);
extern int MdsEventTriggerAndWait(char *name, char *buf, int size);
extern int MdsEventTriggerAndWait(char *name, char *buf, int size);
extern void MdsEventWait(char *name, char *buf, int size, int *retSize);