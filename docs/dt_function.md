Function {#dt_function}
========

MDSplus provides a function data type used for describing references to built
in native TDI functions. You will not normally need to concern yourself with
this data type since it is constructed automatically when you use TDI
expression syntax to invoke built in functions. When you specify an expression
such as "a + b", MDSplus will compile this into a DTYPE_FUNCTION data item. The
function data type consists of a opcode and a list of operands. The opcode is
stored as a 16 bit code and the operands can be any MDSplus data type.

    a + b

will be compiled into the same function structure as if you had specified:

    BUILD_FUNCTION(BUILDIN_OPCODE('ADD'),a,b)

The following table lists some of the TDI functions available for creating and
accessing dtype function:

\latexonly { \tiny \endlatexonly

| **Function**    | **Description**                         |
|-----------------|-----------------------------------------|
| BUILD\_FUNCTION | Construct a internal function reference |
| MAKE\_FUNCTION  | Construct a internal function reference |

\latexonly } \endlatexonly


