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
#ifndef __SERVERSHR
#define __SERVERSHR

#include <config.h>
#ifdef _WIN32
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
typedef void *pthread_cond_t;
typedef void *pthread_t;
typedef void *pthread_mutex_t;
typedef void *pthread_rwlock_t;
#endif
#else
#define _GNU_SOURCE		/* glibc2 needs this */
#if defined(__sparc__)
#include "/usr/include/sys/types.h"
#else
#include <sys/types.h>
#endif
#include <pthread.h>
#endif

#include <servershr_messages.h>

EXPORT extern int ServerAbortServer(char *server, int flush);
EXPORT extern int ServerBuildDispatchTable(char *wildcard, char *monitor_name, void **table);
EXPORT extern int ServerCloseTrees(char *server);
EXPORT extern int ServerCreatePulse(int *id, char *server, char *tree, int shot,
			     void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, void (*before_ast) ());
EXPORT extern int ServerDispatchAction(int *id, char *server, char *tree, int shot, int nid,
				void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock, int *socket,
				void (*before_ast) ());
EXPORT extern int ServerDispatchClose(void *vtable);
EXPORT extern int ServerDispatchCommand(int *id, char *server, char *cli, char *command,
				 void (*ast) (), void *astprm, int *retstatus, pthread_rwlock_t *lock,
				 void (*before_ast) ());
EXPORT extern int ServerSetLinkDownHandler(void (*handler) ());
EXPORT extern void ServerSetDetailProc(char *(*detail_proc) (int));
EXPORT extern char *(*ServerGetDetailProc()) (int);
EXPORT extern int ServerDispatchPhase(int *id, void *vtable, char *phasenam, char noact_in,
			       int sync, void (*output_rtn) (), char *monitor);
EXPORT extern int ServerFailedEssential(void *vtable, int reset);
EXPORT extern char *ServerFindServers(void **ctx, char *wild_match);
EXPORT extern int ServerMonitorCheckin(char *server, void (*ast) (), void *astparam);
EXPORT extern int ServerSetLogging(char *server, char logging_mode);
EXPORT extern int ServerStartServer(char *server);
EXPORT extern int ServerStopServer(char *server);
EXPORT extern void ServerWait(int id);
EXPORT extern int ServerFreeDispatchTable(void *vtable);
EXPORT extern char *ServerGetInfo(int full, char *server);
#endif
