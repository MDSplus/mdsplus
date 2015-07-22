
With Units {#dt_with_units}
==========

MDSplus provides a with units data type which enables you to attach units to a
value. A with units data item is a structure containing two "fields". The first
field is the value and the second is the units. Each part of a with units data
time can be either missing or a value of any other supported data type. General
applications available at your site may assume some convention on the type of
data each field contains (i.e. a units field that when evaluated returns
DTYPE_T, a string).

A with units item can be created using the BUILD_WITH_UNITS or MAKE_WITH_UNITS
functions. For example:

    _mydata = BUILD_WITH_UNITS(42,"volts")


The following table lists some of the TDI functions available for creating or
accessing with units items:

\latexonly { \tiny \endlatexonly

| **Function**       | **Description**                                                                        |
|--------------------|----------------------------------------------------------------------------------------|
| BUILD\_WITH\_UNITS | Build a with units structure                                                           |
| DATA               | Evaluates value portion of with units item converting to one of integer, float or text |
| MAKE\_WITH\_UNITS  | Make a with units structure                                                            |
| UNITS              | Return units or blank                                                                  |
| UNITS\_OF          | Return the units field of with units structure                                         |
| VALUE\_OF          | Return the value field of with units structure                                         |

\latexonly } \endlatexonly

