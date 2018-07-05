/*****************************************************************************

Copyright (c) 2010 - 2012, Tom Zhou at zs68j2ee@gmail.com.

All rights reserved.



Redistribution and use in source and binary forms, with or without

modification, are permitted provided that the following conditions are

met:



* Redistributions of source code must retain the above

  copyright notice, this list of conditions and the

  following disclaimer.



* Redistributions in binary form must reproduce the

  above copyright notice, this list of conditions

  and the following disclaimer in the documentation

  and/or other materials provided with the distribution.



* Neither the name of the creator nor the names of its contributors may be used to

  endorse or promote products derived from this

  software without specific prior written permission.



THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS

IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,

THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR

PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR

CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,

EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,

PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR

PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF

LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING

NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS

SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/



/*****************************************************************************

written by

   Tom Zhou, last updated 05/18/2012

*****************************************************************************/



#ifndef UDTC_H_

#define UDTC_H_



#ifndef WIN32

   #include <sys/types.h>

   #include <sys/socket.h>

   #include <netinet/in.h>

#else

   #if defined __MINGW__ || defined __MINGW64__

      #include <stdint.h>

      #include <ws2tcpip.h>

   #endif

   #include <windows.h>

#endif

#include <string.h>



#ifdef WIN32

   #if !defined __MINGW__ && !defined __MINGW64__

      // Explicitly define 32-bit and 64-bit numbers

      typedef __int32 int32_t;

      typedef __int64 int64_t;

      typedef unsigned __int32 uint32_t;

      #ifndef LEGACY_WIN32

         typedef unsigned __int64 uint64_t;

      #else

         // VC 6.0 does not support unsigned __int64: may cause potential problems.

         typedef __int64 uint64_t;

      #endif



      #ifdef UDT_EXPORTS

         #define UDT_API __declspec(dllexport)

      #else

         #define UDT_API __declspec(dllimport)

      #endif

   #else

      #define UDT_API

   #endif

#else

   #define UDT_API __attribute__ ((visibility("default")))

#endif



#define NO_BUSY_WAITING



#ifdef WIN32

   #if !defined __MINGW__ && !defined __MINGW64__

      typedef SOCKET SYSSOCKET;

   #else

      typedef int SYSSOCKET;

   #endif

#else

   typedef int SYSSOCKET;

#endif



typedef SYSSOCKET UDPSOCKET;

typedef int UDTSOCKET;



#ifdef __cplusplus

extern "C" {

#endif



// UDT status

enum UDT_UDTSTATUS {

	UDT_INIT = 1,

	UDT_OPENED,

	UDT_LISTENING,

	UDT_CONNECTING,

	UDT_CONNECTED,

	UDT_BROKEN,

	UDT_CLOSING,

	UDT_CLOSED,

	UDT_NONEXIST

};



// UDT option

enum UDT_UDTOpt {

	UDT_UDT_MSS,             // the Maximum Transfer Unit

	UDT_UDT_SNDSYN,          // if sending is blocking

	UDT_UDT_RCVSYN,          // if receiving is blocking

	UDT_UDT_CC,              // custom congestion control algorithm

	UDT_UDT_FC,              // Flight flag size (window size)

	UDT_UDT_SNDBUF,          // maximum buffer in sending queue

	UDT_UDT_RCVBUF,          // UDT receiving buffer size

	UDT_UDT_LINGER,          // waiting for unsent data when closing

	UDT_UDP_SNDBUF,          // UDP sending buffer size

	UDT_UDP_RCVBUF,          // UDP receiving buffer size

	UDT_UDT_MAXMSG,          // maximum datagram message size

	UDT_UDT_MSGTTL,          // time-to-live of a datagram message

	UDT_UDT_RENDEZVOUS,      // rendezvous connection mode

	UDT_UDT_SNDTIMEO,        // send() timeout

	UDT_UDT_RCVTIMEO,        // recv() timeout

	UDT_UDT_REUSEADDR,       // reuse an existing port or create a new one

	UDT_UDT_MAXBW,           // maximum bandwidth (bytes per second) that the connection can use

	UDT_UDT_STATE,           // current socket state, see UDTSTATUS, read only

	UDT_UDT_EVENT,           // current available events associated with the socket

	UDT_UDT_SNDDATA,         // size of data in the sending buffer

	UDT_UDT_RCVDATA          // size of data available for recv

};



// UDT error code

enum UDT_ERRNO {

    UDT_SUCCESS = 0,

    UDT_ECONNSETUP = 1000,

    UDT_ENOSERVER = 1001,

    UDT_ECONNREJ = 1002,

    UDT_ESOCKFAIL = 1003,

    UDT_ESECFAIL = 1004,

    UDT_ECONNFAIL = 2000,

    UDT_ECONNLOST = 2001,

    UDT_ENOCONN = 2002,

    UDT_ERESOURCE = 3000,

    UDT_ETHREAD = 3001,

    UDT_ENOBUF = 3002,

    UDT_EFILE = 4000,

    UDT_EINVRDOFF = 4001,

    UDT_ERDPERM = 4002,

    UDT_EINVWROFF = 4003,

    UDT_EWRPERM = 4004,

    UDT_EINVOP = 5000,

    UDT_EBOUNDSOCK = 5001,

    UDT_ECONNSOCK = 5002,

    UDT_EINVPARAM = 5003,

    UDT_EINVSOCK = 5004,

    UDT_EUNBOUNDSOCK = 5005,

    UDT_ENOLISTEN = 5006,

    UDT_ERDVNOSERV = 5007,

    UDT_ERDVUNBOUND = 5008,

    UDT_ESTREAMILL = 5009,

    UDT_EDGRAMILL = 5010,

    UDT_EDUPLISTEN = 5011,

    UDT_ELARGEMSG = 5012,

    UDT_EINVPOLLID = 5013,

    UDT_EASYNCFAIL = 6000,

    UDT_EASYNCSND = 6001,

    UDT_EASYNCRCV = 6002,

    UDT_ETIMEOUT = 6003,

    UDT_EPEERERR = 7000,

    UDT_EUNKNOWN = -1

};



// UDT trace information

typedef struct UDT_CPerfMon_ {

	// global measurements

	int64_t msTimeStamp;                 // time since the UDT entity is started, in milliseconds

	int64_t pktSentTotal;                // total number of sent data packets, including retransmissions

	int64_t pktRecvTotal;                // total number of received packets

	int pktSndLossTotal;                 // total number of lost packets (sender side)

	int pktRcvLossTotal;                 // total number of lost packets (receiver side)

	int pktRetransTotal;                 // total number of retransmitted packets

	int pktSentACKTotal;                 // total number of sent ACK packets

	int pktRecvACKTotal;                 // total number of received ACK packets

	int pktSentNAKTotal;                 // total number of sent NAK packets

	int pktRecvNAKTotal;                 // total number of received NAK packets

	int64_t usSndDurationTotal;		     // total time duration when UDT is sending data (idle time exclusive)



	// local measurements

	int64_t pktSent;                     // number of sent data packets, including retransmissions

	int64_t pktRecv;                     // number of received packets

	int pktSndLoss;                      // number of lost packets (sender side)

	int pktRcvLoss;                      // number of lost packets (receiver side)

	int pktRetrans;                      // number of retransmitted packets

	int pktSentACK;                      // number of sent ACK packets

	int pktRecvACK;                      // number of received ACK packets

	int pktSentNAK;                      // number of sent NAK packets

	int pktRecvNAK;                      // number of received NAK packets

	double mbpsSendRate;                 // sending rate in Mb/s

	double mbpsRecvRate;                 // receiving rate in Mb/s

	int64_t usSndDuration;               // busy sending time (i.e., idle time exclusive)



	// instant measurements

	double usPktSndPeriod;               // packet sending period, in microseconds

	int pktFlowWindow;                   // flow window size, in number of packets

	int pktCongestionWindow;             // congestion window size, in number of packets

	int pktFlightSize;                   // number of packets on flight

	double msRTT;                        // RTT, in milliseconds

	double mbpsBandwidth;                // estimated bandwidth, in Mb/s

	int byteAvailSndBuf;                 // available UDT sender buffer size

	int byteAvailRcvBuf;                 // available UDT receiver buffer size

} UDT_TRACEINFO;



UDT_API extern const UDTSOCKET UDT_INVALID_SOCK;

#undef ERROR

UDT_API extern const int UDT_ERROR;



// library initialization

UDT_API extern int udt_startup();

UDT_API extern int udt_cleanup();



// socket operations

UDT_API extern UDTSOCKET udt_socket(int af, int type, int protocol);

UDT_API extern int udt_bind(UDTSOCKET u, const struct sockaddr* name, int namelen);

UDT_API extern int udt_bind2(UDTSOCKET u, UDPSOCKET udpsock);

UDT_API extern int udt_listen(UDTSOCKET u, int backlog);

UDT_API extern UDTSOCKET udt_accept(UDTSOCKET u, struct sockaddr* addr, int* addrlen);

UDT_API extern int udt_connect(UDTSOCKET u, const struct sockaddr* name, int namelen);

UDT_API extern int udt_close(UDTSOCKET u);

UDT_API extern int udt_getpeername(UDTSOCKET u, struct sockaddr* name, int* namelen);

UDT_API extern int udt_getsockname(UDTSOCKET u, struct sockaddr* name, int* namelen);

UDT_API extern int udt_getsockopt(UDTSOCKET u, int level, int optname, void* optval, int* optlen);

UDT_API extern int udt_setsockopt(UDTSOCKET u, int level, int optname, const void* optval, int optlen);

UDT_API extern int udt_send(UDTSOCKET u, const char* buf, int len, int flags);

UDT_API extern int udt_recv(UDTSOCKET u, char* buf, int len, int flags);

UDT_API extern int udt_sendmsg(UDTSOCKET u, const char* buf, int len, int ttl/* = -1*/, int inorder/* = false*/);

UDT_API extern int udt_recvmsg(UDTSOCKET u, char* buf, int len);

///UDT_API extern int64_t udt_sendfile(UDTSOCKET u, std::fstream& ifs, int64_t& offset, int64_t size, int block = 364000);

///UDT_API extern int64_t udt_recvfile(UDTSOCKET u, std::fstream& ofs, int64_t& offset, int64_t size, int block = 7280000);

UDT_API extern int64_t udt_sendfile2(UDTSOCKET u, const char* path, int64_t* offset, int64_t size, int block/* = 364000*/);

UDT_API extern int64_t udt_recvfile2(UDTSOCKET u, const char* path, int64_t* offset, int64_t size, int block/* = 7280000*/);



// last error detection

UDT_API extern const char * udt_getlasterror_desc();

UDT_API extern int udt_getlasterror_code();

UDT_API extern void udt_clearlasterror();



// performance track

UDT_API extern int udt_perfmon(UDTSOCKET u, UDT_TRACEINFO * perf, int clear);



// get UDT socket state

UDT_API extern int udt_getsockstate(UDTSOCKET u);



// event mechanism

// select and selectEX are DEPRECATED; please use epoll.

enum UDT_EPOLLOpt

{

	// this values are defined same as linux epoll.h

	// so that if system values are used by mistake, they should have the same effect

	UDT_UDT_EPOLL_IN  = 0x1,

	UDT_UDT_EPOLL_OUT = 0x4,

	UDT_UDT_EPOLL_ERR = 0x8

};



UDT_API extern int udt_epoll_create();

UDT_API extern int udt_epoll_add_usock(int eid, UDTSOCKET u, const int* events);

UDT_API extern int udt_epoll_add_ssock(int eid, SYSSOCKET s, const int* events);

UDT_API extern int udt_epoll_remove_usock(int eid, UDTSOCKET u);

UDT_API extern int udt_epoll_remove_ssock(int eid, SYSSOCKET s);

///UDT_API extern int udt_epoll_wait(int eid, std::set<UDTSOCKET>* readfds, std::set<UDTSOCKET>* writefds, int64_t msTimeOut,

///                       std::set<SYSSOCKET>* lrfds = NULL, std::set<SYSSOCKET>* wrfds = NULL);

UDT_API extern int udt_epoll_wait2(int eid, UDTSOCKET* readfds, int* rnum, UDTSOCKET* writefds, int* wnum, int64_t msTimeOut,

                        SYSSOCKET* lrfds, int* lrnum, SYSSOCKET* lwfds, int* lwnum);

UDT_API extern int udt_epoll_release(int eid);



#ifdef __cplusplus

}

#endif



#endif /* UDTC_H_ */

