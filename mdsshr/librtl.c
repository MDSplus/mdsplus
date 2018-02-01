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
#define _XOPEN_SOURCE_EXTENDED
#define _GNU_SOURCE		/* glibc2 needs this */
#include <mdsplus/mdsconfig.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#define setenv(name,value,overwrite) _putenv_s(name,value)
#define localtime_r(time,tm)         localtime_s(tm,time)
#else
#include <sys/wait.h>
#endif

#include <mdstypes.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <mds_stdarg.h>
#include <mdsshr_messages.h>
#include <mdsshr.h>
#include <STATICdef.h>
#include <ctype.h>
#include "mdsshrthreadsafe.h"
#include <release.h>
#define LIBRTL_SRC

typedef struct {
  char *env;
  char *file;
  struct descriptor wild_descr;
  char **env_strs;
  int num_env;
  int next_index;
  int next_dir_index;		/* index intor env_strs to put the next directory to search */
  DIR *dir_ptr;
} FindFileCtx;

typedef struct _VmList {
  void *ptr;
  struct _VmList *next;
} VmList;

typedef struct _ZoneList {
  VmList *vm;
  struct _ZoneList *next;
  pthread_mutex_t lock;
} ZoneList;

typedef struct node {
  void *left;
  void *right;
  short bal;
} LibTreeNode;

#include <libroutines.h>

#ifndef USE_TM_GMTOFF
/* tzset() sets the global statics daylight and timezone.
 * However, this is not threadsafe as we cannot prevent third party code from calling tzset.
 * Therefore, we make our private copies of timezone and daylight.
 * timezone: constant offset of the local standard time to gmt.
 * daylight: constant flag if region implements daylight saving at all. (seasonal flag is tmval.tm_isdst)
 */
time_t ntimezone_;
int daylight_;
static void tzset_(){
  tzset();
  ntimezone_ = - timezone;
  daylight_ = daylight;
}
#endif

static time_t get_tz_offset(time_t* time){
  struct tm tmval;
  localtime_r(time, &tmval);
#ifdef USE_TM_GMTOFF
  return tmval.tm_gmtoff;
#else
  RUN_FUNCTION_ONCE(tzset_);
  return (daylight_ && tmval.tm_isdst) ? ntimezone_ + 3600 : ntimezone_;
#endif
}

STATIC_CONSTANT int64_t VMS_TIME_OFFSET = LONG_LONG_CONSTANT(0x7c95674beb4000);

///
/// Waits for the specified time in seconds. Supports fractions of seconds.
///
/// \param secs the address of a constant floating point number representing the time to wait
/// \return 1 if successful, 0 if failed or interrupted.
///
EXPORT int LibWait(const float *secs)
{
  struct timespec ts;
  ts.tv_sec = (unsigned int)*secs;
  ts.tv_nsec = (unsigned int)((*secs - (float)ts.tv_sec) * 1E9);
  return nanosleep(&ts, 0) == 0;
}

///
/// Call a routine in a shared library passing zero or more arguments.
///
/// \param arglist array of void pointers whose length is specified by the first element interpreted as a long integer. A maximum of 32 arguments to the routine are supported.
/// \param routine address of the routine to call
/// \return the value returned by the routine as a void *
///
EXPORT void *LibCallg(void **arglist, void *(*routine) ())
{
  switch (*(int*)&arglist[0] & 0xff) {
  case 0:
    return (*routine) ();
  case 1:
    return (*routine) (arglist[1]);
  case 2:
    return (*routine) (arglist[1], arglist[2]);
  case 3:
    return (*routine) (arglist[1], arglist[2], arglist[3]);
  case 4:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4]);
  case 5:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5]);
  case 6:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6]);
  case 7:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7]);
  case 8:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8]);
  case 9:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9]);
  case 10:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10]);
  case 11:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11]);
  case 12:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12]);
  case 13:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13]);
  case 14:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14]);
  case 15:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15]);
  case 16:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16]);
  case 17:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17]);
  case 18:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17],
		       arglist[18]);
  case 19:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19]);
  case 20:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20]);
  case 21:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21]);
  case 22:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22]);
  case 23:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23]);
  case 24:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23],
		       arglist[24]);
  case 25:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25]);
  case 26:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26]);
  case 27:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27]);
  case 28:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27], arglist[28]);
  case 29:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27], arglist[28], arglist[29]);
  case 30:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27], arglist[28], arglist[29],
		       arglist[30]);
  case 31:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27], arglist[28], arglist[29], arglist[30],
		       arglist[31]);
  case 32:
    return (*routine) (arglist[1], arglist[2], arglist[3], arglist[4], arglist[5], arglist[6],
		       arglist[7], arglist[8], arglist[9], arglist[10], arglist[11], arglist[12],
		       arglist[13], arglist[14], arglist[15], arglist[16], arglist[17], arglist[18],
		       arglist[19], arglist[20], arglist[21], arglist[22], arglist[23], arglist[24],
		       arglist[25], arglist[26], arglist[27], arglist[28], arglist[29], arglist[30],
		       arglist[31], arglist[32]);
  default:
    printf("Error - currently no more than 32 arguments supported on external calls\n");
  }
  return 0;
}

#ifdef _WIN32

STATIC_ROUTINE char *GetRegistry(HKEY where, const char *pathname)
{
  HKEY regkey;
  unsigned char *path = NULL;
  if (RegOpenKeyEx(where, "SOFTWARE\\MIT\\MDSplus", 0, KEY_READ, &regkey) == ERROR_SUCCESS) {
    unsigned long valtype;
    unsigned long valsize;
    if (RegQueryValueEx(regkey, pathname, 0, &valtype, NULL, &valsize) == ERROR_SUCCESS) {
      valsize += 2;
      path = malloc(valsize + 1);
      RegQueryValueEx(regkey, pathname, 0, &valtype, path, &valsize);
    }
    RegCloseKey(regkey);
  }
  return (char *)path;
}


EXPORT int LibSpawn(struct descriptor *cmd, int waitFlag, int notifyFlag __attribute__ ((unused))){
  char *cmd_c = MdsDescrToCstring(cmd);
  int status;
  void *arglist[255];
  char *tok;
  int numargs = 6;
  tok = strtok(cmd_c, " ");
  arglist[0] = numargs+NULL;
  arglist[1] = (void *)(NULL + (waitFlag ? _P_WAIT : _P_NOWAIT));
  arglist[2] = "cmd";
  arglist[3] = arglist[2];
  arglist[4] = (void *)"/C";
  arglist[5] = (void *)tok;
  while ((tok = strtok(0, " ")) != 0) {
    if (strlen(tok) > 0) {
      arglist[numargs++] = (void *)tok;
      arglist[0] = numargs+NULL;
    }
  }
  arglist[numargs]=(void *)NULL;
  status = (char *)LibCallg(arglist, (void *)_spawnlp) - (char *)NULL;
  free(cmd_c);
  return status;
}

#else				/* WIN32 */


STATIC_ROUTINE char const *nonblank(char *p)
{
  if (!p)
    return (0);
  for (; *p && isspace(*p); p++) ;
  return (*p ? p : 0);
}


STATIC_ROUTINE void child_done(int sig   )
{
  if (sig == SIGCHLD)
    fprintf(stdout, "--> Process completed\n");
  else
    fprintf(stderr, "--> child_done: *NOTE*  sig=0x%08X\n", sig);

  return;
}

EXPORT int LibSpawn(struct descriptor *cmd, int waitflag, int notifyFlag)
{
  char *sh = "/bin/sh";
  pid_t pid, xpid;
  char *cmdstring = MdsDescrToCstring(cmd);
  char *spawn_wrapper = TranslateLogical("MDSPLUS_SPAWN_WRAPPER");
  int sts = 0;
  if (spawn_wrapper) {
    char *oldcmdstring = cmdstring;
    cmdstring = strcpy(malloc(strlen(spawn_wrapper) + strlen(oldcmdstring) + 5), spawn_wrapper);
    strcat(cmdstring, " ");
    strcat(cmdstring, oldcmdstring);
    free(oldcmdstring);
    TranslateLogicalFree(spawn_wrapper);
  }
  signal(SIGCHLD, notifyFlag ? child_done : (waitflag ? SIG_DFL : SIG_IGN));
  pid = fork();
  if (!pid) {
  /*-------------> child process: execute cmd	*/
    char const *arglist[4];
    int i = 0;
    if (!waitflag) {
      pid = fork();
      if (pid != -1 && pid != 0)
	_exit(0);
    }
    signal(SIGCHLD, SIG_DFL);
    arglist[0] = getenv("SHELL");
    if (arglist[0] == 0)
      arglist[0] = sh;
    i++;
    if (cmd->length != 0) {
      arglist[i++] = "-c";
      arglist[i++] = nonblank(cmdstring);
    }
    arglist[i] = 0;
    sts = execvp(arglist[0], (char * const *)arglist);
  }
  /*-------------> parent process ...		*/
  if (pid == -1) {
    fprintf(stderr, "Error %d from fork()\n", errno);
    return (0);
  }
  /*  if (waitflag || cmd->length == 0)
     {
   */
  for (;;) {
    xpid = waitpid(pid, &sts, 0);
    if (xpid == pid)
      break;
    else if (xpid == -1) {
      if (errno != ECHILD)
	perror("Error during wait call");
      break;
    }
  }
  /*
     }
   */
  free(cmdstring);
  /* return sts; */
  return sts >> 8;
}


#endif

EXPORT char *TranslateLogical(char const *pathname)
{
  char *tpath = getenv(pathname);
  if (tpath) return strdup(tpath);
#ifdef _WIN32
  tpath = GetRegistry(HKEY_CURRENT_USER, pathname);
  if (tpath) return strdup(tpath);
  tpath = GetRegistry(HKEY_LOCAL_MACHINE, pathname);
  if (tpath) return strdup(tpath);
#endif
  return NULL;
}

#ifndef va_count
#define  va_count(narg) va_start(incrmtr, first); \
                        for (narg=1; (narg < 256) && (va_arg(incrmtr, struct descriptor *) != MdsEND_ARG); narg++)
#endif				/* va_count */

EXPORT int TranslateLogicalXd(struct descriptor const *in, struct descriptor_xd *out)
{
  struct descriptor out_dsc = { 0, DTYPE_T, CLASS_S, 0 };
  int status = 0;
  char *in_c = MdsDescrToCstring(in);
  char *out_c = TranslateLogical(in_c);
  if (out_c) {
    out_dsc.length = (unsigned short)strlen(out_c);
    out_dsc.pointer = out_c;
    status = 1;
  }
  MdsCopyDxXd(&out_dsc, out);
  if (out_c)
    TranslateLogicalFree(out_c);
  free(in_c);
  return status;
}

EXPORT void MdsFree(void *ptr)
{
  free(ptr);
}

EXPORT char *MdsDescrToCstring(struct descriptor const *in)
{
  char *out = malloc((size_t)in->length + 1);
  memcpy(out, in->pointer, in->length);
  out[in->length] = 0;
  return out;
}

//int LibSigToRet()
//{
//  return 1;
//}

STATIC_THREADSAFE char *FIS_Error = "";

EXPORT char *LibFindImageSymbolErrString()
{
  return FIS_Error;
}

static void *loadLib(const char *dirspec, const char *filename, char *errorstr) {
  void *handle = NULL;
  char *full_filename = alloca( strlen(dirspec) + strlen(filename) + 10);
  if (strlen(dirspec)>0) {
    if (strchr(dirspec,'\\')) {
      sprintf(full_filename, "%s\\%s", dirspec, filename);
    } else {
      sprintf(full_filename, "%s/%s", dirspec, filename);
    }
  } else {
    strcpy(full_filename,filename);
  }
  handle = dlopen(full_filename, RTLD_NOW);
  if (handle == NULL) {
    snprintf(errorstr + strlen(errorstr), 4096 - strlen(errorstr), "Error loading %s: %s\n", full_filename, dlerror());
  }
  return handle;
}

EXPORT int LibFindImageSymbol_C(const char *filename_in, const char *symbol, void **symbol_value)
{
#ifdef _WIN32
  const char *prefix="";
  const char delim = ';';
#else
  const char *prefix="lib";
  const char delim = ':';
#endif
  int status;
  void *handle = NULL;
  char *errorstr = alloca(4096);
  char *filename;
  static pthread_mutex_t dlopen_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&dlopen_mutex);
  pthread_cleanup_push((void*)pthread_mutex_unlock,(void*)&dlopen_mutex);
  filename = alloca(strlen(filename_in) + strlen(prefix) + strlen(SHARELIB_TYPE) + 1);
  errorstr[0]='\0';
  *symbol_value = NULL;
  strcpy(filename,filename_in);
  if ((!(strchr(filename,'/') || strchr(filename,'\\'))) &&
      (strlen(prefix) > 0) &&
      strncmp(filename_in, prefix, strlen(prefix))) {
    sprintf(filename, "%s%s", prefix, filename_in);
  }
  if (strcmp(filename+strlen(filename)-strlen(SHARELIB_TYPE),SHARELIB_TYPE)) {
    strcat(filename,SHARELIB_TYPE);
  }
  handle = loadLib("", filename, errorstr);
  if (handle == NULL &&
      (strchr(filename, '/') == 0) &&
      (strchr(filename, '\\') == 0)) {
    char *library_path=getenv("MDSPLUS_LIBRARY_PATH");
    if (library_path) {
      size_t offset = 0;
      char *libpath=strdup(library_path);
      while (offset < strlen(library_path)) {
	char *dptr = strchr(libpath+offset, delim);
	if (dptr)
	  *dptr='\0';
	handle = loadLib(libpath+offset, filename, errorstr);
	if (handle)
	  break;
	offset = offset+strlen(libpath+offset)+1;
      }
      free(libpath);
    }
    if ((handle == NULL) && (delim == ':')) {
      char *mdir = getenv("MDSPLUS_DIR");
      if (mdir) {
	char *libdir = alloca(strlen(mdir)+10);
	sprintf(libdir, "%s/%s", mdir, "lib");
	handle = loadLib(libdir, filename, errorstr);
      }
    }
  }
  if (handle != NULL) {
    *symbol_value = dlsym(handle, symbol);
    if (*symbol_value == NULL) {
      snprintf(errorstr + strlen(errorstr), 4096 - strlen(errorstr), "Error: %s\n", dlerror());
    }
  }
  if (strlen(FIS_Error) > 0) {
    free(FIS_Error);
    FIS_Error="";
  }
  if (*symbol_value == NULL) {
    FIS_Error = strdup(errorstr);
    status = LibKEYNOTFOU;
  }
  else
    status = 1;
  pthread_cleanup_pop(1);
  return status;
}

EXPORT int LibFindImageSymbol(struct descriptor *filename, struct descriptor *symbol, void **symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  char *c_symbol = MdsDescrToCstring(symbol);
  int status = LibFindImageSymbol_C(c_filename, c_symbol, symbol_value);
  free(c_filename);
  free(c_symbol);
  return status;
}

EXPORT int StrConcat(struct descriptor *out, struct descriptor *first, ...)
{
  int i;
  int narg;
  va_list incrmtr;
  int status = StrCopyDx(out, first);
  if STATUS_OK {
    va_count(narg);
    va_start(incrmtr, first);
    if (out->class == CLASS_D) {
      struct descriptor *arg = va_arg(incrmtr, struct descriptor *);
      for (i = 1; i < narg && STATUS_OK && arg; i++) {
	StrAppend((struct descriptor_d *)out, arg);
	arg = va_arg(incrmtr, struct descriptor *);
      }
    } else if (out->class == CLASS_S) {
      struct descriptor temp = *out;
      struct descriptor *next;
      for (i = 1,
	   temp.length = (unsigned short)(out->length - first->length),
	   temp.pointer = out->pointer + first->length;
	   i < narg && STATUS_OK && temp.length > 0;
	   i++,
	   temp.length = (unsigned short)(temp.length - next->length),
	   temp.pointer += next->length) {
	next = va_arg(incrmtr, struct descriptor *);
	if (next) {
	  StrCopyDx(&temp, next);
	} else {
	  temp.length = 0;
	}
      }
    } else
      status = MDSplusERROR;
  }
  return status;
}

EXPORT int StrPosition(struct descriptor *source, struct descriptor *substring, int *start)
{
  char *source_c = MdsDescrToCstring(source);
  char *substring_c = MdsDescrToCstring(substring);
  char *search = source_c + ((start && *start > 0) ? (*start - 1) : 0);
  char *found = strstr(search, substring_c);
  int answer = found ? (int)(found - source_c) + 1 : 0;
  free(source_c);
  free(substring_c);
  return answer;
}

EXPORT int StrCopyR(struct descriptor *dest, const unsigned short *len, char *source)
{
  const struct descriptor s = { *len, DTYPE_T, CLASS_S, source };
  return StrCopyDx(dest, &s);
}

EXPORT int StrLenExtr(struct descriptor *dest, struct descriptor *source, int *start_in, int *len_in)
{
  unsigned short len = (unsigned short)((*len_in < 0) ? 0 : *len_in & 0xffff);
  unsigned short start = (unsigned short)((*start_in > 1) ? *start_in & 0xffff : 1);
  struct descriptor_d s = { 0, DTYPE_T, CLASS_D, 0 };
  int status = StrGet1Dx(&len, &s);
  int i, j;
  memset(s.pointer, 32, len);
  for (i = start - 1, j = 0; ((i < source->length) && (j < len)); i++, j++)
    s.pointer[j] = source->pointer[i];
  status = StrCopyDx(dest, (struct descriptor *)&s);
  StrFree1Dx(&s);
  return status;
}

EXPORT int StrGet1Dx(const unsigned short *len, struct descriptor_d *out)
{
  if (out->class != CLASS_D)
    return LibINVSTRDES;
  if (out->length == *len)
    return MDSplusSUCCESS;
  if (out->length && (out->pointer != NULL))
    free(out->pointer);
  out->length = *len;
  out->pointer = *len ? malloc(*len) : NULL;
  return MDSplusSUCCESS;
}

//int LibEmul(int *m1, int *m2, int *add, int64_t * prod)
//{
//  int64_t m1_64 = *m1;
//  int64_t m2_64 = *m2;
//  int64_t add_64 = *add;
//  *prod = m1_64 * m2_64 + add_64;
//  return 1;
//}

int LibSFree1Dd(struct descriptor_d *out)
{
  return StrFree1Dx(out);
}

EXPORT int StrTrim(struct descriptor *out, struct descriptor *in, unsigned short *lenout)
{
  struct descriptor_d tmp = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor s = { 0, DTYPE_T, CLASS_S, 0 };
  unsigned short i;
  for (i = in->length; i > 0; i--)
    if (in->pointer[i - 1] != 32 && in->pointer[i - 1] != 9)
      break;
  StrCopyDx((struct descriptor *)&tmp, in);
  s.length = i;
  s.pointer = tmp.pointer;
  if (lenout != NULL)
    *lenout = s.length;
  StrCopyDx(out, &s);
  return StrFree1Dx(&tmp);
}

EXPORT int StrCopyDx(struct descriptor *out, const struct descriptor *in)
{
  if (out->class == CLASS_D && (in->length != out->length))
    StrGet1Dx(&in->length, (struct descriptor_d *)out);
  if (out->length && out->pointer != NULL) {
    unsigned int outlength = (out->class == CLASS_A) ? ((struct descriptor_a *)out)->arsize : out->length;
    unsigned int inlength = (in->class == CLASS_A) ? ((struct descriptor_a *)in)->arsize : in->length;
    unsigned int len = outlength > inlength ? inlength : outlength;
    char *p1, *p2;
    unsigned int i;
    for (i = 0, p1 = out->pointer, p2 = in->pointer; i < len; i++)
      *p1++ = *p2++;
    if (outlength > inlength)
      memset(out->pointer + inlength, 32, outlength - inlength);
  }
  return MDSplusSUCCESS;
}

EXPORT int StrCompare(struct descriptor *str1, struct descriptor *str2)
{
  char *str1c = MdsDescrToCstring(str1);
  char *str2c = MdsDescrToCstring(str2);
  int ans;
  ans = strcmp(str1c, str2c);
  free(str1c);
  free(str2c);
  return ans;
}

EXPORT int StrUpcase(struct descriptor *out, struct descriptor *in)
{
  unsigned int outlength,i;
  StrCopyDx(out, in);
  outlength = (out->class == CLASS_A) ? ((struct descriptor_a *)out)->arsize : out->length;
  for (i = 0; i < outlength; i++)
    out->pointer[i] = (char)toupper(out->pointer[i]);
  return MDSplusSUCCESS;
}

EXPORT int StrRight(struct descriptor *out, struct descriptor *in, unsigned short *start)
{
  struct descriptor_d tmp = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor s = { 0, DTYPE_T, CLASS_S, 0 };
  StrCopyDx((struct descriptor *)&tmp, in);
  s.length = (unsigned short)((int)in->length - *start + 1);
  s.pointer = tmp.pointer + (*start - 1);
  StrCopyDx(out, &s);
  return StrFree1Dx(&tmp);
}

static pthread_mutex_t zones_lock = PTHREAD_MUTEX_INITIALIZER;
#define   LOCK_ZONES pthread_mutex_lock(&zones_lock);pthread_cleanup_push((void*)pthread_mutex_unlock,&zones_lock)
#define UNLOCK_ZONES pthread_cleanup_pop(1);
ZoneList *MdsZones = NULL;
#define   LOCK_ZONE(zone) pthread_mutex_lock(&(zone)->lock);pthread_cleanup_push((void*)pthread_mutex_unlock,&(zone)->lock)
#define UNLOCK_ZONE(zone) pthread_cleanup_pop(1);

EXPORT int LibCreateVmZone(ZoneList ** zone)
{
  ZoneList *list;
  *zone = malloc(sizeof(ZoneList));
  (*zone)->vm = NULL;
  (*zone)->next = NULL;
  pthread_mutex_init(&(*zone)->lock,NULL);
  LOCK_ZONES;
  if (MdsZones == NULL)
    MdsZones = *zone;
  else {
    for (list = MdsZones; list->next; list = list->next) ;
    list->next = *zone;
  }
  UNLOCK_ZONES;
  return (*zone != NULL);
}

EXPORT int LibDeleteVmZone(ZoneList ** zone)
{
  int found;
  ZoneList *list, *prev;
  LibResetVmZone(zone);
  LOCK_ZONES;
  found = 0;
  if (*zone == MdsZones) {
    found = 1;
    MdsZones = (*zone)->next;
  } else {
    for (prev = 0, list = MdsZones; list && list != *zone; prev = list, list = list->next) ;
    if (list && prev) {
      prev->next = list->next;
      found = 1;
    }
  }
  if (found) {
    free(*zone);
    *zone = 0;
  }
  UNLOCK_ZONES;
  return found;
}

EXPORT int LibResetVmZone(ZoneList ** zone)
{
  VmList *list;
  unsigned int len = 1;
  LOCK_ZONES;
  while ((list = zone ? (*zone ? (*zone)->vm : NULL) : NULL) != NULL)
    LibFreeVm(&len, &list->ptr, zone);
  UNLOCK_ZONES;
  return MDSplusSUCCESS;
}

EXPORT int LibFreeVm(unsigned int *len, void **vm, ZoneList ** zone)
{
  VmList *list = NULL;
  if (zone) {
    LOCK_ZONE(*zone);
    VmList *prev;
    for (prev = NULL, list = (*zone)->vm;
	 list && (list->ptr != *vm); prev = list, list = list->next) ;
    if (list) {
      if (prev)
	prev->next = list->next;
      else
	(*zone)->vm = list->next;
    }
    UNLOCK_ZONE(*zone);
  }
  if (len && *len && vm && *vm)
    free(*vm);
  if (list)
    free(list);
  return MDSplusSUCCESS;
}

EXPORT int libfreevm_(unsigned int *len, void **vm, ZoneList ** zone)
{
  return (LibFreeVm(len, vm, zone));
}

EXPORT int libfreevm(unsigned int *len, void **vm, ZoneList ** zone)
{
  return (LibFreeVm(len, vm, zone));
}

EXPORT int LibGetVm(unsigned int *len, void **vm, ZoneList ** zone)
{
  *vm = malloc(*len);
  if (*vm == NULL) {
    printf("Insufficient virtual memory\n");
  }
  if (zone) {
    VmList *list = malloc(sizeof(VmList));
    list->ptr = *vm;
    list->next = NULL;
    LOCK_ZONE(*zone);
    if ((*zone)->vm) {
      VmList *ptr;
      for (ptr = (*zone)->vm; ptr->next; ptr = ptr->next) ;
      ptr->next = list;
    } else
      (*zone)->vm = list;
    UNLOCK_ZONE(*zone);
  }
  return (*vm != NULL);
}

EXPORT int libgetvm_(unsigned int *len, void **vm, ZoneList ** zone)
{
  return (LibGetVm(len, vm, zone));
}

EXPORT int libgetvm(unsigned int *len, void **vm, ZoneList ** zone)
{
  return (LibGetVm(len, vm, zone));
}

//int LibEstablish()
//{
//  return 1;
//}

#define SEC_PER_DAY (60*60*24)

EXPORT int LibConvertDateString(const char *asc_time, int64_t * qtime)
{
  time_t tim = 0;
  char time_out[24];
  int parse_it = 1;
  int ctime_it = 0;

  /* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
  if (asc_time == 0 || strcasecmp(asc_time, "now") == 0) {
    tim = time(NULL);
    parse_it = 0;
  } else if (strcasecmp(asc_time, "today") == 0) {
    tim = time(NULL);
    ctime_it = 1;
  } else if (strcasecmp(asc_time, "tomorrow") == 0) {
    tim = time(NULL) + SEC_PER_DAY;
    ctime_it = 1;
  } else if (strcasecmp(asc_time, "yesterday") == 0) {
    tim = time(NULL) - SEC_PER_DAY;
    ctime_it = 1;
  }
  if (parse_it) {
    if (ctime_it) {
      char *time_str;
      time_str = ctime(&tim);
      time_out[0] = time_str[8];
      time_out[1] = time_str[9];
      time_out[2] = '-';
      time_out[3] = time_str[4];
      time_out[4] = (char)(time_str[5] & 0xdf);
      time_out[5] = (char)(time_str[6] & 0xdf);
      time_out[6] = '-';
      time_out[7] = time_str[20];
      time_out[8] = time_str[21];
      time_out[9] = time_str[22];
      time_out[10] = time_str[23];
      time_out[11] = ' ';
      time_out[12] = '0';
      time_out[13] = '0';
      time_out[14] = ':';
      time_out[15] = '0';
      time_out[16] = '0';
      time_out[17] = ':';
      time_out[18] = '0';
      time_out[19] = '0';
      time_out[20] = '.';
      time_out[21] = '0';
      time_out[22] = '0';
      time_out[23] = 0;
      tim = 0;
      asc_time = time_out;
    }

    {
      struct tm tm = { 0 };
#ifdef _WIN32
      unsigned int day, year, hour, minute, second;
      char month[4];
      char *months[] =
	  { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
      if (sscanf(asc_time, "%u-%3s-%u %u:%u:%u", &day, month, &year, &hour, &minute, &second) < 6) {
	return 0;
      }
      _strupr(month);
      for (tm.tm_mon = 0; tm.tm_mon < 12; tm.tm_mon++) {
	if (strcmp(month, months[tm.tm_mon]) == 0) {
	  break;
	}
      }
      if ((tm.tm_mon == 12) || (day < 1) || (day > 31) ||
	  (year < 1900) || (year > 2200) || (hour > 23) || (minute > 59) || (second > 59))
	return 0;
      tm.tm_mday = day;
      tm.tm_year = year - 1900;
      tm.tm_hour = hour;
      tm.tm_min = minute;
      tm.tm_sec = second;
#else
      strptime(asc_time, "%d-%b-%Y %H:%M:%S", &tm);
#endif
      tm.tm_isdst = -1;
      tim = mktime(&tm);
      if ((int)tim == -1)
	return 0;
    }
  }
  if (tim > 0)
    LibTimeToVMSTime(&tim, qtime);
  else
    *qtime = 0;
  return tim > 0;
}

EXPORT int LibTimeToVMSTime(const time_t * time_in, int64_t * time_out)
{
  time_t time_to_use = time_in ? *time_in : time(NULL);
  struct timeval tv;
  if (time_in)
    tv.tv_usec = 0;
  else
    gettimeofday(&tv,0);
  time_t tz_offset = get_tz_offset(&time_to_use);
  *time_out = (int64_t) (time_to_use + tz_offset) * (int64_t) 10000000 + tv.tv_usec * 10 + VMS_TIME_OFFSET;
  return MDSplusSUCCESS;
}

EXPORT time_t LibCvtTim(int *time_in, double *t)
{
  double t_out;
  time_t bintim = time(&bintim);
  if (time_in) {
    int64_t time_local;
    double time_d;
    memcpy(&time_local, time_in, sizeof(time_local));
    time_local = (*(int64_t *) time_in - VMS_TIME_OFFSET);
    if (time_local < 0)
      time_local = 0;
    bintim = time_local / LONG_LONG_CONSTANT(10000000);
    time_d = (double)bintim + (double)(time_local % LONG_LONG_CONSTANT(10000000)) * 1E-7;
    time_t tz_offset = get_tz_offset(&bintim);
    t_out = (time_d > 0 ? time_d : 0.0) - (double)tz_offset;
    bintim -= tz_offset;
  } else
    bintim = (long)(t_out = (double)time(0));
  if (t != 0)
    *t = t_out;
  return (bintim);
}

EXPORT int LibSysAscTim(unsigned short *len, struct descriptor *str, int *time_in)
{
  char *time_str;
  char time_out[24];
  unsigned short slen = sizeof(time_out)-1;
  time_t bintim = LibCvtTim(time_in, 0);
  int64_t chunks = time_in ? *(int64_t *)time_in % 10000000 : 0;
  time_str = ctime(&bintim);
  if (time_str) {
    time_out[0] = time_str[8];
    time_out[1] = time_str[9];
    time_out[2] = '-';
    time_out[3] = time_str[4];
    time_out[4] = (char)(time_str[5] & 0xdf);
    time_out[5] = (char)(time_str[6] & 0xdf);
    time_out[6] = '-';
    time_out[7] = time_str[20];
    time_out[8] = time_str[21];
    time_out[9] = time_str[22];
    time_out[10] = time_str[23];
    time_out[11] = ' ';
    time_out[12] = time_str[11];
    time_out[13] = time_str[12];
    time_out[14] = time_str[13];
    time_out[15] = time_str[14];
    time_out[16] = time_str[15];
    time_out[17] = time_str[16];
    time_out[18] = time_str[17];
    time_out[19] = time_str[18];
    time_out[20] = '.';
    time_out[21] = (char)('0' + chunks / 1000000);
    time_out[22] = (char)('0' + (chunks % 1000000) / 100000);
    time_out[23] = 0;
  } else
    strcpy(time_out, "\?\?-\?\?\?-\?\?\?\? \?\?:\?\?:\?\?.\?\?");
  StrCopyR(str, &slen, time_out);
  if (len)
    *len = slen;
  return MDSplusSUCCESS;
}

//int LibGetDvi(int *code, void *dummy1, struct descriptor *device, int *ans,
//	      struct descriptor *ans_string, int *len)
//{
//  *ans = 132;
//  return 1;
//}

EXPORT int StrAppend(struct descriptor_d *out, struct descriptor *tail)
{
  if (tail->length != 0 && tail->pointer != NULL) {
    int len = (int)out->length + (int)tail->length;
    if (len > 0xffff)
      return StrSTRTOOLON;
    struct descriptor_d new = { 0, DTYPE_T, CLASS_D, 0 };
    unsigned short us_len = (unsigned short)len;
    StrGet1Dx(&us_len, &new);
    if (out->pointer) {
      memcpy(new.pointer, out->pointer, out->length);
    }
    memcpy(new.pointer + out->length, tail->pointer, tail->length);
    StrFree1Dx(out);
    *out = new;
  }
  return MDSplusSUCCESS;
}

EXPORT int StrFree1Dx(struct descriptor_d *out)
{
  if (out->class == CLASS_D) {
    if (out->pointer)
      free(out->pointer);
    out->pointer = NULL;
    out->length = 0;
  }
  return MDSplusSUCCESS;
}

EXPORT int StrFindFirstNotInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    size_t tmp;
    tmp = strspn(src, s);
    ans = (int)((tmp == strlen(src)) ? 0 : (tmp + 1));
    free(src);
    free(s);
  }
  return ans;
}

EXPORT int StrFindFirstInSet(struct descriptor *source, struct descriptor *set)
{
  int ans = 0;
  if (source->length > 0) {
    char *src = MdsDescrToCstring(source);
    char *s = MdsDescrToCstring(set);
    char *tmp;
    tmp = (char *)strpbrk(src, s);
    ans = tmp ? (int)(tmp - src) + 1 : 0;
    free(src);
    free(s);
  }
  return ans;
}


struct bbtree_info {
  struct node *currentnode;
  char *keyname;
  int (*compare_routine) ();
  int (*alloc_routine) ();
  struct node *new_node;
  int foundintree;
  int controlflags;
  void *user_context;
};

STATIC_ROUTINE int MdsInsertTree();

EXPORT int LibInsertTree(LibTreeNode **treehead, void *symbol_ptr, int *control_flags,
		  int (*compare_rtn) (), int (*alloc_rtn) (), LibTreeNode **blockaddr,
		  void *user_data)
{
  struct bbtree_info bbtree;
  bbtree.currentnode = *treehead;
  bbtree.keyname = symbol_ptr;
  bbtree.compare_routine = compare_rtn;
  bbtree.alloc_routine = alloc_rtn;
  bbtree.new_node = NULL;
  bbtree.foundintree = 0;
  bbtree.controlflags = *control_flags;
  bbtree.user_context = user_data;
  MdsInsertTree(&bbtree);
  if (bbtree.foundintree == 1)
    *treehead = bbtree.currentnode;
  *blockaddr = bbtree.new_node;
  return bbtree.foundintree;
}

STATIC_ROUTINE int MdsInsertTree(struct bbtree_info *bbtree_ptr)
{

#define currentNode (bbtree_ptr->currentnode)
#define ALLOCATE    (*(bbtree_ptr->alloc_routine))
#define left_of(node_ptr) node_ptr->left
#define right_of(node_ptr) node_ptr->right
#define offset_of(node_ptr, offnode_ptr) offnode_ptr

  struct node *save_current;
  int in_balance;
  struct node *down_left;
  struct node *down_right;

  if (currentNode == 0) {
    if (!(ALLOCATE(bbtree_ptr->keyname, &save_current, bbtree_ptr->user_context) & 1))
      return MDSplusERROR;
    currentNode = save_current;
    currentNode->left = 0;
    currentNode->right = 0;
    currentNode->bal = 0;
    bbtree_ptr->new_node = save_current;
    bbtree_ptr->foundintree = 1;
    return MDSplusERROR;
  }
  save_current = currentNode;
  if ((in_balance =
       (*(bbtree_ptr->compare_routine)) (bbtree_ptr->keyname, currentNode,
					 bbtree_ptr->user_context)) <= 0) {
    if ((in_balance == 0) && (!(bbtree_ptr->controlflags & 1))) {
      bbtree_ptr->new_node = save_current;
      bbtree_ptr->foundintree = 3;
      return MDSplusSUCCESS;
    }
    currentNode = left_of(currentNode);
    in_balance = MdsInsertTree(bbtree_ptr);
    if ((bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0))
      return MDSplusSUCCESS;
    down_left = currentNode;
    currentNode = save_current;
    currentNode->left = offset_of(currentNode, down_left);
    if (in_balance)
      return MDSplusSUCCESS;
    else {
      currentNode->bal--;
      if (currentNode->bal == 0)
	return MDSplusSUCCESS;
      else {
	if (currentNode->bal & 1)
	  return MDSplusERROR;
	down_left = left_of(currentNode);
	if (down_left->bal < 0) {
	  currentNode->left = offset_of(currentNode, right_of(down_left));
	  down_left->right = offset_of(down_left, currentNode);
	  currentNode->bal = 0;
	  currentNode = down_left;
	  currentNode->bal = 0;
	  return MDSplusSUCCESS;
	} else {
	  down_right = right_of(down_left);
	  down_left->right = offset_of(down_left, left_of(down_right));
	  down_right->left = offset_of(down_right, down_left);
	  currentNode->left = offset_of(currentNode, right_of(down_right));
	  down_right->right = offset_of(down_right, currentNode);
	  currentNode->bal = 0;
	  down_left->bal = 0;
	  if (down_right->bal > 0)
	    down_left->bal = -1;
	  else if (down_right->bal < 0)
	    currentNode->bal = 1;
	  currentNode = down_right;
	  currentNode->bal = 0;
	  return MDSplusSUCCESS;
	}
      }
    }
  } else {
    currentNode = right_of(currentNode);
    in_balance = MdsInsertTree(bbtree_ptr);
    if ((bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0))
      return MDSplusSUCCESS;
    down_right = currentNode;
    currentNode = save_current;
    currentNode->right = offset_of(currentNode, down_right);
    if (in_balance)
      return MDSplusSUCCESS;
    else {
      currentNode->bal++;
      if (currentNode->bal == 0)
	return MDSplusSUCCESS;
      else {
	if (currentNode->bal & 1)
	  return MDSplusERROR;
	down_right = right_of(currentNode);
	if (down_right->bal > 0) {
	  currentNode->right = offset_of(currentNode, left_of(down_right));
	  down_right->left = offset_of(down_right, currentNode);
	  currentNode->bal = 0;
	  currentNode = down_right;
	  currentNode->bal = 0;
	  return MDSplusSUCCESS;
	} else {
	  down_left = left_of(down_right);
	  down_right->left = offset_of(down_right, right_of(down_left));
	  down_left->right = offset_of(down_left, down_right);
	  currentNode->right = offset_of(currentNode, left_of(down_left));
	  down_left->left = offset_of(down_left, currentNode);
	  currentNode->bal = 0;
	  down_right->bal = 0;
	  if (down_left->bal < 0)
	    down_right->bal = 1;
	  else if (down_left->bal > 0)
	    currentNode->bal = -1;
	  currentNode = down_left;
	  currentNode->bal = 0;
	  return MDSplusSUCCESS;
	}
      }
    }
  }
}

#undef currentNode

EXPORT int LibLookupTree(LibTreeNode **treehead, void *symbolstring, int (*compare_rtn) (),
		  LibTreeNode **blockaddr)
{
  int ch_result;
  struct node *currentnode = *treehead;
  while (currentnode != 0) {
    if ((ch_result = (*compare_rtn) (symbolstring, currentnode)) == 0) {
      *blockaddr = currentnode;
      return MDSplusSUCCESS;
    } else if (ch_result < 0)
      currentnode = left_of(currentnode);
    else
      currentnode = right_of(currentnode);
  }
  return LibKEYNOTFOU;
}

STATIC_ROUTINE int MdsTraverseTree(int (*user_rtn) (), void *user_data, struct node *currentnode);

EXPORT int LibTraverseTree(LibTreeNode **treehead, int (*user_rtn) (), void *user_data)
{
  return MdsTraverseTree(user_rtn, user_data, *treehead);
}

STATIC_ROUTINE int MdsTraverseTree(int (*user_rtn) (), void *user_data, struct node *currentnode)
{
  struct node *right_subtree;
  int status;
  if (currentnode == 0)
    return MDSplusSUCCESS;
  if (left_of(currentnode)) {
    status = MdsTraverseTree(user_rtn, user_data, left_of(currentnode));
    if (!(status & 1))
      return status;
  }
  right_subtree = right_of(currentnode);
  status = (*user_rtn) (currentnode, user_data);
  if (!(status & 1))
    return status;
  if (right_subtree) {
    status = MdsTraverseTree(user_rtn, user_data, right_subtree);
    if (!(status & 1))
      return status;
  }
  return MDSplusSUCCESS;
}

EXPORT int StrCaseBlindCompare(struct descriptor *one, struct descriptor *two)
{
  int ans=0;
  if (one->length != two->length) {
    char *one_c = MdsDescrToCstring(one);
    char *two_c = MdsDescrToCstring(two);
    ans = strcasecmp(one_c, two_c);
    free(one_c);
    free(two_c);
  } else {
    ans = strncasecmp(one->pointer, two->pointer, (size_t)one->length);
  }
  return ans;
}

EXPORT unsigned int StrMatchWild(struct descriptor *candidate, struct descriptor *pattern)
{
  struct descr {
    int length;
    char *ptr;
  };
  struct descr cand;
  struct descr scand;
  struct descr pat;
  struct descr spat;
  char pc;
  cand.length = candidate->length;
  cand.ptr = candidate->pointer;
  scand = cand;
  pat.length = pattern->length;
  pat.ptr = pattern->pointer;
  spat = pat;
  scand.length = 0;

  while (1) {
    if (--pat.length < 0) {
      if (cand.length == 0)
	return StrMATCH;
      else {
	if (--scand.length < 0)
	  return StrNOMATCH;
	else {
	  scand.ptr++;
	  cand = scand;
	  pat = spat;
	}
      }
    } else {
      if ((pc = *pat.ptr++) == '*') {
	if (pat.length == 0)
	  return StrMATCH;
	scand = cand;
	spat = pat;
      } else {
	if (--cand.length < 0)
	  return StrNOMATCH;
	if (*cand.ptr++ != pc) {
	  if (pc != '%') {
	    if (--scand.length < 0)
	      return StrNOMATCH;
	    else {
	      scand.ptr++;
	      cand = scand;
	      pat = spat;
	    }
	  }
	}
      }
    }
  }
  return StrNOMATCH;
}

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (a) > (b) ? (a) : (b)
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (a) < (b) ? (a) : (b)

EXPORT int StrElement(struct descriptor *dest, int *num, struct descriptor *delim, struct descriptor *src)
{
  char *src_ptr = src->pointer;
  char *se_ptr = src_ptr + src->length;
  char *e_ptr;
  unsigned short len;
  int cnt;
  int status;
  if (delim->length != 1)
    return StrINVDELIM;
  for (cnt = 0; (cnt < *num) && (src_ptr < se_ptr); src_ptr++)
    if (*src_ptr == *delim->pointer)
      cnt++;
  if (cnt < *num)
    return StrNOELEM;
  for (e_ptr = src_ptr; src_ptr < se_ptr; src_ptr++)
    if (*src_ptr == *delim->pointer)
      break;
  len = (unsigned short)(src_ptr - e_ptr);
  status = StrCopyR(dest, &len, e_ptr);
  return status;
}

EXPORT int StrTranslate(struct descriptor *dest, struct descriptor *src, struct descriptor *tran,
		 struct descriptor *match)
{
  int status = 0;
  if (src->class == CLASS_S || src->class == CLASS_D) {
    char *dst = (char *)malloc(src->length);
    int i;
    for (i = 0; i < src->length; i++) {
      int j;
      int next = 1;
      for (j = 0; next && j < match->length; j += next)
	next = (match->pointer[j] != src->pointer[i]) ? 1 : 0;
      dst[i] = (char)(next ? src->pointer[i] : ((j < tran->length) ? tran->pointer[j] : ' '));
    }
    status = StrCopyR(dest, &src->length, dst);
    free(dst);
  } else if ((src->class == CLASS_A) && (dest->class == CLASS_A) && (src->length > 0)
	     && (dest->length > 0)
	     && (((struct descriptor_a *)src)->arsize / src->length ==
		 ((struct descriptor_a *)dest)->arsize / dest->length)) {
    struct descriptor outdsc = { 0, DTYPE_T, CLASS_S, 0 };
    struct descriptor indsc = { 0, DTYPE_T, CLASS_S, 0 };
    unsigned int num = ((struct descriptor_a *)src)->arsize / src->length;
    unsigned int i;
    outdsc.length = dest->length;
    outdsc.pointer = dest->pointer;
    indsc.length = src->length;
    indsc.pointer = src->pointer;
    for (i = 0; i < num; i++, outdsc.pointer += outdsc.length, indsc.pointer += indsc.length)
      status = StrTranslate(&outdsc, &indsc, tran, match);
  }
  return status;
}

EXPORT int StrReplace(struct descriptor *dest, struct descriptor *src, int *start_idx, int *end_idx,
	       struct descriptor *rep)
{
  int status;
  int start;
  int end;
  unsigned short int dstlen;
  char *dst;
  char *sptr;
  char *dptr;
  start = MAX(1, MIN(*start_idx, src->length));
  end = MAX(1, MIN(*end_idx, src->length));
  dstlen = (unsigned short)(start - 1 + rep->length + src->length - end + 1);
  dst = (char *)malloc(dstlen);
  for (sptr = src->pointer, dptr = dst; (dptr - dst) < (start - 1); *dptr++ = *sptr++) ;
  for (sptr = rep->pointer; (sptr - rep->pointer) < rep->length; *dptr++ = *sptr++) ;
  for (sptr = src->pointer + end; (sptr - src->pointer) < src->length; *dptr++ = *sptr++) ;
  status = StrCopyR(dest, &dstlen, dst);
  free(dst);
  return status;
}

STATIC_ROUTINE int FindFile(struct descriptor *filespec, struct descriptor *result, FindFileCtx **ctx,
			    int recursively, int caseBlind);
STATIC_ROUTINE int FindFileStart(struct descriptor *filespec, FindFileCtx ** ctx, int caseBlind);
STATIC_ROUTINE int FindFileEnd(FindFileCtx * ctx);
STATIC_ROUTINE char *_FindNextFile(FindFileCtx * ctx, int recursively, int caseBlind);

EXPORT int LibFindFile(struct descriptor *filespec, struct descriptor *result, FindFileCtx **ctx)
{
  return FindFile(filespec, result, ctx, 0, 0);
}

EXPORT int LibFindFileRecurseCaseBlind(struct descriptor *filespec, struct descriptor *result,
				       FindFileCtx **ctx)
{
  return FindFile(filespec, result, ctx, 1, 1);
}

STATIC_ROUTINE int FindFile(struct descriptor *filespec, struct descriptor *result, FindFileCtx **ctx,
			    int recursively, int caseBlind)
{
  int status;
  char *ans;
  if (*ctx == 0) {
    status = FindFileStart(filespec, (FindFileCtx **) ctx, caseBlind);
    if STATUS_NOT_OK  return status;
  }
  ans = _FindNextFile((FindFileCtx *) * ctx, recursively, caseBlind);
  if (ans) {
    struct descriptor ansd = { 0, DTYPE_T, CLASS_S, 0 };
    ansd.length = (unsigned short)strlen(ans);
    ansd.pointer = ans;
    StrCopyDx(result, &ansd);
    free(ans);
    status = MDSplusSUCCESS;
  } else {
    status = MDSplusERROR;
    LibFindFileEnd(ctx);
  }
  return status;
}

EXPORT extern int LibFindFileEnd(FindFileCtx **ctx)
{
  int status = FindFileEnd((FindFileCtx *) * ctx);
  if STATUS_OK
    *ctx = NULL;
  return status;
}

STATIC_ROUTINE int FindFileEnd(FindFileCtx * ctx)
{
  int i;
  if (ctx != NULL) {
    if (ctx->dir_ptr) {
      closedir(ctx->dir_ptr);
      ctx->dir_ptr = 0;
    }
    if (ctx->env != 0)
      free(ctx->env);
    if (ctx->file != 0)
      free(ctx->file);
    for (i = 0; i < ctx->num_env; i++)
      if (ctx->env_strs[i] != 0)
	free(ctx->env_strs[i]);
    if (ctx->env_strs != 0)
      free(ctx->env_strs);
    free(ctx);
  }
  return MDSplusSUCCESS;
}

#define CSTRING_FROM_DESCRIPTOR(cstring, descr)\
  cstring=strncpy(malloc((size_t)(descr->length+1)),descr->pointer,descr->length);\
  cstring[descr->length] = '\0';

STATIC_ROUTINE int FindFileStart(struct descriptor *filespec, FindFileCtx ** ctx, int caseBlind)
{
  FindFileCtx *lctx;
  char *fspec;
  char *colon;
  *ctx = (FindFileCtx *) malloc(sizeof(FindFileCtx));
  memset(*ctx, 0, sizeof(FindFileCtx));
  lctx = *ctx;

  CSTRING_FROM_DESCRIPTOR(fspec, filespec)

      lctx->next_index = lctx->next_dir_index = 0;
  colon = strchr(fspec, ':');
  if (colon == 0) {
    lctx->env = 0;
    colon = fspec - 1;
  } else {
    lctx->env = strncpy(malloc((size_t)(colon - fspec + 1)), fspec, (size_t)(colon - fspec));
    lctx->env[colon - fspec] = '\0';
  }
  if (strlen(colon + 1) == 0) {
    if (lctx->env)
      free(lctx->env);
    free(fspec);
    return 0;
  } else {
    lctx->file = malloc(strlen(colon + 1) + 1);
    strcpy(lctx->file, colon + 1);
    lctx->wild_descr.length = (unsigned short)strlen(colon + 1);
    lctx->wild_descr.pointer = lctx->file;
    lctx->wild_descr.dtype = DTYPE_T;
    lctx->wild_descr.class = CLASS_S;
    if (caseBlind)
      StrUpcase(&lctx->wild_descr, &lctx->wild_descr);
    free(fspec);
  }
  if (lctx->env != 0) {
    int num = 0;
    char *env;
    char *semi;
    char *env_sav;
    env = env_sav = TranslateLogical(lctx->env);
    if (env != 0) {
      if (env[strlen(env) - 1] != ';') {
	char *tmp = malloc(strlen(env) + 2);
	strcpy(tmp, env);
	strcat(tmp, ";");
	env = tmp;
      } else {
	char *tmp = malloc(strlen(env) + 1);
	strcpy(tmp, env);
	env = tmp;
      }
      free(env_sav);
      for (semi = strchr(env, ';'); semi != 0; num++, semi = strchr(semi + 1, ';')) ;
      if (num > 0) {
	char *ptr;
	int i;
	lctx->num_env = num;
	lctx->env_strs = (char **)malloc((size_t)num * sizeof(char *));
	for (ptr = env, i = 0; i < num; i++) {
	  char *cptr;
	  int len = ((cptr = strchr(ptr, ';')) == (char *)0) ? (int)strlen(ptr) : (int)(cptr - ptr);
	  lctx->env_strs[i] = strncpy(malloc((size_t)len + 1), ptr, (size_t)len);
	  lctx->env_strs[i][len] = '\0';
	  ptr = cptr + 1;
	}
      }
      free(env);
    }
  }
  lctx->dir_ptr = 0;
  return 1;
}

STATIC_ROUTINE char *_FindNextFile(FindFileCtx * ctx, int recursively, int caseBlind)
{
  char *ans;
  struct dirent *dp;

  int found = 0;
  while (!found) {
    while (ctx->dir_ptr == 0) {
      if (ctx->next_index < ctx->num_env) {
	ctx->dir_ptr = opendir(ctx->env_strs[ctx->next_index++]);
	ctx->next_dir_index = ctx->next_index;
      } else
	return 0;
    }
    dp = readdir(ctx->dir_ptr);
    if (dp != NULL) {
      struct descriptor_d upname = { 0, DTYPE_T, CLASS_D, 0 };
      DESCRIPTOR_FROM_CSTRING(filename, dp->d_name)
	  if (caseBlind) {
	    StrUpcase((struct descriptor *)&upname, &filename);
      } else {
	    StrCopyDx((struct descriptor *)&upname, &filename);
      }
      found = StrMatchWild((struct descriptor *)&upname, &ctx->wild_descr) & 1;
      StrFree1Dx(&upname);
      if (recursively) {
	if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
	  char *tmp_dirname;
	  DIR *tmp_dir;
	  tmp_dirname = malloc(strlen(ctx->env_strs[ctx->next_index - 1]) + 2 + strlen(dp->d_name));
	  strcpy(tmp_dirname, ctx->env_strs[ctx->next_index - 1]);
	  strcat(tmp_dirname, "/");
	  strcat(tmp_dirname, dp->d_name);
	  tmp_dir = opendir(tmp_dirname);
	  if (tmp_dir != NULL) {
	    int i;
	    closedir(tmp_dir);
	    ctx->env_strs = realloc(ctx->env_strs, sizeof(char *) * (size_t)(ctx->num_env + 1));
	    for (i = ctx->num_env - 1; i >= ctx->next_dir_index; i--)
	      ctx->env_strs[i + 1] = ctx->env_strs[i];
	    ctx->env_strs[ctx->next_dir_index] = tmp_dirname;
	    ctx->num_env++;
	    ctx->next_dir_index++;
	  } else
	    free(tmp_dirname);
	}
      }
    } else {
      closedir(ctx->dir_ptr);
      ctx->dir_ptr = NULL;
    }
  }
  ans = malloc(strlen(ctx->env_strs[ctx->next_index - 1]) + 1 + strlen(dp->d_name) + 1);
  strcpy(ans, ctx->env_strs[ctx->next_index - 1]);
  strcat(ans, "/");
  strcat(ans, dp->d_name);
  return ans;
}

EXPORT void TranslateLogicalFree(char *value)
{
  free(value);
}

#ifdef LOBYTE
#undef LOBYTE
#endif
#ifdef HIBYTE
#undef HIBYTE
#endif
#define LOBYTE(x) ((x) & 0xFF)
#define HIBYTE(x) (((x) >> 8) & 0xFF)

STATIC_ROUTINE unsigned short icrc1(unsigned short crc)
{
  int i;
  unsigned int ans = crc;
  for (i = 0; i < 8; i++) {
    if (ans & 0x8000) {
      ans <<= 1;
      ans = ans ^ 4129;
    } else
      ans <<= 1;
  }
  return (unsigned short)ans;
}

unsigned short Crc(unsigned int len, unsigned char *bufptr)
{
  STATIC_THREADSAFE pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  STATIC_THREADSAFE unsigned short icrctb[256], init = 0;
  pthread_mutex_lock(&mutex);
  //  STATIC_THREADSAFE unsigned char rchr[256];
  //STATIC_CONSTANT unsigned it[16] = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 };
  if (!init) {
    init = 1;
    int i;
    for (i = 0; i < 256; i++) {
      icrctb[i] = icrc1((unsigned short)(i << 8));
      //  rchr[i] = (unsigned char)(it[i & 0xF] << 4 | it[i >> 4]);
    }
  }
  int cword = 0;
  unsigned int j;
  for (j = 0; j < len; j++)
    cword = icrctb[bufptr[j] ^ HIBYTE(cword)] ^ LOBYTE(cword) << 8;
  pthread_mutex_unlock(&mutex);
  return (unsigned short)cword;
}

EXPORT int MdsPutEnv(char const *cmd)
{
  INIT_STATUS_ERROR;
  if (cmd != NULL) {
    if (!strstr(cmd, "MDSPLUS_SPAWN_WRAPPER") && !strstr(cmd, "MDSPLUS_LIBCALL_WRAPPER")){
      char *tmp = strdup(cmd);
      char *saveptr = NULL;
      char *name = strtok_r(tmp,"=",&saveptr);
      char *value = strtok_r(NULL,"=",&saveptr);
      if (name != NULL && value != NULL)
        status = setenv(name,value,1) ? MDSplusERROR : MDSplusSUCCESS;
      free(tmp);
    }
  }
  return status;
}

EXPORT int libffs(int *position, int *size, char *base, int *find_position)
{
  INIT_STATUS_ERROR;
  int i;
  int *bits = (int *)(base + (*position) / 8);
  int top_bit_to_check = ((*size) + *position) - ((*position) / 8) * 8;
  for (i = (*position) % 8; i < top_bit_to_check; i++) {
    if (*bits & (1 << i)) {
      *find_position = ((*position) / 8) * 8 + i;
      status = MDSplusSUCCESS;
      break;
    }
  }
  return status;
}

EXPORT const char *MdsRelease()
{
  return RELEASE;
}

EXPORT struct descriptor *MdsReleaseDsc()
{
  static struct descriptor RELEASE_D = { 0, DTYPE_T, CLASS_S, 0 };
  RELEASE_D.length = (unsigned short)strlen(RELEASE);
  RELEASE_D.pointer = (char *)RELEASE;
  return &RELEASE_D;
}
