/*  CMS REPLACEMENT HISTORY, Element SERVER$PARSE_IDENT.C */
/*  *6    25-OCT-1994 16:24:55 TWF "no more vaxc" */
/*  *5    12-JAN-1994 16:41:00 TWF "Fix it" */
/*  *4    12-JAN-1994 16:28:13 TWF "Make it reentrant" */
/*  *3    25-FEB-1993 14:52:34 TWF "Use DECC" */
/*  *2    22-APR-1992 11:00:12 TWF "Return better status" */
/*  *1    17-APR-1992 10:50:33 TWF "Parse server identifier" */
/*  CMS REPLACEMENT HISTORY, Element SERVER$PARSE_IDENT.C */
/*------------------------------------------------------------------------------

		Name:   SERVER$PARSE_IDENT   

		Type:   C function

     		Author:	TOM FREDIAN

		Date:   17-APR-1992

    		Purpose: Parse Server Identifier 

------------------------------------------------------------------------------

	Call sequence: 

int SERVER$PARSE_IDENT( struct dsc$descriptor *server, struct dsc$descriptor *node, struct dsc$descriptor *object )

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

 	Description:


------------------------------------------------------------------------------*/

#include <descrip.h>
#include <lnmdef.h>
#include <starlet.h>
#include <str$routines.h>

#pragma extern_model save
#pragma extern_model globalvalue
extern SS$_NOSUCHNODE;
#pragma extern_model restore

int SERVER$PARSE_IDENT( struct dsc$descriptor *server, struct dsc$descriptor *node, struct dsc$descriptor *object )
{ 
  char translation[64];
  struct dsc$descriptor id = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
  struct {  unsigned short length;
            unsigned short code;
            char           *buffer;
            unsigned short *outlen;
         } itmlst[] = {{sizeof(translation),LNM$_STRING,0,0},{0,0,0,0}};
  static $DESCRIPTOR(lnm$file_dev,"LNM$FILE_DEV");
  static $DESCRIPTOR(colon,":");
  int status;
  itmlst[0].buffer = id.dsc$a_pointer = translation;
  itmlst[0].outlen = &id.dsc$w_length;
  id.dsc$w_length = sizeof(translation);
  str$trim(&id,server,&id.dsc$w_length);
  while (sys$trnlnm(&LNM$M_CASE_BLIND,&lnm$file_dev,&id,0,itmlst) & 1);
  status = str$element(node,&0,&colon,&id) & 1 ? 1 : SS$_NOSUCHNODE;
  status = (status & 1) ? (str$element(object,&2,&colon,&id) & 1 ? 1 : SS$_NOSUCHNODE) : status;
  if (status & 1)
  {
    str$upcase(node,node);
    str$upcase(object,object);
  }
  return status;
}
