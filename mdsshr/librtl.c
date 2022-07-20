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
#define _GNU_SOURCE /* glibc2 needs this */
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <math.h>
#include <mdsplus/mdsconfig.h>
#include <signal.h>
#include <socket_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <mdsshr.h>
#include <_mdsshr.h>

// #define DEBUG
#include <mdsmsg.h>

#ifdef _WIN32
#include <process.h>
#define setenv(name, value, overwrite) _putenv_s(name, value)
#define unsetenv(name) _putenv_s(name, "")
#define localtime_r(time, tm) localtime_s(tm, time)
#define ctime_r(tm, buf) ctime_s(buf, 32, tm)
#else
#include <sys/wait.h>
#endif

#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mdsshr_messages.h>
#include <mdstypes.h>
#include <strroutines.h>
#include "mdsthreadstatic.h"

#define LIBRTL_SRC

typedef struct
{
  char *env;
  char *file;
  mdsdsc_t wild_descr;
  char **env_strs;
  int num_env;
  int next_index;
  int next_dir_index; /* index intor env_strs to put the next directory to
                         search */
  DIR *dir_ptr;
} FindFileCtx;

typedef struct _VmList
{
  void *ptr;
  struct _VmList *next;
} VmList;

typedef struct _ZoneList
{
  VmList *vm;
  struct _ZoneList *next;
  pthread_mutex_t lock;
} ZoneList;

typedef struct node
{
  void *left;
  void *right;
  short bal;
} LibTreeNode;
#include <libroutines.h>

#ifndef USE_TM_GMTOFF
/* tzset() sets the global statics daylight and timezone.
 * However, this is not threadsafe as we cannot prevent third party code from
 * calling tzset. Therefore, we make our private copies of timezone and
 * daylight. timezone: constant offset of the local standard time to gmt.
 * daylight: constant flag if region implements daylight saving at all.
 * (seasonal flag is tmval.tm_isdst)
 */
time_t ntimezone_;
int daylight_;
inline static void tzset_()
{
  tzset();
  ntimezone_ = -timezone;
  daylight_ = daylight;
}
#endif

static inline time_t get_tz_offset(time_t *const time)
{
  struct tm tmval;
  localtime_r(time, &tmval);
#ifdef USE_TM_GMTOFF
  return tmval.tm_gmtoff;
#else
  RUN_FUNCTION_ONCE(tzset_);
  return (daylight_ && tmval.tm_isdst) ? ntimezone_ + 3600 : ntimezone_;
#endif
}

static int64_t VMS_TIME_OFFSET = LONG_LONG_CONSTANT(0x7c95674beb4000);

///
/// Waits for the specified time in seconds. Supports fractions of seconds.
///
/// \param secs the address of a constant floating point number representing the
/// time to wait \return 1 if successful, 0 if failed or interrupted.
///
EXPORT int LibWait(const float *const secs)
{
  struct timespec ts;
  ts.tv_sec = (unsigned int)*secs;
  ts.tv_nsec = (unsigned int)((*secs - (float)ts.tv_sec) * 1E9);
  return nanosleep(&ts, 0) == 0;
}

EXPORT char *Now32(char *const buf)
{
  const time_t tim = time(0);
  ctime_r(&tim, buf);
  buf[strlen(buf) - 1] = '\0';
  return buf;
}
///
/// Call a routine in a shared library passing zero or more arguments.
///
/// \param arglist array of void pointers whose length is specified by the first
/// element interpreted as a long integer. A maximum of 32 arguments to the
/// routine are supported. \param routine address of the routine to call \return
/// the value returned by the routine as a void *
///
EXPORT void *LibCallg(void **const a, void *(*const routine)())
{
  if (!routine)
    abort(); // intercept definite stack corruption
  switch (*(int *)a & 0xff)
  {
  case 0:
    return routine();
  case 1:
    return routine(a[1]);
  case 2:
    return routine(a[1], a[2]);
  case 3:
    return routine(a[1], a[2], a[3]);
  case 4:
    return routine(a[1], a[2], a[3], a[4]);
  case 5:
    return routine(a[1], a[2], a[3], a[4], a[5]);
  case 6:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6]);
  case 7:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
  case 8:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
  case 9:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
  case 10:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
  case 11:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11]);
  case 12:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12]);
  case 13:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13]);
  case 14:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14]);
  case 15:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15]);
  case 16:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16]);
  case 17:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17]);
  case 18:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18]);
  case 19:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19]);
  case 20:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20]);
  case 21:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21]);
  case 22:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22]);
  case 23:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23]);
  case 24:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24]);
  case 25:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25]);
  case 26:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26]);
  case 27:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27]);
  case 28:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27], a[28]);
  case 29:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27], a[28], a[29]);
  case 30:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27], a[28], a[29], a[30]);
  case 31:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27], a[28], a[29], a[30], a[31]);
  case 32:
    return routine(a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10],
                   a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18],
                   a[19], a[20], a[21], a[22], a[23], a[24], a[25], a[26],
                   a[27], a[28], a[29], a[30], a[31], a[32]);
  default:
    printf("Error - currently no more than 32 arguments supported on external "
           "calls\n");
  }
  return 0;
}

DEFINE_INITIALIZESOCKETS;
EXPORT int _LibGetHostAddr(const char *name, const char *portstr, struct sockaddr *sin)
{
  INITIALIZESOCKETS;
  struct addrinfo *entry, *info = NULL;
  const struct addrinfo hints = {0, sin->sa_family, 0, 0, 0, NULL, NULL, NULL};
  uint32_t port = 0;
  const char *service = portstr;
  if (portstr && (sin->sa_family == AF_INET || sin->sa_family == AF_INET6))
  {
    port = (uint32_t)strtol(portstr, NULL, 0);
    if (port && port <= 0xFFFF)
    {
      port = htons(port);
      service = NULL;
    }
    else
    {
      port = 0;
    }
  }
  if (!getaddrinfo(name, service, &hints, &info) && info)
  {
    for (entry = info; entry; entry = entry->ai_next)
    {
      if (entry->ai_addr)
      {
        *sin = *entry->ai_addr;
        if (port)
        {
          if (sin->sa_family == AF_INET)
          {
            ((struct sockaddr_in *)sin)->sin_port = port;
          }
          else if (sin->sa_family == AF_INET6)
          {
            ((struct sockaddr_in6 *)sin)->sin6_port = port;
          }
        }
        break;
      }
    }
    freeaddrinfo(info);
    return C_OK;
  }
  return C_ERROR;
}

#ifdef _WIN32

static char *GetRegistry(const HKEY where, const char *const pathname)
{
  HKEY regkey;
  unsigned char *path = NULL;
  if (RegOpenKeyEx(where, "SOFTWARE\\MIT\\MDSplus", 0, KEY_READ, &regkey) ==
      ERROR_SUCCESS)
  {
    unsigned long valtype;
    unsigned long valsize;
    if (RegQueryValueEx(regkey, pathname, 0, &valtype, NULL, &valsize) ==
        ERROR_SUCCESS)
    {
      valsize += 2;
      path = malloc(valsize + 1);
      RegQueryValueEx(regkey, pathname, 0, &valtype, path, &valsize);
    }
    RegCloseKey(regkey);
  }
  return (char *)path;
}

EXPORT int LibSpawn(const mdsdsc_t *const cmd, const int waitFlag,
                    const int notifyFlag __attribute__((unused)))
{
  if (MdsSandboxEnabled())
    return MDSplusSANDBOX;
  char *cmd_c = MdsDescrToCstring(cmd);
  int status;
  void *arglist[255];
  char *tok;
  int numargs = 6;
  tok = strtok(cmd_c, " ");
  arglist[0] = numargs + NULL;
  arglist[1] = (void *)(NULL + (waitFlag ? _P_WAIT : _P_NOWAIT));
  arglist[2] = "cmd";
  arglist[3] = arglist[2];
  arglist[4] = (void *)"/C";
  arglist[5] = (void *)tok;
  while ((tok = strtok(0, " ")) != 0)
  {
    if (strlen(tok) > 0)
    {
      arglist[numargs++] = (void *)tok;
      arglist[0] = numargs + NULL;
    }
  }
  arglist[numargs] = (void *)NULL;
  status = (int)(intptr_t)LibCallg(arglist, (void *)_spawnlp);
  free(cmd_c);
  return status;
}

#else /* _WIN32 */

static char const *nonblank(const char *p)
{
  if (!p)
    return (0);
  for (; *p && isspace(*p); p++)
    ;
  return (*p ? p : 0);
}

static void child_done(int sig)
{
  if (sig == SIGCHLD)
    fprintf(stdout, "--> Process completed\n");
  else
    fprintf(stderr, "--> child_done: *NOTE*  sig=0x%08X\n", sig);

  return;
}

EXPORT int LibSpawn(const mdsdsc_t *const cmd, const int waitFlag,
                    const int notifyFlag)
{
  if (MdsSandboxEnabled())
    return MDSplusSANDBOX;
  char *sh = "/bin/sh";
  pid_t pid, xpid;
  char *cmdstring = MdsDescrToCstring(cmd);
  char *spawn_wrapper = TranslateLogical("MDSPLUS_SPAWN_WRAPPER");
  int sts = 0;
  if (spawn_wrapper)
  {
    char *oldcmdstring = cmdstring;
    cmdstring = strcpy(malloc(strlen(spawn_wrapper) + strlen(oldcmdstring) + 5),
                       spawn_wrapper);
    strcat(cmdstring, " ");
    strcat(cmdstring, oldcmdstring);
    free(oldcmdstring);
    TranslateLogicalFree(spawn_wrapper);
  }
  signal(SIGCHLD, notifyFlag ? child_done : (waitFlag ? SIG_DFL : SIG_IGN));
  pid = fork();
  if (!pid)
  {
    /*-------------> child process: execute cmd	*/
    char const *arglist[4];
    int i = 0;
    if (!waitFlag)
    {
      pid = fork();
      if (pid != -1 && pid != 0)
        _exit(0);
    }
    signal(SIGCHLD, SIG_DFL);
    arglist[0] = getenv("SHELL");
    if (arglist[0] == 0)
      arglist[0] = sh;
    i++;
    if (cmd->length != 0)
    {
      arglist[i++] = "-c";
      arglist[i++] = nonblank(cmdstring);
    }
    arglist[i] = 0;
    sts = execvp(arglist[0], (char *const *)arglist);
  }
  /*-------------> parent process ...		*/
  if (pid == -1)
  {
    fprintf(stderr, "Error %d from fork()\n", errno);
    return (0);
  }
  /*  if (waitflag || cmd->length == 0)
     {
   */
  for (;;)
  {
    xpid = waitpid(pid, &sts, 0);
    if (xpid == pid)
      break;
    else if (xpid == -1)
    {
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

EXPORT char *TranslateLogical(const char *const pathname)
{
  char *tpath = getenv(pathname);
  if (tpath)
    return strdup(tpath);
#ifdef _WIN32
  tpath = GetRegistry(HKEY_CURRENT_USER, pathname);
  if (tpath)
    return strdup(tpath);
  tpath = GetRegistry(HKEY_LOCAL_MACHINE, pathname);
  if (tpath)
    return strdup(tpath);
#endif
  return NULL;
}

EXPORT int TranslateLogicalXd(const mdsdsc_t *const in,
                              mdsdsc_xd_t *const out)
{
  mdsdsc_t out_dsc = {0, DTYPE_T, CLASS_S, 0};
  int status = 0;
  char *in_c = MdsDescrToCstring(in);
  char *out_c = TranslateLogical(in_c);
  if (out_c)
  {
    out_dsc.length = (uint16_t)strlen(out_c);
    out_dsc.pointer = out_c;
    status = 1;
  }
  MdsCopyDxXd(&out_dsc, out);
  if (out_c)
    TranslateLogicalFree(out_c);
  free(in_c);
  return status;
}

EXPORT void MdsFree(void *const ptr) { free(ptr); }

EXPORT void MdsFreeDescriptor(mdsdsc_t *d)
{
  if (d)
  {
    if (d->class == CLASS_XD)
      MdsFree1Dx((mdsdsc_xd_t *)d, NULL);
    else if (d->class == CLASS_D)
      free(d->pointer);
    free(d);
  }
}

EXPORT char *MdsDescrToCstring(const mdsdsc_t *const in)
{
  char *out = malloc((size_t)in->length + 1);
  if (in->length > 0)
    memcpy(out, in->pointer, in->length);
  out[in->length] = 0;
  return out;
}

EXPORT char *LibFindImageSymbolErrString()
{
  MDSTHREADSTATIC_INIT;
  return MDS_FIS_ERROR;
}

static void *load_lib(const char *const dirspec, const char *const filename,
                      char *errorstr)
{
  void *handle = NULL;
  char *full_filename = alloca(strlen(dirspec) + strlen(filename) + 10);
  if (strlen(dirspec) > 0)
  {
    if (strchr(dirspec, '\\'))
    {
      sprintf(full_filename, "%s\\%s", dirspec, filename);
    }
    else
    {
      sprintf(full_filename, "%s/%s", dirspec, filename);
    }
  }
  else
  {
    strcpy(full_filename, filename);
  }
#ifdef RTLD_NOLOAD
  handle = dlopen(full_filename, RTLD_NOLOAD | RTLD_LAZY);
  if (!handle)
#endif
    handle = dlopen(full_filename, RTLD_LAZY);
  if (!handle)
  {
    snprintf(errorstr + strlen(errorstr), 4096 - strlen(errorstr),
             "Error loading %s: %s\n", full_filename, dlerror());
  }
  return handle;
}

EXPORT int LibFindImageSymbol_C(const char *const filename_in,
                                const char *const symbol, void **symbol_value)
{
  int status;
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  MUTEX_LOCK_PUSH(&lock);
  if (*symbol_value) // already loaded
    status = 1;
  else
  {
#ifdef _WIN32
    const char *prefix = "";
    const char delim = ';';
#else
    const char *prefix = "lib";
    const char delim = ':';
#endif
    void *handle = NULL;
    char *errorstr = alloca(4096);
    char *filename = alloca(strlen(filename_in) + strlen(prefix) +
                            strlen(SHARELIB_TYPE) + 1);
    errorstr[0] = '\0';
    strcpy(filename, filename_in);
    if ((!(strchr(filename, '/') || strchr(filename, '\\'))) &&
        (strlen(prefix) > 0) && strncmp(filename_in, prefix, strlen(prefix)))
    {
      sprintf(filename, "%s%s", prefix, filename_in);
    }
    if (strcmp(filename + strlen(filename) - strlen(SHARELIB_TYPE),
               SHARELIB_TYPE))
    {
      strcat(filename, SHARELIB_TYPE);
    }
    handle = load_lib("", filename, errorstr);
    if (handle == NULL && (strchr(filename, '/') == 0) &&
        (strchr(filename, '\\') == 0))
    {
      char *library_path = getenv("MDSPLUS_LIBRARY_PATH");
      if (library_path)
      {
        size_t offset = 0;
        char *libpath = strdup(library_path);
        while (offset < strlen(library_path))
        {
          char *dptr = strchr(libpath + offset, delim);
          if (dptr)
            *dptr = '\0';
          handle = load_lib(libpath + offset, filename, errorstr);
          if (handle)
            break;
          offset = offset + strlen(libpath + offset) + 1;
        }
        free(libpath);
      }
      if ((handle == NULL) && (delim == ':'))
      {
        char *mdir = getenv("MDSPLUS_DIR");
        if (mdir)
        {
          char *libdir = alloca(strlen(mdir) + 10);
          sprintf(libdir, "%s/%s", mdir, "lib");
          handle = load_lib(libdir, filename, errorstr);
        }
      }
    }
    if (handle != NULL)
    {
      *symbol_value = dlsym(handle, symbol);
      if (*symbol_value == NULL)
      {
        snprintf(errorstr + strlen(errorstr), 4096 - strlen(errorstr),
                 "Error: %s\n", dlerror());
      }
    }
    MDSTHREADSTATIC_INIT;
    free(MDS_FIS_ERROR);
    if (*symbol_value == NULL)
    {
      MDS_FIS_ERROR = strdup(errorstr);
      status = LibKEYNOTFOU;
    }
    else
    {
      MDS_FIS_ERROR = NULL;
      status = 1;
    }
  }
  MUTEX_LOCK_POP(&lock);
  return status;
}

EXPORT int LibFindImageSymbol(const mdsdsc_t *const filename,
                              const mdsdsc_t *const symbol,
                              void **const symbol_value)
{
  char *c_filename = MdsDescrToCstring(filename);
  char *c_symbol = MdsDescrToCstring(symbol);
  *symbol_value = NULL; // maintain previous behaviour
  int status = LibFindImageSymbol_C(c_filename, c_symbol, symbol_value);
  free(c_filename);
  free(c_symbol);
  return status;
}

EXPORT int StrConcat(mdsdsc_t *const out, ...)
{
  int i, nargs, len;
  mdsdsc_t *arglist[256];
  VA_LIST_MDS_END_ARG(arglist, nargs, 0, 0, out);
  char *new;
  if (out->class == CLASS_D)
  {
    len = 0;
    for (i = 0; i < nargs && arglist[i] && len < 0xFFFF; i++)
      len += (int)arglist[i]->length;
    if (len > 0xFFFF)
      return StrSTRTOOLON;
    new = malloc(len);
  }
  else if (out->class == CLASS_S)
    new = malloc(len = (int)out->length);
  else
    return LibINVSTRDES;
  // concat the strings
  char *p = new, *e = new + len, *p2, *e2;
  for (i = 0; i < nargs && arglist[i] && p < e; i++)
  {
    p2 = arglist[i]->pointer;
    e2 = arglist[i]->pointer + (int)arglist[i]->length;
    while (p < e && p2 < e2)
      *p++ = *p2++;
  }
  if (out->class == CLASS_S)
  {
    memcpy(out->pointer, new, p - new);
    memset(p, ' ', e - p);
    free(new);
  }
  else
  {
    free(out->pointer);
    out->pointer = new;
    out->length = len;
  }
  return MDSplusSUCCESS;
}

EXPORT int StrPosition(const mdsdsc_t *const source,
                       const mdsdsc_t *const substring,
                       const int *const start)
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

EXPORT int StrCopyR(mdsdsc_t *const out, const length_t *const len,
                    char *const in)
{
  const mdsdsc_t in_d = {*len, DTYPE_T, CLASS_S, in};
  return StrCopyDx(out, &in_d);
}

EXPORT int StrLenExtr(mdsdsc_t *const out, const mdsdsc_t *const in,
                      const int *const start_in, const int *const len_in)
{
  const length_t len = (length_t)((*len_in < 0) ? 0 : *len_in & 0xffff);
  const length_t start =
      (length_t)((*start_in > 1) ? (*start_in & 0xffff) - 1 : 0);
  const length_t span = (in->length > start) ? in->length - start : 0;
  const length_t tspan = span < len ? span : len;
  mdsdsc_t s = {len, DTYPE_T, CLASS_S, malloc(len)};
  if (!s.pointer)
    return LibINSVIRMEM;
  memcpy(s.pointer, in->pointer + start, tspan);
  if (span < len)
    memset(s.pointer + span, ' ', len - span);
  int status = StrCopyDx(out, &s);
  free(s.pointer);
  return status;
}

EXPORT int StrGet1Dx(const length_t *const len, mdsdsc_d_t *const out)
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

int LibSFree1Dd(mdsdsc_d_t *const out) { return StrFree1Dx(out); }

EXPORT int StrTrim(mdsdsc_t *const out, const mdsdsc_t *const in,
                   length_t *const lenout)
{
  mdsdsc_d_t tmp = {0, DTYPE_T, CLASS_D, 0};
  mdsdsc_t s = {0, DTYPE_T, CLASS_S, 0};
  uint16_t i;
  for (i = in->length; i > 0; i--)
    if (in->pointer[i - 1] != ' ' && in->pointer[i - 1] != 9)
      break;
  StrCopyDx((mdsdsc_t *)&tmp, in);
  s.length = i;
  s.pointer = tmp.pointer;
  if (lenout != NULL)
    *lenout = s.length;
  StrCopyDx(out, &s);
  return StrFree1Dx(&tmp);
}

EXPORT int StrCopyDx(mdsdsc_t *const out, const mdsdsc_t *const in)
{
  if (out->class == CLASS_D && (in->length != out->length))
    StrGet1Dx(&in->length, (mdsdsc_d_t *)out);
  if (out->length && out->pointer != NULL)
  {
    const length_t outlength =
        (out->class == CLASS_A) ? ((mdsdsc_a_t *)out)->arsize : out->length;
    const length_t inlength =
        (in->class == CLASS_A) ? ((mdsdsc_a_t *)in)->arsize : in->length;
    const length_t len = outlength > inlength ? inlength : outlength;
    char *p1, *p2;
    uint32_t i;
    for (i = 0, p1 = out->pointer, p2 = in->pointer; i < len; i++)
      *p1++ = *p2++;
    if (outlength > inlength)
      memset(out->pointer + inlength, ' ', outlength - inlength);
  }
  return MDSplusSUCCESS;
}

EXPORT int StrCompare(const mdsdsc_t *const str1, const mdsdsc_t *const str2)
{
  const int len = str1->length < str2->length ? str1->length : str2->length;
  const int ans = strncmp(str1->pointer, str2->pointer, len);
  if (ans)
    return ans;
  if (str1->length > str2->length)
    return (str1->pointer[len] & 0xFF);
  if (str2->length > str1->length)
    return -(str2->pointer[len] & 0xFF);
  return 0;
}

EXPORT int StrCaseBlindCompare(const mdsdsc_t *const str1,
                               const mdsdsc_t *const str2)
{
  const int len = str1->length < str2->length ? str1->length : str2->length;
  const int ans = strncasecmp(str1->pointer, str2->pointer, len);
  if (ans)
    return ans;
  if (str1->length > str2->length)
    return (str1->pointer[len] & 0xFF);
  if (str2->length > str1->length)
    return -(str2->pointer[len] & 0xFF);
  return 0;
}

EXPORT int StrUpcase(mdsdsc_t *const out, const mdsdsc_t *const in)
{
  int status = StrCopyDx(out, in);
  if (STATUS_NOT_OK)
    return status;
  const int outlength =
      (out->class == CLASS_A) ? ((mdsdsc_a_t *)out)->arsize : out->length;
  int i;
  for (i = 0; i < outlength; i++)
    out->pointer[i] = (char)toupper(out->pointer[i]);
  return MDSplusSUCCESS;
}

EXPORT int StrRight(mdsdsc_t *const out, const mdsdsc_t *const in,
                    const length_t *const start)
{
  mdsdsc_d_t tmp = {0, DTYPE_T, CLASS_D, 0};
  mdsdsc_t s = {0, DTYPE_T, CLASS_S, 0};
  StrCopyDx((mdsdsc_t *)&tmp, in);
  s.length = (length_t)((int)in->length - *start + 1);
  s.pointer = tmp.pointer + (*start - 1);
  StrCopyDx(out, &s);
  return StrFree1Dx(&tmp);
}

static pthread_mutex_t zones_lock = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_ZONES MUTEX_LOCK_PUSH(&zones_lock)
#define UNLOCK_ZONES MUTEX_LOCK_POP(&zones_lock)
ZoneList *MdsZones = NULL;
#define LOCK_ZONE(zone) MUTEX_LOCK_PUSH(&(zone)->lock)
#define UNLOCK_ZONE(zone) MUTEX_LOCK_POP(&(zone)->lock)

EXPORT int LibCreateVmZone(ZoneList **const zone)
{
  ZoneList *list;
  *zone = malloc(sizeof(ZoneList));
  (*zone)->vm = NULL;
  (*zone)->next = NULL;
  pthread_mutex_init(&(*zone)->lock, NULL);
  LOCK_ZONES;
  if (MdsZones == NULL)
    MdsZones = *zone;
  else
  {
    for (list = MdsZones; list->next; list = list->next)
      ;
    list->next = *zone;
  }
  UNLOCK_ZONES;
  return (*zone != NULL);
}

EXPORT int LibDeleteVmZone(ZoneList **const zone)
{
  int found;
  ZoneList *list, *prev;
  LibResetVmZone(zone);
  LOCK_ZONES;
  found = 0;
  if (*zone == MdsZones)
  {
    found = 1;
    MdsZones = (*zone)->next;
  }
  else
  {
    for (prev = 0, list = MdsZones; list && list != *zone;
         prev = list, list = list->next)
      ;
    if (list && prev)
    {
      prev->next = list->next;
      found = 1;
    }
  }
  if (found)
  {
    free(*zone);
    *zone = 0;
  }
  UNLOCK_ZONES;
  return found;
}

EXPORT int LibResetVmZone(ZoneList **const zone)
{
  if (zone && *zone)
  {
    VmList *vm, *_vm;
    LOCK_ZONE(*zone);
    vm = (*zone)->vm;
    (*zone)->vm = NULL;
    UNLOCK_ZONE(*zone);
    while (vm)
    {
      free(vm->ptr);
      _vm = vm;
      vm = _vm->next;
      free(_vm);
    }
  }
  return MDSplusSUCCESS;
}

static VmList *ZoneList_remove_VmList(ZoneList **zone, void *vm)
{
  VmList *list;
  LOCK_ZONE(*zone);
  VmList **prev = &(*zone)->vm;
  for (list = (*zone)->vm; list; prev = &list->next, list = list->next)
  {
    if (list->ptr == vm)
    {
      *prev = list->next;
      break;
    }
  }
  UNLOCK_ZONE(*zone);
  return list;
}

static void ZoneList_add_VmList(ZoneList **zone, VmList *list)
{
  LOCK_ZONE(*zone);
  list->next = (*zone)->vm;
  (*zone)->vm = list;
  UNLOCK_ZONE(*zone);
}

EXPORT int LibFreeVm(const uint32_t *len, void **vm, ZoneList **zone)
{
  (void)len;
  if (vm && *vm)
  {
    free(*vm);
    if (zone)
    {
      free(ZoneList_remove_VmList(zone, *vm));
    }
  }
  return MDSplusSUCCESS;
}
EXPORT int libfreevm_(const uint32_t *len, void **vm, ZoneList **zone)
{
  return LibFreeVm(len, vm, zone);
}
EXPORT int libfreevm(const uint32_t *len, void **vm, ZoneList **zone)
{
  return LibFreeVm(len, vm, zone);
}

EXPORT int LibGetVm(const uint32_t *len, void **vm, ZoneList **zone)
{
  *vm = malloc(*len);
  if (*vm == NULL)
  {
    printf("Insufficient virtual memory\n");
    return LibINSVIRMEM;
  }
  if (zone)
  {
    VmList *list = malloc(sizeof(VmList));
    list->ptr = *vm;
    ZoneList_add_VmList(zone, list);
  }
  return MDSplusSUCCESS;
}
EXPORT int libgetvm_(const uint32_t *len, void **vm, ZoneList **zone)
{
  return LibGetVm(len, vm, zone);
}
EXPORT int libgetvm(const uint32_t *len, void **vm, ZoneList **zone)
{
  return LibGetVm(len, vm, zone);
}

#define SEC_PER_DAY (60 * 60 * 24)

EXPORT int LibConvertDateString(const char *asc_time, int64_t *const qtime)
{
  time_t tim = 0;
  char time_out[24];
  int parse_it = 1;
  int ctime_it = 0;

  /* VMS time = unixtime * 10,000,000 + 0x7c95674beb4000q */
  if (asc_time == 0 || strcasecmp(asc_time, "now") == 0)
  {
    tim = time(NULL);
    parse_it = 0;
  }
  else if (strcasecmp(asc_time, "today") == 0)
  {
    tim = time(NULL);
    ctime_it = 1;
  }
  else if (strcasecmp(asc_time, "tomorrow") == 0)
  {
    tim = time(NULL) + SEC_PER_DAY;
    ctime_it = 1;
  }
  else if (strcasecmp(asc_time, "yesterday") == 0)
  {
    tim = time(NULL) - SEC_PER_DAY;
    ctime_it = 1;
  }
  if (parse_it)
  {
    if (ctime_it)
    {
      char time_str[32];
      ctime_r(&tim, time_str);
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
      struct tm tm = {0};
#ifdef _WIN32
      uint32_t day, year, hour, minute, second;
      char month[4];
      char *months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
      if (sscanf(asc_time, "%u-%3s-%u %u:%u:%u", &day, month, &year, &hour,
                 &minute, &second) < 6)
      {
        return 0;
      }
      _strupr(month);
      for (tm.tm_mon = 0; tm.tm_mon < 12; tm.tm_mon++)
      {
        if (strcmp(month, months[tm.tm_mon]) == 0)
        {
          break;
        }
      }
      if ((tm.tm_mon == 12) || (day < 1) || (day > 31) || (year < 1900) ||
          (year > 2200) || (hour > 23) || (minute > 59) || (second > 59))
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

EXPORT int LibTimeToVMSTime(const time_t *const time_in,
                            int64_t *const time_out)
{
  struct timeval tv;
  if (time_in)
  {
    tv.tv_sec  = *time_in;
    tv.tv_usec = 0;
  }
  else
    gettimeofday(&tv, 0);
#ifdef _WIN32
  /* MinGW defines timeval as (long,long) so an explicit cast is needed */
  time_t time_to_use = tv.tv_sec;
  time_t tz_offset = get_tz_offset(&time_to_use);
#else
  time_t tz_offset = get_tz_offset(&tv.tv_sec);
#endif
  *time_out = (int64_t)(tv.tv_sec + tz_offset) * (int64_t)10000000 +
              tv.tv_usec * 10 + VMS_TIME_OFFSET;
  return MDSplusSUCCESS;
}

EXPORT time_t LibCvtTim(const int *const time_in, double *const t)
{
  double t_out;
  time_t bintim = time(&bintim);
  if (time_in)
  {
    int64_t time_local;
    double time_d;
    memcpy(&time_local, time_in, sizeof(time_local));
    time_local = (*(int64_t *)time_in - VMS_TIME_OFFSET);
    if (time_local < 0)
      time_local = 0;
    bintim = time_local / LONG_LONG_CONSTANT(10000000);
    time_d = (double)bintim +
             (double)(time_local % LONG_LONG_CONSTANT(10000000)) * 1E-7;
    time_t tz_offset = get_tz_offset(&bintim);
    t_out = (time_d > 0 ? time_d : 0.0) - (double)tz_offset;
    bintim -= tz_offset;
  }
  else
    bintim = (long)(t_out = (double)time(0));
  if (t != 0)
    *t = t_out;
  return (bintim);
}

EXPORT int LibSysAscTim(length_t *const len, mdsdsc_t *const str,
                        const int *const time_in)
{
  char time_out[24];
  uint16_t slen = sizeof(time_out) - 1;
  time_t bintim = LibCvtTim(time_in, 0);
  int64_t chunks = time_in ? *(int64_t *)time_in % 10000000 : 0;
  char time_str[32];
  time_str[0] = '\0';
  ctime_r(&bintim, time_str);
  if (strlen(time_str) > 18)
  {
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
  }
  else
    strcpy(time_out, "\?\?-\?\?\?-\?\?\?\? \?\?:\?\?:\?\?.\?\?");
  StrCopyR(str, &slen, time_out);
  if (len)
    *len = slen;
  return MDSplusSUCCESS;
}

// int LibGetDvi(int *code, void *dummy1, mdsdsc_t *device, int *ans,
//	      mdsdsc_t *ans_string, int *len)
//{
//  *ans = 132;
//  return 1;
//}

EXPORT int StrAppend(mdsdsc_d_t *const out, const mdsdsc_t *const tail)
{
  if (tail->length > 0 && tail->pointer)
  {
    int len = (int)out->length + (int)tail->length;
    if (len > 0xffff)
      return StrSTRTOOLON;
    char *old = out->pointer;
    out->pointer = realloc(out->pointer, len);
    if (out->pointer)
    {
      memcpy(out->pointer + out->length, tail->pointer, tail->length);
      out->length = len;
    }
    else
    {
      out->pointer = old;
      return LibINSVIRMEM;
    }
  }
  return MDSplusSUCCESS;
}

EXPORT int StrFree1Dx(mdsdsc_d_t *const out)
{
  if (out->class == CLASS_D)
  {
    free(out->pointer);
    out->pointer = NULL;
    out->length = 0;
  }
  return MDSplusSUCCESS;
}

EXPORT int StrFindFirstNotInSet(const mdsdsc_t *const source,
                                const mdsdsc_t *const set)
{
  int ans = 0;
  if (source->length > 0)
  {
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

EXPORT int StrFindFirstInSet(const mdsdsc_t *const source,
                             const mdsdsc_t *const set)
{
  int ans = 0;
  if (source->length > 0)
  {
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

struct bbtree_info
{
  struct node *currentnode;
  char *keyname;
  int (*compare_routine)();
  int (*alloc_routine)();
  struct node *new_node;
  int foundintree;
  int controlflags;
  void *user_context;
};

static int MdsInsertTree();

EXPORT int LibInsertTree(LibTreeNode **const treehead, void *const symbol_ptr,
                         int *const control_flags, int (*const compare_rtn)(),
                         int (*const alloc_rtn)(),
                         LibTreeNode **const blockaddr, void *const user_data)
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

static int MdsInsertTree(struct bbtree_info *const bbtree_ptr)
{

#define currentNode (bbtree_ptr->currentnode)
#define ALLOCATE (*(bbtree_ptr->alloc_routine))
#define left_of(node_ptr) node_ptr->left
#define right_of(node_ptr) node_ptr->right
#define offset_of(node_ptr, offnode_ptr) offnode_ptr

  struct node *save_current;
  int in_balance;
  struct node *down_left;
  struct node *down_right;

  if (currentNode == 0)
  {
    if (!(ALLOCATE(bbtree_ptr->keyname, &save_current,
                   bbtree_ptr->user_context) &
          1))
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
  if ((in_balance = (*(bbtree_ptr->compare_routine))(bbtree_ptr->keyname, currentNode, bbtree_ptr->user_context)) <= 0)
  {
    if ((in_balance == 0) && (!(bbtree_ptr->controlflags & 1)))
    {
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
    else
    {
      currentNode->bal--;
      if (currentNode->bal == 0)
        return MDSplusSUCCESS;
      else
      {
        if (currentNode->bal & 1)
          return MDSplusERROR;
        down_left = left_of(currentNode);
        if (down_left->bal < 0)
        {
          currentNode->left = offset_of(currentNode, right_of(down_left));
          down_left->right = offset_of(down_left, currentNode);
          currentNode->bal = 0;
          currentNode = down_left;
          currentNode->bal = 0;
          return MDSplusSUCCESS;
        }
        else
        {
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
  }
  else
  {
    currentNode = right_of(currentNode);
    in_balance = MdsInsertTree(bbtree_ptr);
    if ((bbtree_ptr->foundintree == 3) || (bbtree_ptr->foundintree == 0))
      return MDSplusSUCCESS;
    down_right = currentNode;
    currentNode = save_current;
    currentNode->right = offset_of(currentNode, down_right);
    if (in_balance)
      return MDSplusSUCCESS;
    else
    {
      currentNode->bal++;
      if (currentNode->bal == 0)
        return MDSplusSUCCESS;
      else
      {
        if (currentNode->bal & 1)
          return MDSplusERROR;
        down_right = right_of(currentNode);
        if (down_right->bal > 0)
        {
          currentNode->right = offset_of(currentNode, left_of(down_right));
          down_right->left = offset_of(down_right, currentNode);
          currentNode->bal = 0;
          currentNode = down_right;
          currentNode->bal = 0;
          return MDSplusSUCCESS;
        }
        else
        {
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

EXPORT int LibLookupTree(LibTreeNode **const treehead, void *const symbol_ptr,
                         int (*const compare_rtn)(),
                         LibTreeNode **const blockaddr)
{
  int ch_result;
  struct node *currentnode = *treehead;
  while (currentnode != 0)
  {
    if ((ch_result = compare_rtn(symbol_ptr, currentnode)) == 0)
    {
      *blockaddr = currentnode;
      return MDSplusSUCCESS;
    }
    else if (ch_result < 0)
      currentnode = left_of(currentnode);
    else
      currentnode = right_of(currentnode);
  }
  return LibKEYNOTFOU;
}

inline static int MdsTraverseTree(int (*const user_rtn)(),
                                  void *const user_data,
                                  struct node *const currentnode)
{
  struct node *right_subtree;
  int status;
  if (currentnode == 0)
    return MDSplusSUCCESS;
  if (left_of(currentnode))
  {
    status = MdsTraverseTree(user_rtn, user_data, left_of(currentnode));
    if (STATUS_NOT_OK)
      return status;
  }
  right_subtree = right_of(currentnode);
  status = user_rtn(currentnode, user_data);
  if (STATUS_NOT_OK)
    return status;
  if (right_subtree)
  {
    status = MdsTraverseTree(user_rtn, user_data, right_subtree);
    if (STATUS_NOT_OK)
      return status;
  }
  return MDSplusSUCCESS;
}
EXPORT int LibTraverseTree(LibTreeNode **treehead, int (*user_rtn)(),
                           void *user_data)
{
  return MdsTraverseTree(user_rtn, user_data, *treehead);
}

static int match_wild(const char *const cand_ptr, const int cand_len,
                      const char *const pat_ptr, const int pat_len)
{
  struct descr
  {
    const char *ptr;
    int length;
  };
  struct descr cand = {cand_ptr, cand_len};
  struct descr scand = {cand_ptr, 0};
  struct descr pat = {pat_ptr, pat_len};
  struct descr spat = pat;
  char pc;
  for (;;)
  {
    if (--pat.length < 0)
    {
      if (cand.length == 0)
        return TRUE;
      else
      {
        if (--scand.length < 0)
          return FALSE;
        else
        {
          scand.ptr++;
          cand = scand;
          pat = spat;
        }
      }
    }
    else
    {
      if ((pc = *pat.ptr++) == '*')
      {
        if (pat.length == 0)
          return TRUE;
        scand = cand;
        spat = pat;
      }
      else
      {
        if (--cand.length < 0)
          return FALSE;
        if (*cand.ptr++ != pc)
        {
          if (pc != '%')
          {
            if (--scand.length < 0)
              return FALSE;
            else
            {
              scand.ptr++;
              cand = scand;
              pat = spat;
            }
          }
        }
      }
    }
  }
  return FALSE;
}

EXPORT int StrMatchWild(const mdsdsc_t *const candidate,
                        const mdsdsc_t *const pattern)
{
  if (match_wild(candidate->pointer, candidate->length, pattern->pointer,
                 pattern->length))
    return StrMATCH;
  return StrNOMATCH;
}

EXPORT int StrElement(mdsdsc_t *const out, const int *const num,
                      const mdsdsc_t *const delim, const mdsdsc_t *const in)
{
  char *in_ptr = in->pointer;
  char *se_ptr = in_ptr + in->length;
  char *e_ptr;
  uint16_t len;
  int cnt;
  int status;
  if (delim->length != 1)
    return StrINVDELIM;
  for (cnt = 0; (cnt < *num) && (in_ptr < se_ptr); in_ptr++)
    if (*in_ptr == *delim->pointer)
      cnt++;
  if (cnt < *num)
    return StrNOELEM;
  for (e_ptr = in_ptr; in_ptr < se_ptr; in_ptr++)
    if (*in_ptr == *delim->pointer)
      break;
  len = (uint16_t)(in_ptr - e_ptr);
  status = StrCopyR(out, &len, e_ptr);
  return status;
}

EXPORT int StrTranslate(mdsdsc_t *const out, const mdsdsc_t *const in,
                        const mdsdsc_t *const tran, mdsdsc_t *const match)
{
  int status = 0;
  if (in->class == CLASS_S || in->class == CLASS_D)
  {
    char *dst = (char *)malloc(in->length);
    int i;
    for (i = 0; i < in->length; i++)
    {
      int j;
      int next = 1;
      for (j = 0; next && j < match->length; j += next)
        next = (match->pointer[j] != in->pointer[i]) ? 1 : 0;
      dst[i] = (char)(next ? in->pointer[i]
                           : ((j < tran->length) ? tran->pointer[j] : ' '));
    }
    status = StrCopyR(out, &in->length, dst);
    free(dst);
  }
  else if ((in->class == CLASS_A) && (out->class == CLASS_A) &&
           (in->length > 0) && (out->length > 0) &&
           (((mdsdsc_a_t *)in)->arsize / in->length ==
            ((mdsdsc_a_t *)out)->arsize / out->length))
  {
    mdsdsc_t outdsc = {0, DTYPE_T, CLASS_S, 0};
    mdsdsc_t indsc = {0, DTYPE_T, CLASS_S, 0};
    uint32_t num = ((mdsdsc_a_t *)in)->arsize / in->length;
    uint32_t i;
    outdsc.length = out->length;
    outdsc.pointer = out->pointer;
    indsc.length = in->length;
    indsc.pointer = in->pointer;
    for (i = 0; i < num;
         i++, outdsc.pointer += outdsc.length, indsc.pointer += indsc.length)
      status = StrTranslate(&outdsc, &indsc, tran, match);
  }
  return status;
}

EXPORT int StrReplace(mdsdsc_t *const out, const mdsdsc_t *const in,
                      const int *const start_idx, const int *const end_idx,
                      const mdsdsc_t *const rep)
{
  int status;
  int start;
  int end;
  uint16_t dstlen;
  char *dst;
  char *sptr;
  char *dptr;
#define minmax(l, x, u) (((x) < (l)) ? (l) : (((x) > (u)) ? (u) : (x)))
  start = minmax(1, *start_idx, in->length);
  end = minmax(1, *end_idx, in->length);
  dstlen = (uint16_t)(start - 1 + rep->length + in->length - end + 1);
  dst = (char *)malloc(dstlen);
  for (sptr = in->pointer, dptr = dst; (dptr - dst) < (start - 1);
       *dptr++ = *sptr++)
    ;
  for (sptr = rep->pointer; (sptr - rep->pointer) < rep->length;
       *dptr++ = *sptr++)
    ;
  for (sptr = in->pointer + end; (sptr - in->pointer) < in->length;
       *dptr++ = *sptr++)
    ;
  status = StrCopyR(out, &dstlen, dst);
  free(dst);
  return status;
}

////////////////////////////////////////////////////////////////////////////////
/////FIND FILE//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define DIR_HND HANDLE
#define FILE_INFO WIN32_FIND_DATAA
#else
#define DIR_HND DIR
#define FILE_INFO struct dirent *
#endif

typedef struct
{
  DIR_HND *h;
  size_t wlen;
} findstack_t;
typedef struct
{
  findstack_t *stack;
  int cur_stack;
  int max_stack;
  const char *cptr;
  const char *ptr;
  const char *folders;
  const char *filename;
  const size_t flen;
  char *buffer;
#ifndef _WIN32
  size_t buflen;
#endif
  int recursive;
  int case_blind;
  FILE_INFO fd; // hold the file information
} ctx_t;

#ifdef _WIN32

#define SEP '\\'
#define FILENAME(ctx) ctx->fd.cFileName
#define ISDIRECTORY(ctx) (ctx->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
#define REALLOCBUF(ctx, extra)
static inline void FINDFILECLOSE_OS(const ctx_t *const ctx)
{
  FindClose(ctx->stack[ctx->cur_stack].h);
}
static inline int FINDFILENEXT_OS(ctx_t *const ctx)
{
  return FindNextFile(ctx->stack[ctx->cur_stack].h, &ctx->fd);
}
static inline int FINDFILEFIRST_OS(ctx_t *const ctx)
{
  ctx->stack[ctx->cur_stack].h = FindFirstFile(ctx->buffer, &ctx->fd);
  return ctx->stack[ctx->cur_stack].h != INVALID_HANDLE_VALUE;
}

#else

#define SEP '/'
#define INVALID_HANDLE_VALUE NULL
#define MAX_PATH 1 // at least fit empty string
#define FILENAME(ctx) ctx->fd->d_name
static inline void REALLOCBUF(ctx_t *const ctx, const size_t extra)
{
  const size_t required = ctx->stack[ctx->cur_stack].wlen + extra;
  if (ctx->buflen >= required)
    return;
  char *newbuf = realloc(ctx->buffer, required);
  if (newbuf)
  {
    ctx->buffer = newbuf;
    ctx->buflen = required;
  }
}
#include <sys/stat.h>
static inline int ISDIRECTORY(const ctx_t *const ctx)
{
  struct stat statbuf;
  if (stat(ctx->buffer, &statbuf) != 0)
    return FALSE;
  return S_ISDIR(statbuf.st_mode);
}
static inline void FINDFILECLOSE_OS(const ctx_t *const ctx)
{
  closedir(ctx->stack[ctx->cur_stack].h);
}
static inline int FINDFILENEXT_OS(ctx_t *const ctx)
{
  return (ctx->fd = readdir(ctx->stack[ctx->cur_stack].h)) != NULL;
}
static inline int FINDFILEFIRST_OS(ctx_t *const ctx)
{
  ctx->stack[ctx->cur_stack].h = opendir(ctx->buffer);
  if (!ctx->stack[ctx->cur_stack].h)
  {
    ctx->cur_stack--;
    return FALSE;
  }
  if (!FINDFILENEXT_OS(ctx))
  {
    FINDFILECLOSE_OS(ctx);
    ctx->cur_stack--;
    return FALSE;
  }
  return TRUE;
}

#endif

static int findfileloopstart(ctx_t *const ctx)
{
  ctx->buffer[ctx->stack[ctx->cur_stack].wlen++] = SEP;
#ifdef _WIN32
  // after this wlen will mark the last SEP so we can simply attach the filename
  if (ctx->recursive)
  { // append "\\*\0"
    ctx->buffer[ctx->stack[ctx->cur_stack].wlen] = '*';
    ctx->buffer[ctx->stack[ctx->cur_stack].wlen + 1] = '\0';
  }
  else
  { // append "/<pattern>\0"
    // Windows will find he file much faster
    char *p;
    memcpy(p = ctx->buffer + ctx->stack[ctx->cur_stack].wlen, ctx->filename,
           ctx->flen + 1);
    // replace single char wildcard '%' with '?'
    for (; *p; p++)
      if (*p == '%')
        *p = '?';
  }
#else
  ctx->buffer[ctx->stack[ctx->cur_stack].wlen] = '\0';
#endif
  return FINDFILEFIRST_OS(ctx);
}

static size_t findfileloop(ctx_t *const ctx)
{
  if (ctx->stack[ctx->cur_stack].h == INVALID_HANDLE_VALUE)
  {
    if (!findfileloopstart(ctx))
      return 0;
  }
  else if (!FINDFILENEXT_OS(ctx))
    goto close;
  do
  {
    if (strcmp(FILENAME(ctx), ".") == 0 || strcmp(FILENAME(ctx), "..") == 0)
      continue;
    size_t flen = strlen(FILENAME(ctx));
    REALLOCBUF(ctx,
               flen + 2); // +2 for "/\0" - would be +3 for win but MAX_PATH
    memcpy(ctx->buffer + ctx->stack[ctx->cur_stack].wlen, FILENAME(ctx),
           flen + 1);
    if (ctx->case_blind)
    {
      char *p;
      for (p = FILENAME(ctx); *p; p++)
        *p = toupper(*p);
    }
    if (match_wild(FILENAME(ctx), flen, ctx->filename, ctx->flen))
      return ctx->stack[ctx->cur_stack].wlen + flen;
    if (ctx->recursive && ISDIRECTORY(ctx))
    {
      // MDSDBG("path = %s", ctx->buffer);
      if (++ctx->cur_stack == ctx->max_stack)
      {
        MDSDBG("max_stack increased = %d", ctx->max_stack);
        findstack_t *old = ctx->stack;
        ctx->max_stack *= 2;
        ctx->stack = malloc(sizeof(findstack_t) * ctx->max_stack);
        memcpy(ctx->stack, old, sizeof(findstack_t) * ctx->cur_stack);
        free(old);
      }
      ctx->stack[ctx->cur_stack].h = INVALID_HANDLE_VALUE; // reset handle
      ctx->stack[ctx->cur_stack].wlen =
          ctx->stack[ctx->cur_stack - 1].wlen + flen;
      const size_t len = findfileloop(ctx);
      if (len > 0)
        return len;
    }
  } while (FINDFILENEXT_OS(ctx));
close:
  FINDFILECLOSE_OS(ctx);
  ctx->cur_stack--;
  return 0;
}

static inline void *_findfilestart(const char *const envname,
                                   const char *const filename,
                                   const int recursive, const int case_blind)
{
  MDSDBG("looking for '%s' in '%s'", filename, envname);
  ctx_t *ctx = (ctx_t *)malloc(sizeof(ctx_t));
  ctx->max_stack = recursive ? 8 : 1;
  ctx->stack = malloc(ctx->max_stack * sizeof(findstack_t));
  ctx->cur_stack = -1;
  char *folders;
  if (envname && (folders = getenv(envname)))
  {
    ctx->folders = strdup(folders);
    *(size_t *)&ctx->flen = strlen(filename);
    ctx->filename = memcpy(malloc(ctx->flen + 1), filename, ctx->flen + 1);
  }
  else
  {
    const size_t tlen = strlen(filename);
    char *tmp = memcpy(malloc(tlen + 1), filename, tlen + 1);
#ifdef _WIN32
    char *sep; // replace '/' with '\\'
    for (sep = tmp + tlen; sep-- > tmp;)
      if (*sep == '/')
        *sep = '\\';
#endif
    char *p, *c;
    for (c = tmp, p = NULL; (c = strchr(c, SEP)); p = c, c = p + 1)
      ;
    if (p)
    { // look in specified path
      *(char *)p = '\0';
      const size_t flen = p - tmp + 1;
      *(size_t *)&ctx->flen = tlen - flen;
      ctx->filename = strdup(p + 1);
      ctx->folders = realloc(tmp, flen);
    }
    else
    { // look in current folder only
      *(size_t *)&ctx->flen = tlen;
      ctx->filename = tmp;
      ctx->folders = calloc(1, 1);
    }
  }
  ctx->buffer = malloc(MAX_PATH);
#ifndef _WIN32
  ctx->buflen = MAX_PATH;
#endif
  ctx->recursive = recursive;
  ctx->case_blind = case_blind;
  ctx->cptr = ctx->ptr = ctx->folders;
  if (ctx->case_blind)
  {
    char *p;
    for (p = (char *)ctx->filename; *p; p++)
      *p = toupper(*p);
  }
  return ctx;
}

static inline void *findfilestart(const char *const filename,
                                  const int recursive, const int case_blind)
{
  char *env;
  const char *colon = strchr(filename, ':');
  if (colon)
  {
    size_t envlen = (colon++ - filename);
    env = memcpy(malloc(envlen + 1), filename, envlen);
    env[envlen] = '\0';
  }
  else
  {
    env = NULL;
    colon = filename;
  }
  void *ctx;
  if (strlen(colon) == 0)
    ctx = NULL;
  else
    ctx = _findfilestart(env, colon, recursive, case_blind);
  free(env);
  return ctx;
}

static inline char *findfilenext(ctx_t *const ctx)
{
  if (ctx->cur_stack >= 0)
    do
    {
      if (findfileloop(ctx) > 0)
        return ctx->buffer;
    } while (ctx->cur_stack >= 0);
  if (ctx->cur_stack != -1)
    fprintf(stderr, "ctx_stack = %d != -1\n", ctx->cur_stack);
  while (ctx->cptr)
  {
    const size_t wlen = ((ctx->cptr = strchr(ctx->ptr, ';')) == NULL)
                            ? (int)strlen(ctx->ptr)
                            : (int)(ctx->cptr - ctx->ptr);
    if (wlen == 0)
    {
      ctx->ptr = ctx->cptr + 1; // set ptr to start of next path
      continue;                 // if path empty, skip
    }
    // start search in first folder
    ctx->stack[ctx->cur_stack = 0].h = INVALID_HANDLE_VALUE;
    ctx->stack[ctx->cur_stack].wlen = wlen;
    REALLOCBUF(ctx, 3); // +3 for "./\0" - would be 4 for win but MAX_PATH
    memcpy(ctx->buffer, ctx->ptr, wlen);
    ctx->ptr = ctx->cptr + 1; // set ptr to start of next path
#ifdef _WIN32
    char *sep; // replace '/' with '\\'
    for (sep = ctx->buffer + wlen; sep-- > ctx->buffer;)
      if (*sep == '/')
        *sep = '\\';
#endif
    // allow current path , i.e. wlen = 0
    if (ctx->stack[ctx->cur_stack].wlen == 0)
      ctx->buffer[ctx->stack[ctx->cur_stack].wlen++] = '.';
    else
    {
      // ensure buffer is not terminated by SEP
      while (ctx->stack[ctx->cur_stack].wlen > 0 &&
             ctx->buffer[ctx->stack[ctx->cur_stack].wlen - 1] == SEP)
        ctx->stack[ctx->cur_stack].wlen--;
    }
    // ctx->buffer can enter findfileloop w/o \0 termination
    if (findfileloop(ctx) > 0)
      return ctx->buffer;
  }
  ctx->buffer[0] = '\0';
  return NULL;
}

static inline void findfileend(void *const ctx_i)
{
  if (!ctx_i)
    return;
  ctx_t *ctx = (ctx_t *)ctx_i;
  while (ctx->cur_stack >= 0)
  {
    FINDFILECLOSE_OS(ctx);
    ctx->cur_stack--;
  }
  free((void *)ctx->filename);
  free((void *)ctx->folders);
  free(ctx->stack);
  free(ctx->buffer);
  free(ctx);
}

EXPORT extern int LibFindFileEnd(void **const ctx)
{
  findfileend(*ctx);
  *ctx = NULL;
  return MDSplusSUCCESS;
}

static int find_file(const mdsdsc_t *const filespec, mdsdsc_t *const result,
                     void **const ctx, const int recursively,
                     int const case_blind)
{
#ifdef DEBUG
  clock_t start = clock();
#endif
  int status;
  if (*ctx == 0)
  {
    char *fspec = malloc(filespec->length + 1);
    memcpy(fspec, filespec->pointer, filespec->length);
    fspec[filespec->length] = '\0';
    *ctx = (void *)findfilestart(fspec, recursively, case_blind);
#ifdef DEBUG
    fprintf(stderr, "locking for %s: ", fspec);
#endif
    free(fspec);
    if (!*ctx)
      return MDSplusERROR;
  }
  char *ans = findfilenext(*(ctx_t **)ctx);
  if (ans)
  {
    mdsdsc_t ansd = {strlen(ans), DTYPE_T, CLASS_S, ans};
    StrCopyDx(result, &ansd);
    status = MDSplusSUCCESS;
  }
  else
  {
    status = MDSplusERROR;
    LibFindFileEnd(ctx);
  }
#ifdef DEBUG
  fprintf(stderr, "took %fs: %s\n",
          ((double)(clock() - start)) / CLOCKS_PER_SEC, ans);
#endif
  return status;
}

EXPORT int LibFindFile(const mdsdsc_t *const filespec, mdsdsc_t *const result,
                       void **const ctx)
{
  return find_file(filespec, result, ctx, 0, 0);
}

EXPORT int LibFindFileRecurseCaseBlind(const mdsdsc_t *const filespec,
                                       mdsdsc_t *const result,
                                       void **const ctx)
{
  return find_file(filespec, result, ctx, 1, 1);
}

EXPORT int LibFindFileCaseBlind(const mdsdsc_t *const filespec,
                                mdsdsc_t *const result, void **const ctx)
{
  return find_file(filespec, result, ctx, 0, 1);
}

EXPORT void TranslateLogicalFree(char *const value) { free(value); }

EXPORT int MdsPutEnv(const char *const cmd)
{
  /* cmd		action
   * name		unset name
   * name=	set name to ""
   * name=value   set name to value
   */
  INIT_STATUS_ERROR;
  if (cmd != NULL)
  {
    if (!strstr(cmd, "MDSPLUS_SPAWN_WRAPPER") &&
        !strstr(cmd, "MDSPLUS_LIBCALL_WRAPPER"))
    {
      char *value, *name = strdup(cmd);
      for (value = name; *value && *value != '='; value++)
        ; // find =
      if (*value)
      {
        *(value++) = '\0';
        MDSDBG("setenv %s=%s", name, value);
        status = setenv(name, value, 1);
      }
      else
      {
        MDSDBG("unsetenv %s", name);
        status = unsetenv(name);
      }
      status = status ? MDSplusERROR : MDSplusSUCCESS;
      free(name);
    }
  }
  return status;
}

EXPORT int libffs(const int *const position, const int *const size,
                  const char *const base, int *const find_position)
{
  INIT_STATUS_ERROR;
  int i;
  int *bits = (int *)(base + (*position) / 8);
  int top_bit_to_check = ((*size) + *position) - ((*position) / 8) * 8;
  for (i = (*position) % 8; i < top_bit_to_check; i++)
  {
    if (*bits & (1 << i))
    {
      *find_position = ((*position) / 8) * 8 + i;
      status = MDSplusSUCCESS;
      break;
    }
  }
  return status;
}
