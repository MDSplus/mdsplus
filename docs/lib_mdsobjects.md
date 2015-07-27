MDSplus Objects Intefaces  {#lib_mdsobjects}
=========================


MDSplus provides API's (application programming interface) to several object 
oriented languages. A common set of classes and methods are provided in all 
of these API's. 
There may be some language specific "enhancements" to the common interface to 
take advantage of some features of the language which are not available in all 
of the languages supported. 


C++ Interface
-------------

At the time of writing all the C++ interface library is actually a wrapper
above the MDSplus components such as the mdsip connection library and the tree
structures. It provide a set of objects that describe the common routines
performed to acquire and manage data in a object oriented environment. You
should find it linked in a shared library named MdsObjectsCppShr.so (.dll in
windows) while the file mdsobjects.h contains all the definitions of the object
classes. All the C++ classes for MDSplus objects are declared to belong to \em
MDSplus namespace. It is therefore convenient to begin the program as follows:

\code{.cpp}
  #include <mdsobjects.h>
  using namespace MDSplus;
\endcode

For further details see reference manual for the MDSplus C++ Object component:

|   GO TO THE COMPONENT MANUAL  |
|-------------------------------|
| \ref mdsobjects_cpp_manual    |



Python Interface
----------------

The python interface to MDSplus objects.


|   GO TO THE COMPONENT MANUAL  |
|-------------------------------|
| \ref mdsobjects_py_manual     |


Java Interface
---------------

The Java interface to MDSplus objects.


|   GO TO THE COMPONENT MANUAL  |
|-------------------------------|
| \ref mdsobjects_java_manual   |
