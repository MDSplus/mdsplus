#include <stdio.h>
#include <stdlib.h>
#include <ipcs/config.h>
#include <ipcs/msgs.h>
#include <ipcs/links.h>
#include <ipcs/when.h>
#include <ipcs/mailclass.h>
#include <ipcs/events.h>
#include <ipcs/msgerror.h>
#include <mdsevents.h>


static int debug = 1;
static short timeouttag;

mdsq_event(event)
char *event;
{
    short class[class_size];
    char line[4097];
    int status;
    int hiddennoop();


    idptr id;
    id = create_mdsevent_id(event);
    memset(class,0L,sizeof(class));
    class[major_class] = mc_events;
    class[minor_class] = mc_create;
    class[private_tag] = id->tag;

    sprintf(line,"%s\n0\n0\n",event);
    ipcs_sendmsg(MDSEVENTD,class,line,strlen(line));
    id->timertag = reg_hidden_event(hiddennoop,0,0,0);
    hinvl(1000,id->timertag);
    status = mds_handle_event_msg(id);
		id->status = 0;
    if((status %2) == 1){
        memset(class,0L,sizeof(class));
        class[major_class] = mc_events;
        class[minor_class] = mc_wait;
        class[private_tag] = id->tag;
        ipcs_sendmsg(MDSEVENTD,class,line,strlen(line));
    }




}

int _mds_handle_newmsg(msg)
MSGPTR msg;
{
    short class[class_size];
    idptr id;
    int status;
    memcpy(class,msgclass(msg),class_size * sizeof(short));
    id = find_mdsevent_idtag(class[private_tag]);
    switch(class[minor_class]){
    case mc_create:
        if(id){
            if(id->timertag) cancel_hidden_event(id->timertag);
            if(id->msgtag) cancel_hidden_event(id->msgtag);
            id->status = error_success;
        }
        if(debug)fprintf(stderr,"mdsevents: created good status\n");
        ipcs_deletemsg(msg);
        mdseventdone(id);

        break;
    case mc_wait:
        status = (int) class[standard_status];
        if(debug)fprintf(stderr,"mdsevents: wait replyed with status %d\n",status);
        if(id){
            id->status = status;
            if(id->timertag) cancel_hidden_event(id->timertag);
            if(id->msgtag) cancel_hidden_event(id->msgtag);
        }
        if(id && id->data && msgsize(msg) > 0){
            if(id->dlen > msgsize(msg)) id->dlen = msgsize(msg);
            memcpy(id->data,msgdata(msg),msgsize(msg));
        }
        mdseventdone(id);
        ipcs_deletemsg(msg);
        break;
    }
}

int _mds_handle_returnmsg(msg)
MSGPTR msg;
{
    short class[class_size];
    idptr id;
    int status;
    memcpy(class,msgclass(msg),class_size * sizeof(short));
    id = find_mdsevent_idtag(class[private_tag]);
    switch(class[minor_class]){
    case mc_create:
        if((msgstatus(msg) % 2) == 1){
            status = (int) class[standard_status];
            switch((short)status){
            case (short)error_duplicate:
								status = error_success;
                if(debug)fprintf(stderr,"mdsevents: already there -  good status\n");
                break;
            default:
                if(debug)fprintf(stderr,"mdsevents: create returned bad application status = %d\n",status);
                break;
            }

        } else{
            if(debug)fprintf(stderr,"mdsevents: create returned bad ipcs status\n");
            status = msgstatus(msg);
        }
        if(id){
            if(id->timertag) cancel_hidden_event(id->timertag);
            if(id->msgtag) cancel_hidden_event(id->msgtag);
            id->status = status;
            mdseventdone(id);
        }
        ipcs_deletemsg(msg);
        break;
    case mc_wait:
        if((msgstatus(msg) % 2) == 1){
            status = (int) class[standard_status];
            id = find_mdsevent_idtag(class[private_tag]);
            if(id) id->status = status;
            if(debug)fprintf(stderr,"mdsevents: wait returned status %d\n",status);
        } else{
            if(debug)fprintf(stderr,"mdsevents: wait returned bad ipcs status\n");
            id = find_mdsevent_idtag(class[private_tag]);
            status = msgstatus(msg);
            if(id) id->status = status;
        }
        mdseventdone(id);
        ipcs_deletemsg(msg);

        break;
    default:
        ipcs_deletemsg(msg);
        break;
    }
    return(status);
}



int mds_handle_event_msg(wid)
idptr wid;  /*  id required to return  */
{

    struct event ev;
    int status;
    short class[class_size];
    MSGPTR msg;
    struct hidevent h;
    idptr id;


    status = 0;
    if(wid) wid->status = 0;

    while(1){
        libwhen(hidden_event | new_msg | return_msg,&ev,mc_events,&h);
        switch(ev.event_type){
        case new_msg:
            msg = (MSGPTR)ev.parameter;
            status = _mds_handle_newmsg(msg);
            if(wid && wid->status != 0)return(status);
            break;
        case return_msg:
            msg = (MSGPTR)ev.parameter;
            status = _mds_handle_returnmsg(msg);
            if(wid && wid->status != 0)return(status);
            break;
        case hidden_event:
            if(h.tag == timeouttag){
                if(debug)fprintf(stderr,"mdsevents: create timed out\n");
                status = error_timeout;
                timeouttag = 0;
                return(status);
            }
            break;
        }

    }
    return(status);


}

hiddennoop(ev,pram)
struct event *ev;
void *pram;
{

    return;

}
