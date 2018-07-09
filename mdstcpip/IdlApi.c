/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <STATICdef.h>
#include <mdsplus/mdsconfig.h>
#include <ipdesc.h>
#ifdef WIN32
#define BlockSig(arg)
#define UnBlockSig(arg)
#else
#include <signal.h>
#include <string.h>
#ifndef NULL
#define NULL (void *)0
#endif
STATIC_ROUTINE int BlockSig(int sig_number)
{
  sigset_t newsigset;
#if defined(sun)
  if (sig_number == SIGALRM) {	/* Solaris: simple block doesn't work?  */
    struct sigaction act;
    sigaction(sig_number, NULL, &act);	/* get current state ...        */
    if (~act.sa_flags & SA_RESTART) {	/*...set SA_RESTART bit         */
      act.sa_flags |= SA_RESTART;
      if (sigaction(sig_number, &act, NULL))
	perror("BlockSig *err* sigaction");
    }
  }
#endif
  sigemptyset(&newsigset);
  sigaddset(&newsigset, sig_number);
  return sigprocmask(SIG_BLOCK, &newsigset, NULL);
}

STATIC_ROUTINE int UnBlockSig(int sig_number)
{
  sigset_t newsigset;
  sigemptyset(&newsigset);
  sigaddset(&newsigset, sig_number);
  return sigprocmask(SIG_UNBLOCK, &newsigset, NULL);
}
#endif

#ifdef __MWERKS__
// Start of Mac Changes
static short bGUSIInit = 0;

STATIC_ROUTINE void BlockSig(int)
{
  if (!bGUSIInit) {
    //      GUSISetup ( GUSIwithInternetSockets );
    GUSISetupConfig();
    bGUSIInit = 1;
  }
}

STATIC_ROUTINE void UnBlockSig(int)
{
}

void main()
{
}

// End of Mac Changes
#endif

EXPORT int IdlMdsClose(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsClose', connection, value=[1b])
*/
  int status = 0;
  BlockSig(SIGALRM);
  if (lArgc == 1) {
    status = MdsClose((int)((char *)lpvArgv[0] - (char *)0));
  }
  UnBlockSig(SIGALRM);
  return status;
}

EXPORT int IdlConnectToMds(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlConnectToMds', 'host-name')
*/
  int status = 0;
  BlockSig(SIGALRM);
  if (lArgc == 1) {
    status = ConnectToMds((char *)lpvArgv[0]);
  }
  UnBlockSig(SIGALRM);
  return status;
}

EXPORT int IdlDisconnectFromMds(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlDisconnectFromMds', connection, value=[1b])
*/
  int status = 0;
  BlockSig(SIGALRM);
  if (lArgc == 1) {
    status = DisconnectFromMds((int)((char *)lpvArgv[0] - (char *)0));
  }
  UnBlockSig(SIGALRM);
  return status;
}

EXPORT int IdlMdsOpen(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsOpen', connection, 'tree-name', shot, value = [1b,0b,1b]) 
*/
  int status = 0;
  if (lArgc == 3) {
    BlockSig(SIGALRM);
    status =
      MdsOpen((int)((char *)lpvArgv[0] - (char *)0), (char *)lpvArgv[1],
	      (int)((char *)lpvArgv[2] - (char *)0));
    UnBlockSig(SIGALRM);
  }
  return status;
}

EXPORT int IdlMdsSetDefault(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlMdsSetDefault', connection, 'node', value = [1b,0b]) 
*/
  int status = 0;
  if (lArgc == 2) {
    BlockSig(SIGALRM);
    status = MdsSetDefault((int)((char *)lpvArgv[0] - (char *)0), (char *)lpvArgv[1]);
    UnBlockSig(SIGALRM);
  }
  return status;
}

EXPORT int IdlGetAnsInfo(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlGetAnsInfo', connection_l, dtype_b, length_w, ndims_b, dims_l[7], numbytes_l, ans,
                               value=[1b,0b,0b,0b,0b,0b,0b])
*/
  int status = 0;
  if (lArgc == 7) {
    BlockSig(SIGALRM);
    status =
      GetAnswerInfo((int)((char *)lpvArgv[0] - (char *)0), (char *)lpvArgv[1], (short *)lpvArgv[2],
		    (char *)lpvArgv[3], (int *)lpvArgv[4], (int *)lpvArgv[5], (void **)lpvArgv[6]);
    UnBlockSig(SIGALRM);
  }
  return status;
}

EXPORT int Idlmemcpy(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','Idlmemcpy', answer, answer_ptr, nbytes, value=[0b,1b,1b])
*/
  int status = 0;
  if (lArgc == 3) {
#ifdef __alpha
    memcpy((void *)lpvArgv[0], *(void **)lpvArgv[1], (int)((char *)lpvArgv[2] - (char *)0));
#else
    memcpy((void *)lpvArgv[0], (void *)lpvArgv[1], (int)((char *)lpvArgv[2] - (char *)0));
#endif
    status = 1;
  }
  return status;
}

EXPORT int IdlSendArg(int lArgc, void * *lpvArgv)
{
/*  status = call_external('mdsipshr','IdlSendArg', connection_l, idx_l, dtype_b, nargs_w, length_w, ndims_b, dims_l[7], 
			    bytes, value=[1b,1b,1b,1b,1b,1b,1b,0b,0b])
*/
  int status = 0;
  if (lArgc == 8) {
    unsigned char idx = (unsigned char)((char *)lpvArgv[1] - (char *)0);
    unsigned char dtype = (unsigned char)((char *)lpvArgv[2] - (char *)0);
    unsigned char nargs = (unsigned char)((char *)lpvArgv[3] - (char *)0);
    short length = (short)((char *)lpvArgv[4] - (char *)0);
    char ndims = (char)((char *)lpvArgv[5] - (char *)0);
    BlockSig(SIGALRM);
    status =
      SendArg((int)((char *)lpvArgv[0] - (char *)0), idx, dtype, nargs, length, ndims,
	      (int *)lpvArgv[6], (char *)lpvArgv[7]);
    UnBlockSig(SIGALRM);
  }
  return status;
}

EXPORT int IdlSetCompressionLevel(int lArgc, void * *lpvArgv)
{
  extern int MdsSetCompression(int conid, int level);
/*  status = call_external('mdsipshr','IdlSetCompressionLevel', connection_l, level_l, value=[1b,1b])
*/
  int status = 0;
  if (lArgc == 2) {
    status = MdsSetCompression((int)((char *)lpvArgv[0] - (char *)0),
			   (int)((char *)lpvArgv[1] - (char *)0));
  }
  return status;
}
