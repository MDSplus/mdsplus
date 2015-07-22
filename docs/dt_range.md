Range {#dt_range}
=====

MDSplus provides a range data type which provides a compact mechanism for
expressing series of values with fixed intervals. The range data type is a
structure consisting of three parts; begin, end, and delta. This data type is
used often in MDSplus for representing data such as the output of clock devices
used in data acquisition. The following example shows three simple instances of
ranges:

    _range1 = 1 : 10
    _range1b = build_range(1,10,*)
    _range2 = 1 : 10 : .5
    _range2b = build_range(1,10,.5)
    _range3 = * : * : 1E-6
    _range3b = build_range(*,*,1E-6)

In the above examples, we show two formats for creating ranges. The MDSplus
expression evaluator has a built in syntax for specifying ranges which is:
begin : end [: delta]. In the first two examples (_range1 and _range1b) a range
is created with a beginning of 1 and and ending of 10 with no delta specified.
If the delta is missing, it defaults to a delta of 1. If these ranges were
evaluated using data(_range1), for example, it would result in an integer array
of ten values: [1,2,3,4,5,6,7,8,9,10]. The next examples (_range2 and _range2b)
specify a delta of .5. In this case the result when evaluated would be an array
of 19 values: [1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5,
8., 8.5, 9., 9.5, 10.]. The last two examples (_range3 and _range3b) show a
range with no beginning or end but with a delta of 1E-6. This represents an
infinite ramp of values beginning at minus infinity and ending in positive
infinity with each value being separated by 1E-6. Obviously you could not
evaluate this range to get an array of values since the array would have an
inifinite number of elements. This type of range can be used however as the
axis part of a dimension data item where the dimension's window part is used to
select a finite segment of this infinite list of values.

Ranges can be much more complex in MDSplus by specifying vectors for each of
the fields. For example:

    _range = build_range([1.,100.],[5.,105.],[.5,1.])

When you evaluate the above example using data(_range), you will get the
following array of values: [1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 100., 101.,
102., 103., 104., 105.]. The range describes two ramp segments: 1 : 5 : .5 and
100 : 105 : 1. The following might represent a two speed clock which shifts
frequence at time 0:

    _clock = build_range([$ROPRAND,0.],[0.,$ROPRAND],[1E-3,1E-6])

Like the example _range3 above, this range could not be evaluated independently
and would need to be part of a dimension item with a window selecting a finite
set of the values. The $ROPRAND value shown in this example is a mechanism for
indicating missing values in arrays of floating point data. This example shows
two range segments: * : 0. : 1E-3 and 0. : * : 1E-6.



The following table lists some of the TDI functions available for creating or
accessing ranges:

\latexonly { \tiny \endlatexonly

| **Function** | **Description**                             |
|--------------|---------------------------------------------|
| BEGIN\_OF    | Return begin portion of a range             |
| BUILD\_RANGE | Build a range structure                     |
| END\_OF      | Build a signal structure                    |
| MAKE\_RANGE  | Make a range structure                      |
| SLOPE\_OF    | Return the delta field of a range structure |

\latexonly } \endlatexonly
