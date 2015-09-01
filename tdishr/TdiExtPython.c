#include <mdsdescrip.h>
#include <mdsshr.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <tdishr_messages.h>
#include <dlfcn.h>
#include <signal.h>
#include <config.h>
#include <stdlib.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

typedef void* PyObject,PyThreadState;
typedef ssize_t Py_ssize_t;


static void (*DynPy_DecRef)() = 0;
#define Py_DECREF (*DynPy_DecRef)
static PyObject *(*DynPyTuple_New) () = 0;
#define PyTuple_New (*DynPyTuple_New)
static PyObject *(*DynPyString_FromString) () = 0;
#define PyString_FromString (*DynPyString_FromString)
static void (*DynPyTuple_SetItem) () = 0;
#define PyTuple_SetItem (*DynPyTuple_SetItem)
static PyObject *(*DynPyTuple_GetItem) () = 0;
#define PyTuple_GetItem (*DynPyTuple_GetItem)
static PyObject *(*DynPyObject_CallObject) () = 0;
#define PyObject_CallObject (*DynPyObject_CallObject)
static PyObject *(*DynPyObject_GetAttrString) () = 0;
#define PyObject_GetAttrString (*DynPyObject_GetAttrString)
static void *(*DynPyLong_AsVoidPtr) () = 0;
#define PyLong_AsVoidPtr (*DynPyLong_AsVoidPtr)
static PyObject *(*DynPyErr_Occurred) () = 0;
#define PyErr_Occurred (*DynPyErr_Occurred)
static void (*DynPyErr_Print) () = 0;
#define PyErr_Print (*DynPyErr_Print)
static PyObject *(*DynPyImport_ImportModule) () = 0;
#define PyImport_ImportModule (*DynPyImport_ImportModule)
static PyObject *(*DynPyModule_GetDict) () = 0;
#define PyModule_GetDict (*DynPyModule_GetDict)
static PyObject *(*DynPyDict_GetItemString) () = 0;
#define PyDict_GetItemString (*DynPyDict_GetItemString)
static PyObject *(*DynPy_BuildValue) () = 0;
#define Py_BuildValue (*DynPy_BuildValue)
static PyObject *Dyn_Py_NoneStruct;
static PyObject *(*DynPy_EndInterpreter) () = 0;
#define Py_EndInterpreter (*DynPy_EndInterpreter)
static PyThreadState *(*DynPy_NewInterpreter) () = 0;
#define Py_NewInterpreter (*DynPy_NewInterpreter)
static PyObject *(*DynPyList_Append) () = 0;
#define PyList_Append (*DynPyList_Append)
static PyObject *(*DynPyObject_CallFunction) () = 0;
#define PyObject_CallFunction (*DynPyObject_CallFunction)
static PyObject *(*DynPySys_GetObject) () = 0;
#define PySys_GetObject (*DynPySys_GetObject)
static int64_t (*DynPyLong_AsLong) () = 0;
#define PyLong_AsLong (*DynPyLong_AsLong)
static char *(*DynPyString_AsString) () = 0;
#define PyString_AsString (*DynPyString_AsString)
static Py_ssize_t(*DynPyList_Size) () = 0;
#define PyList_Size (*DynPyList_Size)
static int (*DynPyCallable_Check) () = 0;
#define PyCallable_Check (*DynPyCallable_Check)
static PyObject *(*DynPyList_GetItem) () = 0;
#define PyList_GetItem (*DynPyList_GetItem)
static PyObject *(*DynPyObject_Str) () = 0;
#define PyObject_Str (*DynPyObject_Str)

#define loadrtn(prefix,name,check) prefix ## name=dlsym(handle,#name);	\
  if (check && !prefix ## name) { \
  fprintf(stderr,"\n\nError finding python routine: %s\n\n",#name); \
  return 0;\
}

static int Initialize()
{
  if (!DynPyTuple_New) {
    void *(*tmpPy_Initialize) () = 0;
    void *handle;
    char *lib;
    char *envsym = getenv("PyLib");
    if (!envsym) {
      fprintf(stderr,
	      "\n\nYou cannot use the Py function until you defined the PyLib environment variable!\n\n",
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
#ifdef RTLD_NOLOAD
    /*** See if python routines are already available ***/
    handle = dlopen(0, RTLD_NOLOAD);
    loadrtn(tmp, Py_Initialize, 0);
    /*** If not, load the python library ***/
#endif
    if (!tmpPy_Initialize) {
      handle = dlopen(lib, RTLD_NOW | RTLD_GLOBAL);
      if (!handle) {
	fprintf(stderr, "\n\nUnable to load python library: %s\nError: %s\n\n", lib, dlerror());
	free(lib);
	return 0;
      }
      free(lib);
      loadrtn(tmp, Py_Initialize, 1);
      (*tmpPy_Initialize) ();
    }
    loadrtn(Dyn, Py_DecRef, 1);
    loadrtn(Dyn, PyTuple_New, 1);
    loadrtn(Dyn, PyString_FromString, 1);
    loadrtn(Dyn, PyTuple_SetItem, 1);
    loadrtn(Dyn, PyTuple_GetItem, 1);
    loadrtn(Dyn, PyObject_CallObject, 1);
    loadrtn(Dyn, PyObject_GetAttrString, 1);
    loadrtn(Dyn, PyLong_AsVoidPtr, 1);
    loadrtn(Dyn, PyErr_Occurred, 1);
    loadrtn(Dyn, PyErr_Print, 1);
    loadrtn(Dyn, PyImport_ImportModule, 1);
    loadrtn(Dyn, PyModule_GetDict, 1);
    loadrtn(Dyn, PyDict_GetItemString, 1);
    loadrtn(Dyn, Py_BuildValue, 1);
    loadrtn(Dyn, _Py_NoneStruct, 1);
    loadrtn(Dyn, Py_EndInterpreter, 1);
    loadrtn(Dyn, Py_NewInterpreter, 1);
    loadrtn(Dyn, PyList_Append, 1);
    loadrtn(Dyn, PyObject_CallFunction, 1);
    loadrtn(Dyn, PySys_GetObject, 1);
    loadrtn(Dyn, PyLong_AsLong, 1);
    loadrtn(Dyn, PyString_AsString, 1);
    loadrtn(Dyn, PyList_Size, 1);
    loadrtn(Dyn, PyCallable_Check, 1);
    loadrtn(Dyn, PyList_GetItem, 1);
    loadrtn(Dyn, PyObject_Str,1);
  }
  return 1;
}

static PyObject *getFunction(char *modulename, char *functionname)
{
  /* Given a directory path and a function name import a module with the same name as the
     function and find the function in that module with that name. Return null if there
     if an error.
   */
  PyObject *module;
  PyObject *moduleDict;
  PyObject *method;
  PyObject *ans = 0;
  PyObject *args;
  module = PyImport_ImportModule(modulename);
  if (module == 0) {
    printf("Error importing module %s\n", modulename);
    if (PyErr_Occurred()) {
      PyErr_Print();
    }
  } else {
    ans = PyObject_GetAttrString(module, functionname);
    if (ans == 0) {
      printf("Error finding function called '%s' in module %s\n", functionname, modulename);
      if (PyErr_Occurred()) {
	PyErr_Print();
      }
      Py_DECREF(module);
    } else {
      if (!PyCallable_Check(ans)) {
	printf("Error, item called '%s' in module %s is not callable\n", functionname, modulename);
	Py_DECREF(ans);
	ans = 0;
      }
    }
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
  sys_path = PySys_GetObject("path");
  listlen = PyList_Size(sys_path);
  for (idx = 0; idx < listlen; idx++) {
    PyObject *pathPart;
    pathPart = PyList_GetItem(sys_path, idx);
    if (strcmp(PyString_AsString(PyObject_Str(pathPart)), dirspec) == 0) {
      break;
    }
  }
  if (idx == listlen) {
    path = PyString_FromString(dirspec);
    PyList_Append(sys_path, path);
  }
}

static char *findModule(struct descriptor *modname_d, char **modname_out)
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
  int status;
  status = LibFindFileRecurseCaseBlind(&modfile_d, &ans_d, &ctx);
  LibFindFileEnd(&ctx);
  if (status & 1) {
    char *dirspec = MdsDescrToCstring((struct descriptor *)&ans_d);
    char *modnam = (char *)malloc(modlen + 1);
    strncpy(modnam, &dirspec[strlen(dirspec) - modlen - strlen(ftype)], modlen);
    modnam[modlen] = 0;
    *modname_out = modnam;
    dirspec[strlen(dirspec) - modlen - strlen(ftype)] = 0;
    return dirspec;
  } else {
    return 0;
  }
}

static PyObject *argsToTuple(int nargs, struct descriptor **args)
{
  /* Convert descriptor argument list to a tuple of python objects. */
  int idx = 0;
  PyObject *pointerToObject = 0;
  PyObject *ans = PyTuple_New(nargs);
  if (!pointerToObject)
    pointerToObject = getFunction("MDSplus", "pointerToObject");
  if (pointerToObject) {
    for (idx = 0; idx < nargs; idx++) {
      PyObject *arg =
	  PyObject_CallFunction(pointerToObject, (sizeof(void *) == 8) ? "L" : "l", args[idx]);
      if (arg) {
	PyTuple_SetItem(ans, idx, arg);
      } else {
	if (PyErr_Occurred()) {
	  PyErr_Print();
	}
	break;
      }
    }
  }
  if (idx != nargs) {
    Py_DECREF(ans);
    ans = PyTuple_New(0);
  }
  return ans;
}

static void getAnswer(PyObject * value, struct descriptor_xd *outptr)
{
  PyObject *makeDataFunction;
  PyObject *dataObj;
  makeDataFunction = getFunction("MDSplus", "makeData");
  dataObj = PyObject_CallFunction(makeDataFunction, "O", value);
  if (dataObj) {
    PyObject *descr = PyObject_GetAttrString(dataObj, "descriptor");
    if (descr) {
      PyObject *descrPtr = PyObject_GetAttrString(descr, "addressof");
      if (descrPtr) {
	MdsCopyDxXd((struct descriptor *)PyLong_AsLong(descrPtr), outptr);
	Py_DECREF(descrPtr);
      } else {
	printf("Error getting address of descriptor\n");
	if (PyErr_Occurred()) {
	  PyErr_Print();
	}
      }
      Py_DECREF(descr);
    } else {
      printf("Error getting descriptor\n");
      if (PyErr_Occurred()) {
	PyErr_Print();
      }
    }
    Py_DECREF(dataObj);
  } else {
    printf("Error converting answer to MDSplus datatype\n");
    if (PyErr_Occurred()) {
      PyErr_Print();
    }
  }
}

int TdiExtPython(struct descriptor *modname_d,
		 int nargs, struct descriptor **args, struct descriptor_xd *out_ptr)
{
  /* Try to locate a python module in the MDS_PATH search list and if found execute a function with the same name
     as the module in that module passing the arguments and get the answer back from python. */
  int status = TdiUNKNOWN_VAR;
  char *filename;
  int stat;
#ifndef _WIN32
  struct sigaction offact = {SIG_DFL, NULL, 0, 0, NULL};
  struct sigaction oldact;
  stat=sigaction(SIGCHLD, &offact, &oldact);
#endif
  char *dirspec = findModule(modname_d, &filename);
  if (dirspec) {
    if (Initialize()) {
      PyObject *ans;
      PyObject *pyargs;
      PyThreadState *tstate = Py_NewInterpreter();
      PyObject *pyFunction;
      PyObject *pyArgs;
      addToPath(dirspec);
      free(dirspec);
      pyFunction = getFunction(filename, filename);
      if (pyFunction) {
	free(filename);
	pyArgs = argsToTuple(nargs, args);
	ans = PyObject_CallObject(pyFunction, pyArgs);
	if (ans == 0) {
	  printf("Error calling fun in %s\n", filename);
	  if (PyErr_Occurred()) {
	    PyErr_Print();
	  }
	} else {
	  getAnswer(ans, out_ptr);
	  Py_DECREF(ans);
	  Py_DECREF(pyArgs);
	  status = 1;
	}
      }
      Py_EndInterpreter(tstate);
    }
  }
#ifndef _WIN32
  stat=sigaction(SIGCHLD, &oldact, NULL);
#endif
  return status;
}
