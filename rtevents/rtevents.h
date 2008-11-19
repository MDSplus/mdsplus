#ifndef RTEVENTS_H_
#define RTEVENTS_H_
#ifdef __cplusplus
#define EXTERN 
extern "C" {
#else
#define EXTERN extern
#endif
EXTERN void *EventAddListener(char *name,  void (*callback)(char *, char *, int, bool));
EXTERN void *EventAddListenerGlobal(char *name,  void (*callback)(char *, char *, int, bool));
EXTERN void EventRemoveListener(void *eventHandler);
EXTERN int EventTrigger(char *name, char *buf, int size);
EXTERN int EventTriggerAndWait(char *name, char *buf, int size);
EXTERN void EventClean();
EXTERN void EventReset();
	
#ifdef __cplusplus
}
#endif
#endif /*RTEVENTS_H_*/
