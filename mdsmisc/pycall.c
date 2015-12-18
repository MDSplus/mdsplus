#include <config.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
typedef void* PyThreadState;
static void (*Py_Initialize)() = 0;
static int  (*PyEval_ThreadsInitialized)() = 0;
static void (*PyEval_InitThreads)() = 0;
static PyThreadState *(*PyGILState_Ensure)() = 0;
static void (*PyGILState_Release)() = 0;
static PyThreadState *(*PyEval_SaveThread)() = 0;
static void (*PyEval_RestoreThread)() = 0;

static void (*PyRun_SimpleString) (char *) = 0;

#define loadrtn(name,check) name=dlsym(handle,#name);	\
  if (check && !name) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",#name); \
  PyGILState_Ensure=0;\
  return 0;\
}

EXPORT int PyCall(char *cmd)
{
#ifndef _WIN32
  void (*old_handler) (int);
#endif
  PyThreadState *GIL;
  if (!Py_Initialize) {
    void *handle;
    char *lib;
    char *envsym = getenv("PyLib");
    if (!envsym) {
      fprintf(stderr,
	      "\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n"
	      "Please define PyLib to be the name of your python library, i.e. 'python2.4 or /usr/lib/libpython2.4.so.1'\n\n\n");
      return 0;
    }
#ifdef _WIN32
    lib = strcpy((char *)malloc(strlen(envsym) + 5), envsym);
    strcat(lib, ".dll");
#else
    if (envsym[0] == '/' || strncmp(envsym, "lib", 3) == 0) {
      lib = strcpy((char *)malloc(strlen(envsym) + 1), envsym);
    } else {
      lib = strcpy((char *)malloc(strlen(envsym) + 7), "lib");
      strcat(lib, envsym);
      strcat(lib, ".so");
    }
#endif
#ifndef _WIN32
    handle = dlopen(0, RTLD_NOLOAD);
#endif
    loadrtn(Py_Initialize, 0);
    /*** If not, load the python library ***/
    if (!Py_Initialize) {
      handle = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
      if (!handle) {
	fprintf(stderr, "\n\nUnable to load python library: %s\nError: %s\n\n", lib, dlerror());
	free(lib);
	return 0;
      }
      free(lib);
      loadrtn(Py_Initialize, 1);
    }
    loadrtn(PyEval_ThreadsInitialized, 1);
    loadrtn(PyEval_InitThreads, 1);
    loadrtn(PyGILState_Ensure, 1);
    loadrtn(PyRun_SimpleString, 1);
    loadrtn(PyGILState_Release, 1);
  }
  (*Py_Initialize)();
  if (!(*PyEval_ThreadsInitialized)())
      (*PyEval_InitThreads)();
  GIL = (*PyGILState_Ensure)();
#ifndef _WIN32
  old_handler = signal(SIGCHLD, SIG_DFL);
#endif
  (*PyRun_SimpleString)(cmd);
#ifndef _WIN32
  signal(SIGCHLD, old_handler);
#endif
  (*PyGILState_Release)(GIL);
  return 1;
}
