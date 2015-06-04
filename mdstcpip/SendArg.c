#include "mdsip_connections.h"
#include <stdlib.h>
#include <string.h>


///
/// This function can be used to send data form a mds descriptor. It was designed
/// to pass descriptors arguments of a TDI command. It accepts the data descriptor
/// info values as input arguments, then a Message is compiled with these informations
/// and the proper connection message id. 
/// The connection message_id is read from idx field and written inside message header 
/// It has to be incremented only if this is the first command argument passed.. 
/// this aims to tag with the same message_id all the arguments that belongs to 
/// the same command.
/// The idx field can also tag for special io commands (MDS_IO_xxx) identified by 
/// checking (idx > nargs) in \ref ProcessMessage() function. This is used to add
/// distribute client access to parse files.
/// The function SendMdsMsg() is used to format message memory and actually send 
/// it through the connection.
/// 
///
/// \param id the id of the instanced connection in the connections list
/// \param idx the id of the descriptor argument in a evaluated expression
/// \param dtype the descriptor type code
/// \param nargs number of descriptor arguments
/// \param length data type length of a single element in descriptor.
/// \param ndims number of dimensions of the descriptor data
/// \param dims dimensions of descriptor data
/// \param bytes pointer to descriprot memory
/// \return the exit status of SendMdsMsg() that is true if the message was succesfully
/// sent or false otherwise.
/// 

int SendArg(int id, unsigned char idx, char dtype, unsigned char nargs, short length, char ndims,
	    int *dims, char *bytes)
{
  int status;
  int msglen;
  int i;
  int nbytes = length;
  Message *m;
  
  // MESSAGE ID //
  // * if this is the first argument sent, increments connection message id   //
  // * get the connection message_id and store it inside message              //
  
  int msgid = (idx == 0 || nargs == 0) ?
              IncrementConnectionMessageId(id) : GetConnectionMessageId(id);
  if (msgid < 1)
    return 0;
  
  if (idx > nargs) {
    /**** Special I/O message ****/
    nbytes = dims[0];
  } else {
    for (i = 0; i < ndims; i++)
      nbytes *= dims[i];
  }
  
  msglen = sizeof(MsgHdr) + nbytes;
  m = memset(malloc(msglen), 0, msglen);
  m->h.client_type = 0;
  m->h.msglen = msglen;
  m->h.message_id = msgid;
  m->h.descriptor_idx = idx;
  m->h.dtype = dtype;
  m->h.nargs = nargs;
  m->h.length = length;
  m->h.ndims = ndims;
#ifdef __CRAY
  for (i = 0; i < 4; i++)
    m->h.dims[i] =
	((ndims > i * 2) ? (dims[i * 2] << 32) : 0) | ((ndims >
							(i * 2 + 1)) ? (dims[i * 2 + 1]) : 0);
#else
  for (i = 0; i < MAX_DIMS; i++)
    m->h.dims[i] = i < ndims ? dims[i] : 0;
#endif
  memcpy(m->bytes, bytes, nbytes);
  status = SendMdsMsg(id, m, 0);
  free(m);
  return status;
}
