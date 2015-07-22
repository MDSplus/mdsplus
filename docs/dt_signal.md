
Signal {#dt_signal}
======

MDSplus provides a signal data type which combines dimension descriptions with
the data. While it was initially designed to be used for efficient storage of
data acquired from measurement devices attached to an experiment it has been
found to be a very useful way of storing additional information such as results
from data analysis or modeling data. A signal is a structure of three or more
fields. The first field is the "value" field of the signal. This is followed by
an optional raw data field (explained later). These two fields can be followed
by one or more dimension descriptions. The number of dimension descriptions in
general should match the dimensionality of the value.

The easiest way to describe a signal is through an example. MDSplus stores data
from a transient recorder which is a device which measures voltage as a
function of time. Typically a transient recorder uses a analog to digital
converter and records the data as a series of integer values. A linear
conversion can be done to convert these integer values into input voltages
being measured. When MDSplus stores the data for such a device it uses a signal
datatype to record the data. The signal structure is constructed by putting an
expression for converting the integer data to volts in the "value" portion, the
integer data in the raw data portion, and a representation of the timebase in
the single dimension portion.

Many applications have been written for visualizing MDSplus signals. Simple but
efficient applications such as dwscope assume the item it is displaying is a
one dimensional vector. The application can simply ask MDSplus for the
evaluation of the value portion of a signal and plot that against the
evaluation of the first dimension portion. More complex visualization tools can
query to determine the dimensionality of the signal and automatically provide
X-Y plots, contour or surface plots or more complicated displays based on the
shape of the signal.

Another feature of MDSplus signals is that they can be subscripted in
expressions where the subscripts are expressed in the units of the dimension.
In the transient recorder example, such a signal could be subscripted to
extract the values within a certain time range. The data returned from a
subscripting operation on a signal is represented as another signal containing
a subset of the original data along with the matching dimensions of this
subset.

Applications can construct signals by using the BUILD_SIGNAL or MAKE_SIGNAL
builtin functions. MAKE_SIGNAL should be used instead of BUILD_SIGNAL in
writing TDI functions to ensure that local variable references are replaced
with the contents of those variable when constructing the signal being returned
to the caller. Three builtin functions are provided to access the various
portions of the signal; VALUE_OF, RAW_OF and DIM_OF. If you attempt to convert
the value of a signal to a primitive type (integer, floating point or text),
MDSplus does an implicit VALUE_OF call to access the value portion of the
signal.

Since each portion of a signal can contain any supported MDSplus datatype, you
can store structures such as "with units" datatypes in the parts of a signal.
This is done when MDSplus stores signals during data acquisition so an
application can find out the units of the various parts of the signal.

The following shows some examples of creating and accessing an MDSplus signal.
These examples are written in TDI (the MDSplus expression evaluator language).

##Example 1

    _MYSIGNAL = BUILD_SIGNAL([1,2,3],*,BUILD_DIM(,[4,5,6]))
    _SUBSET = _MYSIGNAL[4.5:6]
    WRITE(*,_SUBSET)

The above simple example builds a signal with a three element array as the
value and a matching three element array as the dimension. The signal is then
subscripted using a range requesting all the data between 4.5 and 6 in its
dimension. The write statement would output the following: "Build_Signal([2,3],
*, [5,6])". Note the subscripting does not do any interpolation but merely
extracts the values found lying within the ranges of the subscripting.

##Example 2

    _DIGSIG = BUILD_SIGNAL(
             BUILD_WITH_UNITS($VALUE * 1E-3 + .5,"volts"),
             BUILD_WITH_UNITS([100,200,300,397...],"counts"),
             BUILD_DIM(BUILD_WINDOW(0,8191,TRIGGER_NODE),CLOCK_NODE))

Example 2 shows how experimental data read from a transient recorder might be
stored as a signal. The value portion of the signal is an expression using the
special variable, $VALUE, which represents the raw data portion of the same
signal. The dimension description uses a special MDSplus construct for
compactly representing the timestamps of the data which is computed when the
dimension of the signal is evaluated. This dimension construct consisting of a
window data type and a clock reference defines the timebase. How this gets
evaluated into a series of timestamps is explained in the description of the
DTYPE_WINDOW datatype.

##Example 3

    _VALUE = VALUE_OF(_DIGSIG)
    _RAW = RAW_OF(_DIGSIG)
    _TIME = DIM_OF(_DIGSIG)
    _VALUE_UNITS = UNITS_OF(VALUE_OF(_DIGSIG))
    _RAW_UNITS = UNITS_OF(RAW_OF(_DIGSIG))
    _TIME_UNITS = UNITS_OF(DIM_OF(_DIGSIG))
    _VALUE_UNITS = UNITS_OF(_VALUE)

Example 3 illustrates how to access the parts of the signal. Since this signal
had units attached to the parts you can get the units of each part by using the
UNITS_OF function on each part.

##Example 4

    IDL> mdsput,'\image', $
            'BUILD_SIGNAL( BUILD_WITH_UNITS($,"Photons"),,' + $
            'BUILD_WITH_UNITS($,"Sec"), BUILD_WITH_UNITS($,"cm") )' $
            , image_data, t, x

Example 4 illustrates how to build a two dimensional signal with units for
signal (in this case "Photons") and both dimensions (in this case "Sec" and
"cm"). The example is written in IDL. The three variables containing the
data-arrays are image_data(2D), t(1D) and x(1D).


Another important piece of information regarding the signal data type is how
MDSplus handles signals when performing some of the arithmethic builtin
operations on a signal. MDSplus attempts to retain the signal characteristics
or a a signal through operations as long as it can. If you perform a simple
binary operation using a signal and a scalar value (such as mutiply a signal be
the constant 3), MDSplus will return a signal with the original dimensions.
However, if you do a similer operation with another signal, MDSplus will not
make any attempt to reconcile the dimension information either by subscripting
or interpolation. In this case, both signals will be stripped of their
dimensions prior to performing the operation and the operation will take place
as if the two signals were two simple arrays of data. If you know that both
signals had the same dimension then you would have to operate on the signals
and create a new signal using the dimensions of one of the signals as shown
below:

    BUILD_SIGNAL(_SIG1+_SIG2,*,DIM_OF(_SIG1))





The following table lists some of the TDI functions available for creating or
accessing signals:

\latexonly { \tiny \endlatexonly


| **Function**      | **Description**                                                               |
|-------------------|-------------------------------------------------------------------------------|
| BUILD\_SIGNAL     | Build a signal structure                                                      |
| DATA              | Evaluates value portion of signal converting to one of integer, float or text |
| DATA\_WITH\_UNITS | Same as DATA but preserves units                                              |
| DIM\_OF           | Returns the dimension field of a signal                                       |
| MAKE\_SIGNAL      | Make a signal structure                                                       |
| RAW\_OF           | Return the raw data field of signal                                           |

\latexonly } \endlatexonly



