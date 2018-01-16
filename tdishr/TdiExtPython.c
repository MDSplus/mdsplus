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

#include <mdsplus/mdsconfig.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread_port.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <tdishr_messages.h>
#include <libroutines.h>
#include <strroutines.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#define Py_None _Py_NoneStruct

#define loadrtn(name,check) name=dlsym(handle,#name); \
  if (check && !name) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",#name); \
  return LibNOTFOU; \
}
//"

typedef void* PyThreadState;
static PyThreadState *(*PyGILState_Ensure)() = NULL;
static void (*PyGILState_Release)(PyThreadState *) = NULL;

typedef void* PyObject;
typedef ssize_t Py_ssize_t;
static void (*Py_DecRef)() = NULL;//getFunction,argsToTuple,getAnswer
static PyObject *(*PyTuple_New) () = NULL;//argsToTuple
static PyObject *(*PyString_FromString) () = NULL;//->addToPath
static PyObject *(*PyUnicode_FromString) () = NULL;//->addToPath
static void (*PyTuple_SetItem) () = NULL;//argsToTuple
static PyObject *(*PyObject_CallObject) () = NULL;//TdiExtPython
static PyObject *(*PyObject_GetAttrString) () = NULL;//getFunction,getAnswer
static void *(*PyLong_AsVoidPtr) () = NULL;//getAnswer
static PyObject *(*PyErr_Occurred) () = NULL;//getFunction
static void (*PyErr_Print) () = NULL;//getFunction
static PyObject *(*PyImport_ImportModule) () = NULL;//getFunction
static PyObject *_Py_NoneStruct;//TdiExtPython
static PyObject *(*PyList_Insert) () = NULL;//addToPath
static PyObject *(*PyObject_CallFunction) () = NULL;//argsToTuple,getAnswer
static PyObject *(*PySys_GetObject) () = NULL;//addToPath
static int64_t (*PyLong_AsLong) () = NULL;//TdiExtPython
static char *(*PyString_AsString) () = NULL;//addToPath
static char *(*PyBytes_AsString)() = NULL;//addToPath
static PyObject *(*PyUnicode_AsEncodedString)() = NULL;//addToPath
static Py_ssize_t(*PyList_Size) () = NULL;//addToPath
static int (*PyCallable_Check) () = NULL;//getFunction
static PyObject *(*PyList_GetItem) () = NULL;//addToPath
static PyObject *(*PyObject_Str) () = NULL;//addToPath
static int (*PyObject_IsSubclass) () = NULL;//TdiExtPython

static int Initialize(){
  if (PyGILState_Ensure)
    return MDSplusSUCCESS;
  else {
    void (*Py_Initialize) () = NULL;
    void (*PyEval_InitThreads)() = NULL;
    int  (*PyEval_ThreadsInitialized)() = NULL;
    void *(*PyEval_SaveThread)() = NULL;
    void *handle;
    char *lib;
    char *envsym = getenv("PyLib");
    if (!envsym) {
#ifdef _WIN32
      envsym = "python27";
      const char * aspath = "C:\\Python27\\python27.dll";
      _putenv_s("PyLib",envsym);
#else
      envsym = "python2.7";
      const char * aspath = "/usr/lib/python2.7.so.1";
      setenv("PyLib",envsym,B_FALSE);
#endif
      fprintf(stderr,"\nYou should defined the PyLib environment variable!\nPlease define PyLib to be the name of your python library, i.e. '%s' or '%s'.\nWe will try '%s' as default.\n\n",envsym,aspath,envsym);
    }
#ifdef RTLD_NOLOAD
    /*** See if python routines are already available ***/
    handle = dlopen(0, RTLD_NOLOAD);
    loadrtn(Py_Initialize, 0);
    /*** If not, load the python library ***/
#endif
    if (!Py_Initialize) {
#ifdef _WIN32
      if (strlen(envsym)>6 && (envsym[1] == ':' || strncmp(envsym+strlen(envsym)-4, ".dll", 4) == 0)) {
        lib = strcpy((char *)malloc(strlen(envsym) + 1), envsym);
      } else {
        lib = strcpy((char *)malloc(strlen(envsym) + 5), envsym);
        strcat(lib, ".dll");
      }
#else
      if (strlen(envsym)>6 && (envsym[0] == '/' || strncmp(envsym, "lib", 3) == 0)) {
        lib = strcpy((char *)malloc(strlen(envsym) + 1), envsym);
      } else {
        lib = strcpy((char *)malloc(strlen(envsym) + 7), "lib");
        strcat(lib, envsym);
        strcat(lib, ".so");
      }
#endif
      handle = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
      if (!handle) {
        fprintf(stderr, "\n\nUnable to load python library: %s\nError: %s\n\n", lib, dlerror());
        free(lib);
        return MDSplusERROR;
      }
      free(lib);
      loadrtn(Py_Initialize, 1);
      (*Py_Initialize) ();
      loadrtn(PyEval_ThreadsInitialized, 1);
      if ((*PyEval_ThreadsInitialized)() == C_OK) {
        loadrtn(PyEval_InitThreads, 1);
        loadrtn(PyEval_SaveThread, 1);
        (*PyEval_InitThreads) ();
        (*PyEval_SaveThread) ();
      }
    }
    loadrtn(PyGILState_Ensure, 1);
    loadrtn(PyGILState_Release, 1);
    /*** load python functions ***/
    loadrtn(Py_DecRef, 1);
    loadrtn(PyTuple_New, 1);
    loadrtn(PyString_FromString, 0);
    if (!PyString_FromString) {
      loadrtn(PyUnicode_FromString, 1);
    }
    loadrtn(PyTuple_SetItem, 1);
    loadrtn(PyObject_CallObject, 1);
    loadrtn(PyObject_GetAttrString, 1);
    loadrtn(PyLong_AsVoidPtr, 1);
    loadrtn(PyErr_Occurred, 1);
    loadrtn(PyErr_Print, 1);
    loadrtn(PyImport_ImportModule, 1);
    loadrtn(_Py_NoneStruct, 1);
    loadrtn(PyList_Insert, 1);
    loadrtn(PyObject_CallFunction, 1);
    loadrtn(PySys_GetObject, 1);
    loadrtn(PyLong_AsLong, 1);
    loadrtn(PyString_AsString, 0);
    if (!PyString_AsString) {
      loadrtn(PyUnicode_AsEncodedString, 1);
      loadrtn(PyBytes_AsString, 1);
    }
    loadrtn(PyList_Size, 1);
    loadrtn(PyCallable_Check, 1);
    loadrtn(PyList_GetItem, 1);
    loadrtn(PyObject_Str,1);
    loadrtn(PyObject_IsSubclass,1);
    return MDSplusSUCCESS;
  }
}

static char *getStringFromPyObj(PyObject *obj) {
  return PyString_AsString ? (*PyString_AsString)(obj) :
        (*PyBytes_AsString)((*PyUnicode_AsEncodedString)(obj, "utf-8",""));
}

static PyObject *pyObjFromString(char *str) {
  return PyString_FromString ? (*PyString_FromString)(str) : (*PyUnicode_FromString)(str);
}

static PyObject *getFunction(char *modulename, char *functionname)
{
  /* Given a directory path and a function name import a module with the same name as the
     function and find the function in that module with that name. Return null if there
     if an error.
   */
  PyObject *module;
  PyObject *ans = NULL;
  module = (*PyImport_ImportModule)(modulename);
  if (!module) {
    printf("Error importing module %s\n", modulename);
    if ((*PyErr_Occurred)()) {
      PyErr_Print();
    }
  } else {
    ans = (*PyObject_GetAttrString)(module, functionname);
    if (!ans) {
      printf("Error finding function called '%s' in module %s\n", functionname, modulename);
      if ((*PyErr_Occurred)()) {
        (*PyErr_Print)();
      }
    } else {
      if (!(*PyCallable_Check)(ans)) {
        printf("Error, item called '%s' in module %s is not callable\n", functionname, modulename);
        (*Py_DecRef)(ans);
        ans = NULL;
      }
    }
    (*Py_DecRef)(module);
  }
  return ans;
}

static void addToPath(char *dirspec)
{
  /* Add a directory to sys.path if it is not already in the path.
   */
  Py_ssize_t idx, listlen;
  PyObject *sys_path;
  PyObject *path;
  int found = B_FALSE;
  sys_path = (*PySys_GetObject)("path");
  listlen = (*PyList_Size)(sys_path);
  for (idx = 0; idx < listlen && (found == B_FALSE); idx++) {
    PyObject *pathPart;
    pathPart = (*PyList_GetItem)(sys_path, idx);
    if (strcmp(getStringFromPyObj((*PyObject_Str)(pathPart)), dirspec) == 0)
      found = B_TRUE;
  }
  if (found == B_FALSE) {
    path = pyObjFromString(dirspec);
    (*PyList_Insert)(sys_path, (Py_ssize_t) 0, path);
  }
}

char *findModule(struct descriptor *modname_d, char **modname_out)
{
  /* Look for at python module in MDS_PATH. */
  static char *mpath = "MDS_PATH:";
  static char *ftype = ".py";
  char *modname = MdsDescrToCstring(modname_d);
  size_t modlen = strlen(modname);
  char *moduleFile = alloca(strlen(modname) + strlen(mpath) + strlen(ftype) + 1);
  sprintf(moduleFile, "%s%s%s", mpath, modname, ftype);
  free(modname);
  struct descriptor modfile_d = { strlen(moduleFile), DTYPE_T, CLASS_S, moduleFile };
  struct descriptor ans_d = { 0, DTYPE_T, CLASS_D, 0 };
  void *ctx = 0;
  INIT_STATUS;
  status = LibFindFileRecurseCaseBlind(&modfile_d, &ans_d, &ctx);
  LibFindFileEnd(&ctx);
  char *dirspec = NULL;
  if STATUS_OK {
    dirspec = MdsDescrToCstring((struct descriptor *)&ans_d);
    char *modnam = (char *)malloc(modlen + 1);
    strncpy(modnam, &dirspec[strlen(dirspec) - modlen - strlen(ftype)], modlen);
    modnam[modlen] = 0;
    *modname_out = modnam;
    dirspec[strlen(dirspec) - modlen - strlen(ftype)] = 0;
  }
  free(ans_d.pointer);
  return dirspec;
}

static PyObject *argsToTuple(int nargs, struct descriptor **args)
{
  /* Convert descriptor argument list to a tuple of python objects. */
  int idx = 0;
  PyObject *ans = (*PyTuple_New)(nargs);
  PyObject *pointerToObject = getFunction("MDSplus", "pointerToObject");
  if (pointerToObject) {
    for (idx = 0; idx < nargs; idx++) {
      PyObject *arg =
        (*PyObject_CallFunction)(pointerToObject, (sizeof(void *) == 8) ? "L" : "l", args[idx]);
      if (arg) {
        (*PyTuple_SetItem)(ans, idx, arg);
      } else {
        if ((*PyErr_Occurred)()) {
          (*PyErr_Print)();
        }
        break;
      }
    }
    (*Py_DecRef)(pointerToObject);
  }
  if (idx != nargs) {
    (*Py_DecRef)(ans);
    ans = (*PyTuple_New)(0);
  }
  return ans;
}

static void getAnswer(PyObject * value, struct descriptor_xd *outptr)
{
  PyObject *dataObj = NULL;
  PyObject *makeDataFunction = getFunction("MDSplus", "makeData");
  if (makeDataFunction) {
    dataObj = (*PyObject_CallFunction)(makeDataFunction, "O", value);
    (*Py_DecRef)(makeDataFunction);
  }
  if (dataObj) {
    PyObject *descr = (*PyObject_GetAttrString)(dataObj, "descriptor");
    if (descr) {
      PyObject* descrPtr = NULL;
      if (descr!=Py_None){
        descrPtr = (*PyObject_GetAttrString)(descr, "addressof");
        if (descrPtr) {
          MdsCopyDxXd((struct descriptor *)(*PyLong_AsVoidPtr)(descrPtr), outptr);
          (*Py_DecRef)(descrPtr);
        } else {
          printf("Error getting address of descriptor\n");
          if ((*PyErr_Occurred)()) {
            (*PyErr_Print)();
          }
        }
      }
      (*Py_DecRef)(descr);
    } else {
      printf("Error getting descriptor\n");
      if ((*PyErr_Occurred)()) {
        (*PyErr_Print)();
      }
    }
    (*Py_DecRef)(dataObj);
  } else {
    printf("Error converting answer to MDSplus datatype\n");
    if ((*PyErr_Occurred)()) {
      (*PyErr_Print)();
    }
  }
}

int TdiExtPython2(char *dirspec,char *filename,
                 int nargs, struct descriptor **args, struct descriptor_xd *out_ptr){
  /* Try to locate a python module in the MDS_PATH search list and if found execute a function with the same name
     as the module in that module passing the arguments and get the answer back from python. */
  INIT_STATUS;
#ifndef _WIN32
  struct sigaction offact;
  struct sigaction oldact;
  memset(&offact,0,sizeof(offact));
  offact.sa_handler=SIG_DFL;
  sigaction(SIGCHLD, &offact, &oldact);
#endif
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lock);
    pthread_cleanup_push((void*)pthread_mutex_unlock,&lock);
    status = Initialize();
    pthread_cleanup_pop(1);
    if STATUS_OK {
      PyThreadState *GIL = (*PyGILState_Ensure)();
      PyObject *ans;
      PyObject *pyFunction;
      PyObject *pyArgs;
      addToPath(dirspec);
      pyFunction = getFunction(filename, filename);
      if (pyFunction) {
        pyArgs = argsToTuple(nargs, args);
        ans = (*PyObject_CallObject)(pyFunction, pyArgs);
        if (!ans) {
          PyObject* exc = PyErr_Occurred();
          if (exc) {
            PyObject *MDSplusException = getFunction("MDSplus", "MDSplusException");
            if (MDSplusException && PyObject_IsSubclass(exc,MDSplusException)) {
              PyObject *status_obj = (*PyObject_GetAttrString)(exc, "status");
              status = (int)(*PyLong_AsLong)(status_obj);
              (*Py_DecRef)(status_obj);
              if STATUS_NOT_OK {
                char *fac_out=NULL, *msgnam_out=NULL, *text_out=NULL;
                const char *f = "WSEIF???";
                MdsGetStdMsg(status,(const char **)&fac_out,(const char **)&msgnam_out,(const char **)&text_out);
                printf("%%%s-%c-%s: %s\n",fac_out,f[status&7],msgnam_out,text_out);
               }
            } else {
              fprintf(stderr,"Error calling fun in %s\n", filename);
              (*PyErr_Print)();
              status = MDSplusERROR;
            }
            (*Py_DecRef)(MDSplusException);
          }
        } else {
          getAnswer(ans, out_ptr);
          (*Py_DecRef)(ans);
        }
        (*Py_DecRef)(pyArgs);
        (*Py_DecRef)(pyFunction);
      } else {
        status = TdiUNKNOWN_VAR;
        fprintf(stderr,"Error: Module '%s' found but it does not contain function '%s'\n",filename,filename);
      }
      (*PyGILState_Release)(GIL);
    }
#ifndef _WIN32
  sigaction(SIGCHLD, &oldact, NULL);
#endif
  return status;
}

int TdiExtPython(struct descriptor *modname_d,
                 int nargs, struct descriptor **args, struct descriptor_xd *out_ptr){
  int status;
  char *dirspec = NULL, *filename = NULL;
  FREE_ON_EXIT(dirspec);
  FREE_ON_EXIT(filename);
  dirspec = findModule(modname_d, &filename);
  if (dirspec)
    status = TdiExtPython2(dirspec,filename,nargs,args,out_ptr);
  else
    status = TdiUNKNOWN_VAR;
  FREE_NOW(filename);
  FREE_NOW(dirspec);
  return status;
}
