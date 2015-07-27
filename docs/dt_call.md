Call {#dt_call}
====


MDSplus provides a call data type used for describing references to external
functions. The MDSplus expression evaluator has the ability to call external
functions in shared libararies or DLL's. The call datatype describes the type
of the return value of the function, the name of the library or DLL where the
function can be found, the name of the function and the list of arguments to
present to the function when calling it. The expression evaluator in MDSplus
has syntax for easily building instances of this data type as will as builtin
functions for constructing them. The call to the external function occurs when
the expression containing a call item is evaluated. The following example shows
an example of calling an external function:

    mysharedlib->myfunction(42)

The code above is the most common form of the function reference. When
evaluated, the function called “myfunction” found in the library “mysharedlib”
will be called with one argument, 42. The return value of the function is
assumed to be a 32-bit signed integer.

    build_call(kind(1),“mysharedlib”,“myfunction”,42)

The above code creates the same function as the previous example but uses the
build\_call function to construct the call item. The behavior would be
identical to the previous example.

    _x=zero(100)
    build_call(kind(1e6),"/home/twf/mylibraries/libmysharedlib",“myfunction2”,val(42),ref(_x))

This example builds a similar call but in this case the function return is
handled as a floating point value and the “myfunction2” function is called with
two arguments, 42 passed by value and a TDI variable passed by reference. This
example uses the full file specification (without the file type).

MDSplus uses operating system specific rules to locate shared libraries when
the full file specification of the library is not given. On OpenVMS, the system
would first look for a logical name matching the name of the library and if one
is found it would activate the image defined by the logical name. If their is
no such logical name, the system would then look in the directory search path
defined by SYS$SHARE for the library. The file type for shared libraries on
OpenVMS is ".EXE". On Unix systems the system looks for a library path
environment variable such as LD\_LIBRARY\_PATH or SHLIB\_PATH and looks in
these directories. If there is no such environment variable, the system looks
in system library directories (usually /usr/lib or /usr/local/lib) for the
shared library. The file type for shared libraries on Unix is generally
".so",".sl" or sometimes ".a". On Windows systems, the system looks for shared
libraries in the directory where the application was run or in the directory
list specified by the “PATH” environment variable. The file type of shared
libraries on Windows is ".dll".

The following table lists some of the TDI functions available for creating and
accessing the call function:

\latexonly { \tiny \endlatexonly

| **Function** | **Description**                                   |
|--------------|---------------------------------------------------|
| BUILD\_CALL  | Build a call structure                            |
| IMAGE\_OF    | Return the image or shared library part of a call |
| ROUTINE\_OF  | Return the routine part of a call                 |
| MAKE\_CALL   | Make a call structure                             |

\latexonly } \endlatexonly
  
