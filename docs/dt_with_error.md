With Error {#dt_with_error}
==========

MDSplus provides a with error data type which enables you to attach error
information to a value. A with error data item is a structure containing two
"fields". The first field is the value and the second is the error. Each part
of a with error data type can be either missing or a value of any other
supported data type. General applications available at your site may assume
some convention on the type of data each field contains.

A with error item can be created using the BUILD_WITH_ERROR or MAKE_WITH_ERROR
functions. For example:

    _mydata = BUILD_WITH_ERROR([42,43,40],[.5,.45,.6])

The following table lists some of the TDI functions available for creating or
accessing with error items:

\latexonly { \tiny \endlatexonly

| **Function**       | **Description**                                                                        |
|--------------------|----------------------------------------------------------------------------------------|
| BUILD\_WITH\_ERROR | Build a with error structure                                                           |
| DATA               | Evaluates value portion of with error item converting to one of integer, float or text |
| ERROR\_OF          | Return the error field of with error structure                                         |
| MAKE\_WITH\_ERROR  | Make a with error structure                                                            |
| VALUE\_OF          | Return the value field of with error structure                                         |


\latexonly } \endlatexonly

