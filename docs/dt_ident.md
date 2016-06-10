Ident {#dt_ident}
=====

MDSplus expressions can contain private and public variables (often called "Ken
Variables" after the person that implemented most of TDI, Ken Klare). When an
expression is compiled containing these variables, the reference to the
variable is stored as an DTYPE_IDENT distinguishing the name from a node
reference or a text string. When an expression containing variable references
are evaluated, the current contents of the variable is substituted for this
reference.

Care should be taken to not store variable references as data in nodes or
return them in TDI function unless they are guaranteed to be correctly defined
when the expression is evaluated. The MAKE_structure built-in functions are
provided to ensure that the variables are dereferenced. For example, if your
TDI function looked something like:

###INCORRECT!###

    Public Fun MYFUN(IN _X, IN _Y) {
      return(BUILD_SIGNAL(_Y,*,_X));
    }

You would get unexpected results. In this case when you try to plot the signal
returned, you would get the current contents of the private variables _X and _Y
if they are defined at all and not necessarily the values passed into the
function. To avoid this you would use the MAKE_SIGNAL function instead of the
BUILD_SIGNAL function. The MAKE_SIGNAL function replaces any of its arguments
that are Identifiers with the contents of that identifier.

###CORRECT!###

    Public Fun MYFUN(IN _X, IN _Y) {
      return(MAKE_SIGNAL(_Y,*,_X));
    }

The following table lists some of the functions used to create or access the
ident datatype:

\latexonly { \tiny \endlatexonly


| **Function**      | **Description**                          |
|-------------------|------------------------------------------|
| ALLOCATED         | Test if variable is currently defined    |
| DEALLOCATE        | Release variable                         |
| EQUALS \[=\]      | Load variable                            |
| MAKE\_ACTION      | Make an action structure                 |
| MAKE\_CALL        | Make a call structure                    |
| MAKE\_CONDITION   | Make a condition structure               |
| MAKE\_CONGLOM     | Make a conglomerate structure            |
| MAKE\_DEPENDENCY  | Make a dependency structure              |
| MAKE\_DIM         | Make a dimension structure               |
| MAKE\_DISPATCH    | Make a dispatch structure                |
| MAKE\_FUNCTION    | Make a function structure                |
| MAKE\_METHOD      | Make a method structure                  |
| MAKE\_PARAM       | Make a parameter structure               |
| MAKE\_PROCEDURE   | Make a procedure structure               |
| MAKE\_PROGRAM     | Make a program structure                 |
| MAKE\_RANGE       | Make a range structure                   |
| MAKE\_ROUTINE     | Make a routine structure                 |
| MAKE\_SIGNAL      | Make a signal structure                  |
| MAKE\_SLOPE       | Make a slope structure                   |
| MAKE\_WINDOW      | Make a window structure                  |
| MAKE\_WITH\_ERROR | Make a with\_error structure             |
| MAKE\_WITH\_UNITS | Make a with\_units structure             |
| RESET\_PRIVATE    | Deassign private variables and functions |
| RESET\_PUBLIC     | Deassign public variables and functions  |
| SHOW\_PRIVATE     | List private variables and functions     |
| SHOW\_PUBLIC      | List public variables and functions      |
| VAR               | Define or reference variable             |

\latexonly } \endlatexonly
