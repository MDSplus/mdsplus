#include <config.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void *(*PyGILState_Ensure)()=0;
static void *(*PyGILState_GetThisThreadState)()=0;
static void (*Py_Initialize)()=0;
static void (*PyEval_InitThreads)()=0;
static void (*PyEval_ReleaseThread)(void *)=0;
static void (*PyRun_SimpleString)(char *)=0;
static void (*PyGILState_Release)(void *)=0;

#define loadrtn(name,check) name=dlsym(handle,#name);	\
  if (check && !name) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",#name); \
  PyGILState_Ensure=0;\
  return 0;\
}

int PyCall(char *cmd,int lock) {
  void *GIL;
  if (!PyGILState_Ensure) {
    void *handle;
    char *lib;
    char *envsym=getenv("PyLib");
    if (!envsym) {
      fprintf(stderr,"\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n"
	      "Please define PyLib to be the name of your python library, i.e. 'python2.4 or /usr/lib/libpython2.4.so.1'\n\n\n");
      return 0;
    }
    if (envsym[0]=='/' || strncmp(envsym,"lib",3)==0) {
      lib=strcpy((char *)malloc(strlen(envsym)+1),envsym);
    } else {
      lib=strcpy((char *)malloc(strlen(envsym)+7),"lib");
      strcat(lib,envsym);
      strcat(lib,".so");
    }
    /*** See if python routines are already available ***/
#ifndef HAVE_WINDOWS_H
    handle = dlopen(0,RTLD_NOLOAD);
#endif
    loadrtn(PyGILState_Ensure,0);
    /*** If not, load the python library ***/
    if (!PyGILState_Ensure) {
      handle = dlopen(lib,RTLD_NOW|RTLD_GLOBAL);
      if (!handle) {
	fprintf(stderr,"\n\nUnable to load python library: %s\nError: %s\n\n",lib,dlerror());
	free(lib);
	return 0;
      }
      free(lib);
      loadrtn(PyGILState_Ensure,1);
    }
    loadrtn(PyGILState_GetThisThreadState,1);
    loadrtn(Py_Initialize,1);
    loadrtn(PyEval_InitThreads,1);
    loadrtn(PyEval_ReleaseThread,1);
    loadrtn(PyRun_SimpleString,1);
    loadrtn(PyGILState_Release,1);
    (*Py_Initialize)();
    (*PyEval_InitThreads)();
  }
  if (lock) 
    GIL=(*PyGILState_Ensure)();
  (*PyRun_SimpleString)(cmd);
  if (lock)
    (*PyGILState_Release)(GIL);
  return 1;
}

void PyReleaseThreadLock() {
  if (PyGILState_GetThisThreadState && PyEval_ReleaseThread) {
    void *STATE = (*PyGILState_GetThisThreadState)();
    (*PyEval_ReleaseThread)(STATE);
  }
}

#ifdef MAIN
int main() {
  pycall("print 42");
}
#endif
