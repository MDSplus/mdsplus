/*------------------------------------------------------------------------------

		Name:   ServerSendMessage

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Send message to server.

------------------------------------------------------------------------------

	Call sequence: 

int ServerSendMessage();

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <mdsdescrip.h>
#include <mdsserver.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <ipcs/msgs.h>
#include <ipcs/ipcspdata.h>
#include <ipcs/mailclass.h>

static void (*LinkDown)() = 0;
static int Incarnation = 0;
static int TransactionId = 0;

/*
static void NodeLost(int *nid)
{
  if (LinkDown)
    (*LinkDown)(nid);
  return;
}

static void ProcessResponse(int *nid,int *length,Msg *reply)
{
  if (reply->incarnation != Incarnation) return;
  if (reply->retstatus)
    *reply->retstatus = reply->status;
  if (reply->ast)
    (*reply->ast)(reply->astparam,nid, length, reply);
  if (reply->efn)
    sys$setef(reply->efn);
  return;
}
*/

int ServerSetLinkDownHandler(void (*handler)())
{
  LinkDown = handler;
  return 1;
}



int ServerSendMessage( int sendast, char *server, MsgType type, int length, char *msg, int *retstatus, 
                         void (*ast)(), void *astparam, void (*before_ast)(), int *netid_return)
{
    IPCS_PDATA_PTR  pdata_ptr,  ipcs_pdata_ptr();
    int mlen = sizeof(Msg) + length - 1;
    Msg *m = malloc(mlen);
    static int time[2] = {0,0};
    static short class[8] = {
        mc_mdsserver,1,2,3,4,5,6,7      };
    m->opcode = type;
    m->status = 0;
	m->now = sendast;
    if (length)
      memcpy(m->data,msg,length);
    pdata_ptr = ipcs_pdata_ptr();
    if (pdata_ptr->common_write_base == 0)
      ipcsinit(0);
	return ipcs_sendmsg(server,class,(unsigned char *)m,mlen);
}


int ServerSendReply(void *lid,int length,unsigned char *reply)
{
	return ipcs_replymsg((MSGPTR)lid,0,reply,length);
}
