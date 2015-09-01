 
Routine {#dt_routine}
=======

MDSplus provides a routine data type for describing an function call to be used
as the task portion of an ction data item.

The routine data type is a structure consisting of three fields followed by
optional arguments to the method being performed.

The first field is a timeout value specified in seconds. When the routine is
being executed and a timeout is specified, a timer is started when the
operation is started and if the operation is not completed before the specified
timeout, the operation will be aborted abruptly. This field can either be
omitted or should contain a value or expression which when evaluated will
result in a scalar numeric value.

The next field in the routine data type is the name of an image which contains
the routine to be executed. This should contain a value or expression which
when evaluated will result in a scalar text value indicating the name of the
image or library where the routine can be found (case sensitive on Unix
platforms). See the description of the call data type for additional
information on how the library is located.

The next field is the routine name to execute. This should conatin a value or
expression which when evaluated will result in a scalar text value indicating
the name of the routine (case sensitive on Unix and Windows platforms) to be
executed.

The following fields represent optional arguments to be passed to the routine.
The arguments will be evaluated and converted to native data types before being
passed to the routine.

This data type is rarely used since the task part of an action can be also be
an expression. The call capabilities built into the expression evaluator
provides much more flexibility in specifying how the arguments to the routine
are to be passed.



The following table lists some of the TDI functions available for creating and
accessing routines:

\latexonly { \tiny \endlatexonly

| **Function**   | **Description**                         |
|----------------|-----------------------------------------|
| BUILD\_ROUTINE | Build a routine structure               |
| IMAGE\_OF      | Return the image name part of a routine |
| MAKE\_ROUTINE  | Make a routine structure                |
| ROUTINE\_OF    | Return the object part of a routine     |
| TIME\_OUT\_OF  | Return the timeout part of a routine    |


\latexonly } \endlatexonly
