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
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <tdishr_messages.h>
#include <libroutines.h>
#include <strroutines.h>
#define Py_None _Py_NoneStruct

#define loadrtn2(sym,name,check) do{\
  sym = dlsym(handle,name); \
  if (check && !sym) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",name); \
  return; \
}}while(0)
//"
#define loadrtn(sym,check) loadrtn2(sym,#sym,check)

// general
#define Py_single_input 256
#define Py_file_input 257
#define Py_eval_input 258
typedef void*          PyThreadState;
typedef void*          PyObject;
typedef ssize_t        Py_ssize_t;
static PyThreadState*(*PyGILState_Ensure)() = NULL;
static void          (*PyGILState_Release)(PyThreadState *) = NULL;
static void          (*Py_DecRef)() = NULL;
static PyObject*     (*PyErr_Occurred) () = NULL;
static void          (*PyErr_Print) () = NULL;

//loadPyFunction
static PyObject* (*PyDict_GetItemString) () = NULL;
static PyObject* (*PyDict_SetItemString) () = NULL;
static PyObject* (*PyImport_AddModule) () = NULL;
static PyObject* (*PyModule_GetDict) () = NULL;
static PyObject* (*PyString_FromString) () = NULL;
//callPyFunction
static PyObject* _Py_NoneStruct = NULL;
static int64_t   (*PyLong_AsLong) () = NULL;
static PyObject* (*PyObject_CallObject) () = NULL;
static int       (*PyObject_IsSubclass) () = NULL;
//getAnswer
static void*     (*PyLong_AsVoidPtr) () = NULL;
static PyObject* (*PyObject_CallFunctionObjArgs) () = NULL;//argsToTuple
static PyObject* (*PyObject_GetAttrString) () = NULL;//getFunction
//getFunction
static int       (*PyCallable_Check) () = NULL;
static PyObject* (*PyImport_ImportModule) () = NULL;
//argsToTuple
static PyObject* (*PyObject_CallFunction) () = NULL;
static PyObject* (*PyTuple_New) () = NULL;
static void      (*PyTuple_SetItem) () = NULL;
#ifdef _WIN32
//addTo/removeFromPath
static PyObject* (*PySys_GetObject) () = NULL;
static int       (*PyList_Insert) () = NULL;
static int       (*PySequence_DelItem) () = NULL;
#else
static PyObject* (*PyDict_Copy) () = NULL;
static PyObject* (*PyRun_FileEx)() = NULL;
#endif

static pthread_once_t once = PTHREAD_ONCE_INIT;
#define PYTHON_OPEN \
pthread_once(&once,&initialize);\
if (PyGILState_Ensure) { \
  PyThreadState *GIL = (*PyGILState_Ensure)(); \
  pthread_cleanup_push((void*)PyGILState_Release,(void*)GIL);

#define PYTHON_CLOSE \
  pthread_cleanup_pop(1);\
} else status = LibNOTFOU;

static void initialize(){
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
       return;
    }
    free(lib);
    loadrtn(Py_Initialize, 1);
    Py_Initialize();
    loadrtn(PyEval_ThreadsInitialized, 1);
    if (PyEval_ThreadsInitialized() == C_OK) {
      loadrtn(PyEval_InitThreads, 1);
      loadrtn(PyEval_SaveThread, 1);
      PyEval_InitThreads();
      PyEval_SaveThread();
    }
  }
  loadrtn(Py_DecRef, 1);
  loadrtn(PyGILState_Release, 1);
  loadrtn(PyErr_Occurred, 1);
  loadrtn(PyErr_Print, 1);
  //loadPyFunction
  loadrtn(PyDict_GetItemString, 1);
  loadrtn(PyDict_SetItemString, 1);
  loadrtn(PyImport_AddModule, 1);
  loadrtn(PyModule_GetDict, 1);
  loadrtn(PyString_FromString, 0);
  if (!PyString_FromString)
    loadrtn2(PyString_FromString,"PyUnicode_FromString", 1);
  //callPyFunction
  loadrtn(_Py_NoneStruct, 1);
  loadrtn(PyLong_AsLong, 1);
  loadrtn(PyObject_CallObject, 1);
  loadrtn(PyObject_IsSubclass,1);
  //getAnswer
  loadrtn(PyLong_AsVoidPtr, 1);
  loadrtn(PyObject_CallFunctionObjArgs, 1);
  loadrtn(PyObject_GetAttrString, 1);
  //getFunction
  loadrtn(PyCallable_Check, 1);
  loadrtn(PyImport_ImportModule, 1);
  //argsToTuple
  loadrtn(PyObject_CallFunction, 1);
  loadrtn(PyTuple_New, 1);
  loadrtn(PyTuple_SetItem, 1);
#ifdef _WIN32
  //addTo/removeFromPath
  loadrtn(PySys_GetObject, 1);
  loadrtn(PyList_Insert, 1);
  loadrtn(PySequence_DelItem, 1);
#else
  loadrtn(PyDict_Copy, 1);
  loadrtn(PyRun_FileEx, 1);
#endif
  // last key function
  loadrtn(PyGILState_Ensure, 1);
}

#ifdef _WIN32
static void addToPath(const char *dirspec){
  // Add directory to top of sys.path
  PyObject *sys_path = PySys_GetObject("path");
  PyObject *path = PyString_FromString(dirspec);
  PyList_Insert(sys_path, (Py_ssize_t) 0, path);
  Py_DecRef(path);
}

static void removeFromPath(){
  // Add directory to top of sys.path
  PyObject *sys_path = PySys_GetObject("path");
  PySequence_DelItem(sys_path, (Py_ssize_t) 0);
}
#endif

static PyObject *getFunction(const char *modulename, const char *functionname){
  /* Given a directory path and a function name import a module with the same name as the
     function and find the function in that module with that name. Return null if there
     if an error.
   */
  PyObject *module;
  PyObject *ans = NULL;
  module = PyImport_ImportModule(modulename);
  if (!module) {
    printf("Error importing module %s\n", modulename);
    if (PyErr_Occurred()) PyErr_Print();
  } else {
    ans = PyObject_GetAttrString(module, functionname);
    if (!ans) {
      printf("Error finding function called '%s' in module %s\n", functionname, modulename);
      if (PyErr_Occurred()) PyErr_Print();
    } else {
      if (!PyCallable_Check(ans)) {
        printf("Error, item called '%s' in module %s is not callable\n", functionname, modulename);
        Py_DecRef(ans);
        ans = NULL;
      }
    }
    Py_DecRef(module);
  }
  return ans;
}

char *findModule(struct descriptor *modname_d, char **modname_out){
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

static PyObject *argsToTuple(int nargs, struct descriptor **args){
  /* Convert descriptor argument list to a tuple of python objects. */
  PyObject *pointerToObject = getFunction("MDSplus", "pointerToObject");
  if (!pointerToObject) {
    fprintf(stderr,"Error loading function MDSplus.pointerToObject()\n");
    if (PyErr_Occurred()) PyErr_Print();
    return PyTuple_New(0);
  }
  PyObject *ans = PyTuple_New(nargs);
  int idx;
  for (idx = 0; idx < nargs; idx++) {
    PyObject *arg = PyObject_CallFunction(pointerToObject, (sizeof(void *) == 8) ? "L" : "l", args[idx]);
    if (arg)
       PyTuple_SetItem(ans, idx, arg);
    else {
      fprintf(stderr,"Error getting Object from arg %d\n",idx);
      if (PyErr_Occurred()) PyErr_Print();
      PyTuple_SetItem(ans, idx, Py_None);
    }
  }
  Py_DecRef(pointerToObject);
  return ans;
}

static void getAnswer(PyObject * value, struct descriptor_xd *outptr){
  PyObject *makeData = getFunction("MDSplus", "makeData");
  if (!makeData) {
    fprintf(stderr,"Error loading function MDSplus.makeData()\n");
    if (PyErr_Occurred()) PyErr_Print();
    return;
  }
  PyObject *dataObj = PyObject_CallFunctionObjArgs(makeData, value, NULL);
  Py_DecRef(makeData);
  if (!dataObj) {
    printf("Error converting answer to MDSplus datatype\n");
    if (PyErr_Occurred()) PyErr_Print();
     return;
  }
  PyObject *descr = PyObject_GetAttrString(dataObj, "descriptor");
  Py_DecRef(dataObj);
  if (!descr) {
    printf("Error getting descriptor\n");
    if (PyErr_Occurred()) PyErr_Print();
    return;
  }
  PyObject* descrPtr = NULL;
  if (descr!=Py_None)
    descrPtr = PyObject_GetAttrString(descr, "addressof");
  Py_DecRef(descr);
  if (!descrPtr) {
    printf("Error getting address of descriptor\n");
    if (PyErr_Occurred()) PyErr_Print();
    return;
  }
  MdsCopyDxXd((struct descriptor *)PyLong_AsVoidPtr(descrPtr), outptr);
  Py_DecRef(descrPtr);
}

int loadPyFunction_(const char *dirspec,const char *filename) {
#ifdef _WIN32
  // cannot get the other method to work in wine
  PyObject *pyFunction;
  addToPath(dirspec);
  pthread_cleanup_push(removeFromPath,NULL);
  pyFunction = getFunction(filename,filename);
  pthread_cleanup_pop(1);
#else
  char* fullpath = malloc(strlen(dirspec)+strlen(filename)+4);
  strcpy(fullpath, dirspec);
  strcat(fullpath, filename);
  strcat(fullpath, ".py");
  FILE *fp = fopen(fullpath,"r");
  if (!fp) {
    fprintf(stderr,"Error opening file '%s'\n",fullpath);
    free(fullpath);
    return TdiUNKNOWN_VAR;
  }
  PyObject *__main__ = PyImport_AddModule("__main__");
  if (!__main__) {
    fprintf(stderr,"Error getting __main__ module'\n");
    if (PyErr_Occurred()) PyErr_Print();
    fclose(fp);
    free(fullpath);
    return MDSplusERROR;
  }
  PyObject *env = PyDict_Copy(PyModule_GetDict(__main__));
  // add __file__ required for e.g. RFXDEVICES.py
  PyObject *__file__ = PyString_FromString(fullpath);
  free(fullpath);
  PyDict_SetItemString(env,"__file__",__file__);
  Py_DecRef(__file__);
  PyObject *res = PyRun_FileEx(fp, filename, Py_file_input, env, env, 1);
  if (!res) {
    fprintf(stderr,"Error compiling file '%s%s.py'\n",dirspec,filename);
    if (PyErr_Occurred()) PyErr_Print();
    Py_DecRef(env);
    return TdiUNKNOWN_VAR;
  }
  Py_DecRef(res);
  PyObject *pyFunction = PyDict_GetItemString(env,filename);
  Py_DecRef(env);
#endif
  if (!pyFunction) {
    fprintf(stderr,"Error loading function '%s' from file '%s%s.py'\n",filename,dirspec,filename);
    return TdiUNKNOWN_VAR;
  }
  PyObject *tdi_functions = PyImport_AddModule("tdi_functions"); // from sys.modules or new
  PyObject *tdi_dict = PyModule_GetDict(tdi_functions);
  int status = PyDict_SetItemString(tdi_dict, filename, pyFunction) ? MDSplusERROR : MDSplusSUCCESS;
  Py_DecRef(pyFunction);
  return status;
}

int callPyFunction_(char *filename,int nargs, struct descriptor **args, struct descriptor_xd *out_ptr) {
  PyObject *pyFunction = getFunction("tdi_functions", filename);
  if (!pyFunction) {
    if (PyErr_Occurred()) PyErr_Print();
    return MDSplusERROR;
  }
  PyObject *pyArgs = argsToTuple(nargs, args);
  PyObject *ans = PyObject_CallObject(pyFunction, pyArgs);
  Py_DecRef(pyFunction);
  Py_DecRef(pyArgs);
  if (ans) {
    getAnswer(ans, out_ptr);
    Py_DecRef(ans);
    return MDSplusSUCCESS;
  }
  PyObject* exc = PyErr_Occurred();
  if (exc) {
    PyErr_Print();
    PyObject *MDSplusException = getFunction("MDSplus","MDSplusException");
    if (MDSplusException && PyObject_IsSubclass(exc,MDSplusException)) {
      PyObject *status_obj = PyObject_GetAttrString(exc, "status");
      int status = (int)PyLong_AsLong(status_obj);
      Py_DecRef(status_obj);
      if STATUS_NOT_OK {
        char *fac_out=NULL, *msgnam_out=NULL, *text_out=NULL;
        const char *f = "WSEIF???";
        MdsGetStdMsg(status,(const char **)&fac_out,(const char **)&msgnam_out,(const char **)&text_out);
        printf("%%%s-%c-%s: %s\n",fac_out,f[status&7],msgnam_out,text_out);
      }
      return status;
    } else {
      fprintf(stderr,"Error calling fun in %s\n", filename);
      if (!MDSplusException)
        fprintf(stderr,"Error loading class MDSplus.MDSplusException\n");
      PyErr_Print();
      return MDSplusERROR;
    }
  }
  return MDSplusSUCCESS;
}

int loadPyFunction(const char *dirspec,const char *filename) {
  int status;
  PYTHON_OPEN;
    status = loadPyFunction_(dirspec,filename);
  PYTHON_CLOSE;
  return status;
}

int callPyFunction(char *filename,int nargs, struct descriptor **args, struct descriptor_xd *out_ptr) {
  int status;
  PYTHON_OPEN;
    status = callPyFunction_(filename,nargs,args,out_ptr);
  PYTHON_CLOSE;
  return status;
}

