Dimension Data Type {#dt_dimension}
===================


MDSplus provides a dimension data type which provides a compact mechanism for
expressing signal dimensions (See: DTYPE_SIGNAL). The dimension data type was
implemented as a way to represent data such as the timebase for signals
recorded by data acquisition equipment such as transient recorders. These
devices typically have a clock (either a separate external device or built into
the transient recorder) which tells the device to record a sample into its
internal cyclic memory buffer. In many cases this clock is a single speed clock
which means each clock pulse occurs at a constant delta time from the previous
clock pulse. A transient digitizer has another input called a trigger which
tells the device to stop recording samples (either immediately or after some
number of subsequent clock pulses). If one knows when the clock began running
and the delta time between each clock pulse along with the time the trigger
took place, one could compute the time that each sample in the digitizers
internal memory was recorded. MDSplus could compute these times when storing
the data for the device and store these times along with the data in a signal
however it is much more efficient to store a representation of this timebase
which could be evaluated as needed. This is the purpose of the dimension data
type.

A dimension data type is a structure which has two parts, a window and an axis.
The axis part is a representation of a series of values (i.e. time stamps) and
is generally represented by a DTYPE_RANGE data item. If this was a single speed
clock, for example, the axis would be represented by a range consisting or an
optional start time, an optional end time and a single delta time value. This
range could represent a series of clock pulses which began some time infinitely
in the past and continuing to some time infinitely in the future. The window
portion of the dimention is used to select a set of these infinite stream of
clock pulses that represent those clock pulses which match the samples that
were recorded in the digitizers internal memory. The window portion is usually
represented by a DTYPE_WINDOW data item. A window consists of a start index,
and end index and a value at index 0. For a transient digitizer, the samples in
the memory were recorded at a known number of clock pulses before the trigger
(the start index) and continuing a known number of clock pulses after the
trigger. The time the module was triggered is the value at index 0 part of the
window.

The best way to explain how a dimension works is to give an example. Let us
imagine we had an external clock that just ticks continously every second. If
we hooked this clock to a transient digitizer and told that digitizer to start
recording, every second it would record the voltage level of its inputs into
its circular memory buffer. It would continue to record these samples for ever
until told to stop. We can represent this unbounded time base using a
DTYPE_RANGE data item:

    * : * : 1.0
 
or

    BUILD_RANGE(*,*,1.0)

If we asked MDSplus to evaluate this time base we would get an error since it
represents an infinite stream of values with each value being one larger than
the previous value.

Now if the digitizer is configured to stop recording values when it receives a
trigger input it should be possible to compute when each sample in its buffer
was recorded (approximately since in this example we don't know the exact time
of each clock pulse). If we label the trigger time in this example as time 0.0
and for purposes of illustration say the digitizer can record ten samples, we
can represent the time base using:

    _CLOCK = * : * : 1.0
    _TRIGGER = 0.0
    BUILD_DIM(BUILD_WINDOW(-9,0,_TRIGGER), _CLOCK)

The clock pulses occurring close to the time the module was triggered would be
be times such as ...,-19,-18,-17,-16,...,-4,-3,-2,-1,0,1,2,3,... an so on.
Since the device was told to stop recording approximately at the time 0.0, the
ten samples that have been recorded in the digitizers memory would have
occurred at -9,-8,-7,-6,-5,-4,-3,-2,-1,0. These are the values that MDSplus
would return if the above dimension was evaluated.

Of course, there are numerous ways this same information could have been
represented. However, one thing the special about the dimension concept is that
the trigger time and the clock representation does not need to be known when
the dimension item is stored. They can be simply node references to pieces of
information which is stored by other devices which are responsible for
generating the trigger and clocks. The implementation of a transient digitizer
in MDSplus can be implemented independently of trigger devices and clock
devices. It knows only the number of pre-trigger and post-trigger samples it
takes and therefore can store a dimension item with a window indicating the
start index and end index and simply use node references for the trigger and
clock.

The dimension data type can obviously be used for more than just representing
time stamps of data recorded by transient digitizers. This compact
representation can be used for storing dimension information of almost any kind
of signals. The axis portion of the dimension does not have to be a regular
continuous range. It can be an unbounded or bounded range, an simple array of
values or an expression returning a range or array of values. The window
portion of a dimension can be missing if the axis portion is finite.

The dimension data type is very important when you use the MDSplus subscripting
capability. MDSplus signals can be subscripted using bounds expressed in units
of its dimensions. For example:

    mysignal[.3:.9]

This would extract the subset of the signal which has dimension values between
.3 and .9. MDSplus subscripting behaves slightly different when subscripting a
signal which as an array of values for the dimension versus a dimension item. A
signal that looks like build_signal(y-values,*,x-values-array) is treated like
a list of discrete values while a signal such as build_signal(y-values, *,
build_dim(*,x-values-array)) is treated as a continous signal. If a range
without a delta is used to subscript the former signal it evaluates the range
with the default increment of one and uses the result to pick the discrete
values from the signal. In the latter case, it treats the range as a beginning
and ending and extracts the portion of the signal within this range. In most
cases the latter behavior is desired so you should take care to store signals
using dimension items for the dimensions.

    _signal=build_signal([1,2,3,4,5],*,[.1,.2,.3,.4,.5])
    _signal[.2:.4]

    result = build_signal([2],*,[.2])

    _signal=build_signal([1,2,3,4,5],*,
            build_dim(*,[.1,.2,.3,.4,.5]))
    _signal[.2:.4]

    result = build_signal([2,3,4],*,[.2,.3,.4])




The following table lists some of the TDI functions available for creating or
accessing dimensions:

\latexonly { \tiny \endlatexonly

| **Function**  | **Description**                                                      |
|---------------|----------------------------------------------------------------------|
| AXIS\_OF      | Return axis portion of dimension                                     |
| BUILD\_DIM    | Build a dimension structure                                          |
| BUILD\_SIGNAL | Build a signal structure                                             |
| BUILD\_WINDOW | Build a window structure                                             |
| DATA          | Evaluates dimension item converting to one of integer, float or text |
| DIM\_OF       | Return one of the dimension parts of a signal structure              |
| MAKE\_DIM     | Make a dimension structure                                           |
| WINDOW\_OF    | Return the window field of a dimension structure                     |

\latexonly } \endlatexonly




