#include <stdio.h>
#include <ipcs/config.h>
#include <ipcs/msgs.h>
#include <ipcs/links.h>
#include <ipcs/when.h>
#include <ipcs/mailclass.h>
#include <ipcs/events.h>
#include <ipcs/msgerror.h>
#include <mdsevents.h>
/*
This module will setup an AST that is executed whenever a 
named MDS event occurs.
*/

static unsigned short eventtag = 0;
static idptr head,tail;

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifdef HAVE_STDARG_H
mdseventast(name,astadr)
char *name;
void (*astadr)();
#else
mdseventast(va_alist)
va_dcl
#endif

{

    va_list incrm;
    void *astprm;
    idptr id,*eventid;
    int len;
    int i;

#ifdef HAVE_STDARG_H
    va_start(incrm,astadr);
    len = 0;
    while (va_arg(incrm, void *) != 0)++len;
    va_end(incrm);
    va_start(incrm,astadr);
    if(len > 2){
        astprm = va_arg(incrm,void*);
#ifndef __alpha
        astprm = va_arg(incrm,void*);
#endif
        if(len > 3){
            eventid = (idptr *)va_arg(incrm,idptr);
        } else {
            eventid = NULL;
        }
    } else {
        astprm = NULL;
    }
    va_end(incrm);
#else
    char *name;
    void (*astadr)();
    va_count(len);
    va_start(incrm);
    name = va_arg(incrm,char*);
#ifndef __alpha
    name = va_arg(incrm,char*);
#endif
    astadr = va_arg(incrm,(void*)());
    if(len > 2){
        astprm = va_arg(incrm,void*);
        if(len > 3){
            eventid = (idptr *)va_arg(incrm,idptr);
        } else {
            eventid = NULL;
        }
    } else {
        astprm = NULL;
    }
    va_end(incrm);
#endif

    if(name == NULL || astadr == NULL)return(error_bad_arg_address);

    id = create_mdsevent_id();
    id->astadr = astadr;
    id->astprm = astprm;
    if(eventid) *eventid = id;

    return(error_success);
}

idptr  create_mdsevent_id(name)
char *name;
{
    idptr id;
    int i;
    id = (idptr)calloc(1,sizeof(struct _id));
    id->name = (char *)calloc(1,strlen(name) + 1);
    for(i = 0; i < strlen(name); ++i){
        if(islower(name[i]))id->name[i] = toupper(name[i]);
        else id->name[i] = name[i];
    }
    id->tag = eventtag++;
    add_link(id,&head,&tail);
}

idptr find_mdsevent_idtag(tag)
unsigned short tag;
{
    idptr id;
    char *uname;
    int i;

    for(id = head; id != NULL; id = id->link.next){
        if(id->tag == tag){
            return(id);
        }
    }
    return(NULL);
}

idptr find_mdsevent_id(name,sid)
char *name;
idptr sid;  /*  starting id - used to allow multiple asts for one event */
{
    idptr id;
    char *uname;
    int i;

    if(name == NULL)return(NULL);
    uname = (char *)calloc(1,strlen(name) + 1);
    for(i = 0; i < strlen(name); ++i){
        if(islower(name[i]))uname[i] = toupper(name[i]);
        else uname[i] = name[i];
    }
    if(sid == NULL) id = head;
    else id = sid->link.next;
    for(; id != NULL; id = id->link.next){
        if(strcmp(uname,id->name) == 0){
            free(uname);
            return(id);
        }
    }
    free(uname);
    return(NULL);
}


mdseventcan(eventid)
idptr eventid;
{

    if(eventid == NULL)return(error_bad_arg_address);
    if(find_mdsevent_id(eventid->name,NULL) == NULL) /*  not queued  */
        return(error_bad_arg_address);
    rem_link(eventid,&head,&tail);
    free(eventid->name);
    free(eventid);
}

void mdseventdone(id)
idptr id;
{
    if(id && id->astadr)(*id->astadr)(id->astprm);
    return;
}


/*
a future improvement, or maybe just something to look out for,
would be to make sure the ipcs msg queue are empty of msgs
relating to mdsevents
*/

/*
still need to inform the server that we don't want it somehow
*/
void mdsdeq_all()
{

    idptr id;
    for(id = head; id != NULL; id = id->link.next){
        mdseventcan(id);
    }
    return;

}

void mdsdeq_event(name)
char *name;
{
    idptr id;
    while((id = find_mdsevent_id(name,NULL)) != NULL){
        rem_link(id,&head,&tail);
        free(id->name);
        free(id);
    }
    return;
}
