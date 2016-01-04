#include <config.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif

#define loadrtn(name,check) name=dlsym(handle,#name);	\
  if (check && !name) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",#name); \
  return 0;\
}
typedef void* PyThreadState;
static PyThreadState *(*PyGILState_Ensure)() = 0;
static void (*PyGILState_Release)(PyThreadState *) = 0;

static void (*PyRun_SimpleString)(char *) = 0;

static int Initialize()
{
  if (!PyGILState_Ensure) {
    void (*Py_Initialize) () = 0;
    void (*PyEval_InitThreads)() = 0;
    int  (*PyEval_ThreadsInitialized)() = 0;
    void *(*PyEval_SaveThread)() = 0;
    void *handle;
    char *lib;
    char *envsym = getenv("PyLib");
    if (!envsym) {
      fprintf(stderr,"\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\nPlease define PyLib to be the name of your python library, i.e. 'python2.4 or /usr/lib/libpython2.4.so.1'\n\n\n");
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
#ifdef RTLD_NOLOAD
    /*** See if python routines are already available ***/
    handle = dlopen(0, RTLD_NOLOAD);
    loadrtn(Py_Initialize, 0);
    /*** If not, load the python library ***/
#endif
    if (!Py_Initialize) {
      handle = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
      if (!handle) {
	fprintf(stderr, "\n\nUnable to load python library: %s\nError: %s\n\n", lib, dlerror());
	free(lib);
	return 0;
      }
      free(lib);
      loadrtn(Py_Initialize, 1);
      (*Py_Initialize) ();
      loadrtn(PyEval_ThreadsInitialized, 1);
      if ((*PyEval_ThreadsInitialized)() == 0) {
        loadrtn(PyEval_InitThreads, 1);
        loadrtn(PyEval_SaveThread, 1);
        (*PyEval_InitThreads) ();
        (*PyEval_SaveThread) ();
      }
    }
    loadrtn(PyGILState_Ensure, 1);
    loadrtn(PyGILState_Release, 1);
    /*** load python functions ***/
    loadrtn(PyRun_SimpleString, 1);
  }
  return 1;
}

EXPORT int PyCall(char *cmd)
{
  int status = 0;
#ifndef _WIN32
  struct sigaction offact = {SIG_DFL, 0, 0, 0, 0};
  struct sigaction oldact;
  sigaction(SIGCHLD, &offact, &oldact);
#endif
  if (Initialize()) {
    PyThreadState *GIL = (*PyGILState_Ensure)();
    (*PyRun_SimpleString) (cmd);
    (*PyGILState_Release) (GIL);
    status = 1;
  }
#ifndef _WIN32
  sigaction(SIGCHLD, &oldact, NULL);
#endif
  return status;
}
