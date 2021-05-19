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

#include <dlfcn.h>
#include <libroutines.h>
#include <mds_stdarg.h>
#include <mdsdescrip.h>
#include <mdsplus/mdsconfig.h>
#include <mdsshr.h>
#include <pthread_port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strroutines.h>
#include <tdishr_messages.h>

//#define DEBUG
#include <mdsmsg.h>
#ifdef DEBUG
#define DEBUG_GIL_CHECK                                                   \
  if (PyGILState_Check)                                                   \
    fprintf(stderr,                                                       \
            "INFO: thread 0x%" PRIxPTR " holds GIL 0x%" PRIxPTR ": %c\n", \
            (uintptr_t)pthread_self(), (uintptr_t)GIL,                    \
            "ny"[PyGILState_Check() != 0]);
#else
#define DEBUG_GIL_CHECK
#endif

#ifdef _WIN32
#define USE_EXECFILE /* windows cannot use PyRun_File because if crashes on \
                        _lockfile */
#else
//#define USE_EXECFILE	/* for debugging purpose */
#endif

#define loadrtn2(sym, name, check)                                       \
  do                                                                     \
  {                                                                      \
    sym = dlsym(handle, name);                                           \
    if (check && !sym)                                                   \
    {                                                                    \
      fprintf(stderr, "\n\nError finding python routine: %s\n\n", name); \
      return;                                                            \
    }                                                                    \
  } while (0)
//"
#define loadrtn(sym, check) loadrtn2(sym, #sym, check)
// general
#define Py_single_input 256
#define Py_file_input 257
#define Py_eval_input 258
#define Py_None _Py_NoneStruct
typedef void *PyThreadState;
typedef void *PyObject;
typedef ssize_t Py_ssize_t;
static void (*Py_DecRef)() = NULL;
static PyObject *(*PyErr_Occurred)() = NULL;
static void (*PyErr_Print)() = NULL;
static PyObject *(*PyImport_ImportModule)() = NULL;
static PyThreadState *(*PyGILState_Ensure)() = NULL;
static void (*PyGILState_Release)(PyThreadState *) = NULL;
static int (*PyGILState_Check)() =
    NULL; // new in python 3.4 and required for 3.7

// loadPyFunction
static int (*PyCallable_Check)() = NULL;
static void (*PyErr_Clear)() = NULL;
static PyObject *(*PyImport_AddModule)() = NULL;
static PyObject *(*PyModule_AddObject)() = NULL;
static PyObject *(*PyObject_CallFunctionObjArgs)() = NULL;
static PyObject *(*PyString_FromString)() = NULL;
// callPyFunction
static PyObject *_Py_NoneStruct = NULL;
static int64_t (*PyLong_AsLong)() = NULL;
static PyObject *(*PyObject_CallObject)() = NULL;
static int (*PyObject_IsSubclass)() = NULL;
// get_answer
static void *(*PyLong_AsVoidPtr)() = NULL;
static PyObject *(*PyObject_GetAttrString)() = NULL;
// args_to_tuple
static PyObject *(*PyObject_CallFunction)() = NULL;
static PyObject *(*PyTuple_New)() = NULL;
static void (*PyTuple_SetItem)() = NULL;
#ifdef USE_EXECFILE
static int (*PyRun_SimpleStringFlags)() = NULL;
#else
static FILE *(*_Py_fopen_obj)() = NULL;
static int (*PyRun_SimpleFileExFlags)() = NULL;
#endif
static PyObject *pointerToObject = NULL;
static PyObject *makeData = NULL;
static PyObject *MDSplusException = NULL;

inline static void initialize()
{
  int old_state = PTHREAD_CANCEL_DISABLE;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
  void *handle;
  char *lib;
  char *envsym = getenv("PyLib");
  if (!envsym || (envsym[0] == '\0'))
  { // NULL or empty
#ifdef _WIN32
    envsym = "python27";
    const char *aspath = "C:\\Python27\\python27.dll";
    _putenv_s("PyLib", envsym);
#else
    envsym = "python2.7";
    const char *aspath = "/usr/lib/python2.7.so.1";
    setenv("PyLib", envsym, B_FALSE);
#endif
    fprintf(stderr,
            "\nYou should defined the PyLib environment variable!\nPlease "
            "define PyLib to be the name of your python library, i.e. '%s' or "
            "'%s'.\nWe will try '%s' as default.\n\n",
            envsym, aspath, envsym);
  }
  void (*Py_InitializeEx)() = NULL;
#ifdef RTLD_NOLOAD
  /*** See if python routines are already available ***/
  handle = dlopen(0, RTLD_NOLOAD);
  loadrtn(Py_InitializeEx, 0);
  /*** If not, load the python library ***/
  if (!Py_InitializeEx)
  {
#else
  {
#endif
#ifdef _WIN32
    if (strlen(envsym) > 6 &&
        (envsym[1] == ':' ||
         strncmp(envsym + strlen(envsym) - 4, ".dll", 4) == 0))
    {
      lib = strcpy((char *)malloc(strlen(envsym) + 1), envsym);
    }
    else
    {
      lib = strcpy((char *)malloc(strlen(envsym) + 5), envsym);
      strcat(lib, ".dll");
    }
#else
    if (strlen(envsym) > 6 &&
        (envsym[0] == '/' || strncmp(envsym, "lib", 3) == 0))
    {
      lib = strcpy((char *)malloc(strlen(envsym) + 1), envsym);
    }
    else
    {
      lib = strcpy((char *)malloc(strlen(envsym) + 7), "lib");
      strcat(lib, envsym);
      strcat(lib, ".so");
    }
#endif
    handle = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
      fprintf(stderr, "\n\nUnable to load python library: %s\nError: %s\n\n",
              lib, dlerror());
      free(lib);
      return;
    }
    MDSDBG("TdiExtPython: loaded %s\n", lib);
    free(lib);
    loadrtn(Py_InitializeEx, 1);
  }
  int (*Py_IsInitialized)();
  loadrtn(Py_IsInitialized, 1);
  if (!Py_IsInitialized())
  {                     // eg when called from python itself
    Py_InitializeEx(0); // 1: register signals; 0: don't
    int (*PyEval_ThreadsInitialized)() = NULL;
    loadrtn(PyEval_ThreadsInitialized, 1);
    if (!PyEval_ThreadsInitialized())
    {
      void (*PyEval_InitThreads)();
      loadrtn(PyEval_InitThreads, 1);
      PyEval_InitThreads(); // pyhton3.7 calls PyEval_InitThreads during
                            // Py_InitializeEx
    }
    void *(*PyEval_SaveThread)();
    loadrtn(PyEval_SaveThread, 1);
    PyEval_SaveThread();
  }
  loadrtn(Py_DecRef, 1);
  loadrtn(PyErr_Occurred, 1);
  loadrtn(PyErr_Print, 1);
  loadrtn(PyImport_ImportModule, 1);
  // loadPyFunction
  loadrtn(PyCallable_Check, 1);
  loadrtn(PyErr_Clear, 1);
  loadrtn(PyImport_AddModule, 1);
  loadrtn(PyModule_AddObject, 1);
  loadrtn(PyObject_CallFunctionObjArgs, 1);
  loadrtn(PyString_FromString, 0);
  if (!PyString_FromString)
    loadrtn2(PyString_FromString, "PyUnicode_FromString", 1);
  // callPyFunction
  loadrtn(_Py_NoneStruct, 1);
  loadrtn(PyLong_AsLong, 1);
  loadrtn(PyObject_CallObject, 1);
  loadrtn(PyObject_IsSubclass, 1);
  // get_answer
  loadrtn(PyLong_AsVoidPtr, 1);
  loadrtn(PyObject_GetAttrString, 1);
  // args_to_tuple
  loadrtn(PyObject_CallFunction, 1);
  loadrtn(PyTuple_New, 1);
  loadrtn(PyTuple_SetItem, 1);
#ifdef USE_EXECFILE
  loadrtn(PyRun_SimpleStringFlags, 1);
#else
  loadrtn(_Py_fopen_obj, 0);
  loadrtn(PyRun_SimpleFileExFlags, 1);
#endif
  loadrtn(PyGILState_Check, 0);
  loadrtn(PyGILState_Release, 1);
  loadrtn(PyGILState_Ensure, 1);
  if (old_state != PTHREAD_CANCEL_DISABLE)
    pthread_setcancelstate(old_state, NULL);
}

static void PyGILState_Cleanup(void *GIL)
{
  MDSDBG("PyGILState_Cleanup(0x%" PRIxPTR ") 0x%" PRIxPTR "\n", (uintptr_t)GIL,
         (uintptr_t)pthread_self());
  if (PyGILState_Check && PyGILState_Check())
  {
    fprintf(stderr,
            "FATAL: thread 0x%" PRIxPTR " still holds 0xGIL %" PRIxPTR
            "; ABORT\n",
            (uintptr_t)pthread_self(), (uintptr_t)GIL);
    abort();
  }
}

#define PYTHON_OPEN                                                               \
  if (PyGILState_Ensure)                                                          \
  {                                                                               \
    PyThreadState *GIL = PyGILState_Ensure();                                     \
    MDSDBG("PyGILState_Ensured(0x%" PRIxPTR ") 0x%" PRIxPTR "\n", (uintptr_t)GIL, \
           (uintptr_t)pthread_self());                                            \
    pthread_cleanup_push(PyGILState_Cleanup, (void *)GIL); //"

#define PYTHON_CLOSE                                                             \
  PyGILState_Release(GIL);                                                       \
  MDSDBG("PyGILState_Released(0x%" PRIxPTR ") 0x%" PRIxPTR "\n", (uintptr_t)GIL, \
         (uintptr_t)pthread_self());                                             \
  pthread_cleanup_pop(0);                                                        \
  DEBUG_GIL_CHECK;                                                               \
  } //"

static void importMDSplus()
{ // used in once
  initialize();
  PYTHON_OPEN
  {
    PyObject *MDSplus = PyImport_ImportModule("MDSplus");
    if (MDSplus)
    {
      pointerToObject = PyObject_GetAttrString(MDSplus, "pointerToObject");
      if (!pointerToObject)
      {
        fprintf(stderr, "Error loading function MDSplus.pointerToObject(); no "
                        "argument support\n");
        if (PyErr_Occurred())
          PyErr_Print();
      }
      makeData = PyObject_GetAttrString(MDSplus, "makeData");
      if (!makeData)
      {
        fprintf(stderr, "Error loading function MDSplus.makeData(); no return "
                        "value support\n");
        if (PyErr_Occurred())
          PyErr_Print();
      }
      MDSplusException = PyObject_GetAttrString(MDSplus, "MDSplusException");
      if (!MDSplusException)
      {
        fprintf(stderr, "Error loading class MDSplus.MDSplusException; only "
                        "limited error handling\n");
        if (PyErr_Occurred())
          PyErr_Print();
      }
      Py_DecRef(MDSplus); // release file lock
    }
    else
    {
      fprintf(stderr, "Error loading package MDSplus, check your PYTHONPATH; "
                      "very limited functionality\n");
      if (PyErr_Occurred())
        PyErr_Print();
    }
  }
  PYTHON_CLOSE;
}

static inline void importMDSplus_once()
{
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once, &importMDSplus);
}

static inline PyObject *args_to_tuple(const int nargs,
                                      const mdsdsc_t *const *const args)
{
  /* Convert descriptor argument list to a tuple of python objects. */
  if (!pointerToObject)
    return PyTuple_New(0); // no arguments
  PyObject *ans = PyTuple_New(nargs);
  int idx;
  for (idx = 0; idx < nargs; idx++)
  {
    PyObject *arg = PyObject_CallFunction(
        pointerToObject, (sizeof(void *) == 8) ? "L" : "l", args[idx]);
    if (arg)
      PyTuple_SetItem(ans, idx, arg);
    else
    {
      fprintf(stderr, "Error getting Object from arg %d\n", idx);
      if (PyErr_Occurred())
        PyErr_Print();
      PyTuple_SetItem(ans, idx, Py_None);
    }
  }
  return ans;
}

static inline void get_answer(const PyObject *const value,
                              mdsdsc_xd_t *const outptr)
{
  if (!makeData)
    return; // no return value
  PyObject *dataObj = PyObject_CallFunctionObjArgs(makeData, value, NULL);
  if (!dataObj)
  {
    printf("Error converting answer to MDSplus datatype\n");
    if (PyErr_Occurred())
      PyErr_Print();
    return;
  }
  PyObject *descr = PyObject_GetAttrString(dataObj, "descriptor");
  Py_DecRef(dataObj);
  if (!descr)
  {
    printf("Error getting descriptor\n");
    if (PyErr_Occurred())
      PyErr_Print();
    return;
  }
  PyObject *descrPtr = NULL;
  if (descr != Py_None)
    descrPtr = PyObject_GetAttrString(descr, "addressof");
  Py_DecRef(descr);
  if (!descrPtr)
  {
    printf("Error getting address of descriptor\n");
    if (PyErr_Occurred())
      PyErr_Print();
    return;
  }
  MdsCopyDxXd((mdsdsc_t *)PyLong_AsVoidPtr(descrPtr), outptr);
  Py_DecRef(descrPtr);
}

static inline int is_callable(const PyObject *const fun,
                              const char *const funname,
                              const char *const fullpath)
{
  if (!fun)
  {
    printf("Error finding function called '%s' in module %s\n", funname,
           funname);
    if (PyErr_Occurred())
      PyErr_Print();
    return FALSE;
  }
  if (!PyCallable_Check(fun))
  {
    printf("Error, item called '%s' in '%s' is not callable\n", funname,
           fullpath);
    Py_DecRef(fun);
    return FALSE;
  }
  if ((strcasecmp("py", funname) == 0) && (MdsSandboxEnabled() == 1))
  {
    printf("Error, item called '%s' in '%s' is not callable. The py() function "
           "is currently disabled\n",
           funname, fullpath);
    Py_DecRef(fun);
    return FALSE;
  }

  return TRUE;
}

#ifdef USE_EXECFILE
static inline PyObject *get_exec_file(const PyObject *const __main__)
{
  PyObject *execfile = PyObject_GetAttrString(__main__, "execfile");
  if (!execfile)
  { // not defined yet, so we define it
    PyErr_Clear();
    char def[] = "import __main__\ndef execfile(filename):\n with "
                 "open(filename,'r') as f:\n  "
                 "exec(compile(f.read(),filename,'exec'),__main__.__dict__,__"
                 "main__.__dict__)\0";
    int flags = 0;
    if (PyRun_SimpleStringFlags(def, &flags))
    {
      fprintf(stderr, "Error defining execfile\n");
      if (PyErr_Occurred())
        PyErr_Print();
      return NULL;
    }
    execfile = PyObject_GetAttrString(__main__, "execfile");
  }
  return execfile;
}
#endif

static inline void add__file__fun(const PyObject *const tdi_functions,
                                  const char *const filename,
                                  const PyObject *const __file__)
{
  char *__file__fun = malloc(strlen(filename) + 9);
  strcpy(__file__fun, "__file__");
  strcat(__file__fun, filename);
  if (PyModule_AddObject(tdi_functions, __file__fun, __file__))
  {
    fprintf(stderr, "Failed putting %s\n", __file__fun);
    if (PyErr_Occurred())
      PyErr_Print();
  }
  free(__file__fun);
}

static inline int load_python_fun(const char *const fullpath,
                                  char **const funname)
{
  // get __main__
  PyObject *__main__ = PyImport_AddModule("__main__");
  if (!__main__)
  {
    fprintf(stderr, "Error getting __main__ module'\n");
    if (PyErr_Occurred())
      PyErr_Print();
    return MDSplusERROR;
  }
#ifdef USE_EXECFILE
  PyObject *execfile = get_exec_file(__main__);
  if (!execfile)
    return MDSplusERROR;
#endif
  // add __file__=<fullpath> to globals
  if (PyModule_AddObject(
          __main__, "__file__",
          PyString_FromString(fullpath)))
  { // no need to deref PyString
    fprintf(stderr, "Failed adding __file__='%s'\n", fullpath);
    if (PyErr_Occurred())
      PyErr_Print();
  }
  PyObject *__file__ = PyString_FromString(fullpath);
#ifdef USE_EXECFILE
  PyObject *ans = PyObject_CallFunctionObjArgs(execfile, __file__, NULL);
  Py_DecRef(execfile);
  Py_DecRef(ans);
  if (!ans)
  {
#else
  int err;
  INIT_AND_FCLOSE_ON_EXIT(fp);
  if (_Py_fopen_obj)
    fp = _Py_fopen_obj(__file__, "r");
  else
    fp = fopen(fullpath, "r");
  if (!fp)
  {
    fprintf(stderr, "Error opening file '%s'\n", fullpath);
    Py_DecRef(__file__);
    return MDSplusERROR;
  }
  int flags = 0;
  err = PyRun_SimpleFileExFlags(fp, fullpath, 1, &flags);
  FCLOSE_CANCEL(fp);
  if (err)
  {
#endif
    fprintf(stderr, "Error compiling file '%s'\n", fullpath);
    if (PyErr_Occurred())
      PyErr_Print();
    Py_DecRef(__file__);
    return TdiUNKNOWN_VAR;
  }
  const char *c, *p = fullpath;
  for (; (c = strchr(p, '/')); p = c + 1)
    ;
#ifdef _WIN32
  for (; (c = strchr(p, '\\')); p = c + 1)
    ;
#endif
  const size_t mlen = strlen(p) - 3;
  *funname = memcpy(malloc(mlen + 1), p, mlen);
  funname[0][mlen] = '\0';
  PyObject *pyFunction = PyObject_GetAttrString(__main__, *funname);
  if (!is_callable(pyFunction, *funname, fullpath))
  {
    free(*funname);
    *funname = NULL;
    Py_DecRef(__file__);
    return TdiUNKNOWN_VAR;
  }
  PyObject *tdi_functions =
      PyImport_AddModule("tdi_functions"); // from sys.modules or new
  // add __file__<filename> to module
  add__file__fun(tdi_functions, *funname, __file__);
  // add function to module
  // PyModule_AddObject: This steals a reference to value - no
  // Py_DecRef(pyFunction)
  if (PyModule_AddObject(tdi_functions, *funname, pyFunction))
    return MDSplusERROR;
  return MDSplusSUCCESS;
}

static inline int call_python_fun(const char *const filename, const int nargs,
                                  const mdsdsc_t *const *const args,
                                  mdsdsc_xd_t *const out_ptr)
{
  PyObject *tdi_functions = PyImport_AddModule("tdi_functions");
  if (tdi_functions)
  {
    PyObject *__main__ = PyImport_AddModule("__main__");
    char *__file__fun = malloc(strlen(filename) + 9);
    strcpy(__file__fun, "__file__");
    strcat(__file__fun, filename);
    PyObject *__file__ = PyObject_GetAttrString(tdi_functions, __file__fun);
    free(__file__fun);
    if (__file__)
      PyModule_AddObject(__main__, "__file__", __file__);
    else
    { // silently fail and set __file__ to None
      PyModule_AddObject(__main__, "__file__", Py_None);
      if (PyErr_Occurred())
        PyErr_Clear();
    }
  }
  else
    fprintf(stderr, "Failed getting module tdi_functions\n");
  if (PyErr_Occurred())
    PyErr_Print();
  if ((strcasecmp("py", filename) == 0) && (MdsSandboxEnabled() == 1))
    return MDSplusSANDBOX;
  PyObject *pyFunction = PyObject_GetAttrString(tdi_functions, filename);
  if (!pyFunction)
  {
    if (PyErr_Occurred())
      PyErr_Print();
    return MDSplusERROR;
  }
  PyObject *pyArgs = args_to_tuple(nargs, args);
  PyObject *ans = PyObject_CallObject(pyFunction, pyArgs);
  Py_DecRef(pyFunction);
  Py_DecRef(pyArgs);
  if (ans)
  {
    get_answer(ans, out_ptr);
    Py_DecRef(ans);
    return MDSplusSUCCESS;
  }
  PyObject *exc = PyErr_Occurred();
  if (exc)
  {
    PyErr_Print();
    if (MDSplusException && PyObject_IsSubclass(exc, MDSplusException))
    {
      PyObject *status_obj = PyObject_GetAttrString(exc, "status");
      int status = (int)PyLong_AsLong(status_obj);
      Py_DecRef(status_obj);
      if (STATUS_NOT_OK)
      {
        char *fac_out = NULL, *msgnam_out = NULL, *text_out = NULL;
        const char *f = "WSEIF???";
        MdsGetStdMsg(status, (const char **)&fac_out,
                     (const char **)&msgnam_out, (const char **)&text_out);
        printf("%%%s-%c-%s: %s\n", fac_out, f[status & 7], msgnam_out,
               text_out);
      }
      return status;
    }
    else
    {
      fprintf(stderr, "Error calling fun in %s\n", filename);
      return MDSplusERROR;
    }
  }
  return MDSplusSUCCESS;
}
int tdi_load_python_fun(const char *const filepath, char **const funname)
{
  int status;
  importMDSplus_once();
  PYTHON_OPEN;
  status = load_python_fun(filepath, funname);
  PYTHON_CLOSE else status = LibNOTFOU;
  return status;
}

int tdi_call_python_fun(const char *const filename, const int nargs,
                        const mdsdsc_t *const *const args,
                        mdsdsc_xd_t *const out_ptr)
{
  int status;
  importMDSplus_once();
  PYTHON_OPEN;
  status = call_python_fun(filename, nargs, args, out_ptr);
  PYTHON_CLOSE else status = LibNOTFOU;
  return status;
}
