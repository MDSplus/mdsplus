
Floating Point Numbers {#dt_float}
======================


MDSplus supports several formats of floating point values including
several types of floating values found on OpenVMS systems as well as the
more common IEEE standard floating representations. Floating point data
can be stored in any of the various types of floating point values. The
floating point values will be converted to “native” floating point types
when application referent the data. You may have applications running on
an OpenVMS system storing data in Vax F\_FLOAT format but an application
running on a Unix system will receive IEEE format data when it accesses
the data.

Like most programming languages, when operations are performed with
values of different size, the values are converted to a common size (the
largest of the values being operated on) before performing the
operation. For example, if you were to add a single precision floating
point value with a double precision value the single precision valu
would first be converted to a double precision before performing the add
operation.

Floating point data can be simple scalar values or regular arrays with
one to seven dimensions. When loaded into MDSplus trees, floating point
arrays may be compressed using a non-destructive delta compression
algorithm automatically depending on the characteristics of the node in
the tree where the data is being stored. Decompression of this data
occurs automatically when the data is referenced. While floating point
data does not generally compress as well as integer data, it is not
uncommon to see a floating point data reduced to one half of its
original size.




Expression evaluator (TDI) operations used on Floating Point Data Types
-----------------------------------------------------------------------

**The following table lists some of the more common operations performed
on floating point datatypes:**

\latexonly { \tiny \endlatexonly
  

| **Function**               | **Description**                                                   |
|----------------------------|-------------------------------------------------------------------|
| ABS                        | Absolute Value                                                    |
| ABS1                       | Absolute Value with L1 norm                                       |
| ABSSQ                      | Absolute Value Squared                                            |
| ACCUMULATE                 | Running Sum                                                       |
| ACOS                       | Arc Cosine                                                        |
| ACOSD                      | Arc Cosine (degrees)                                              |
| ADD (+)                    | Add                                                               |
| AIMAG                      | Imaginary part of complex                                         |
| AINT                       | Truncation to a whole number                                      |
| ANINT                      | Nearest whole number                                              |
| ARG                        | Argument of complex number in radians                             |
| ARRAY                      | Construct array (uninitialized values)                            |
| ASIN                       | Arc Sine                                                          |
| ASIND                      | Arc Sine (degrees)                                                |
| ATAN                       | Arc Tangent                                                       |
| ATAN2                      | Arc Tangent                                                       |
| ATAND                      | Arc Tangent (degrees)                                             |
| ATAN2D                     | Arc Tangent (degrees)                                             |
| ATANH                      | Hyperbolic Arc Tangent (degrees)                                  |
| BSEARCH                    | Binary search in sorted table                                     |
| BYTE                       | Convert to 8-bit signed data type                                 |
| BYTE\_UNSIGNED             | Convert to 8-bit unsigned data type                               |
| CEILING                    | Smallest whole number above argument                              |
| CMPLX                      | Convert to complex                                                |
| CONDITIONAL (test ? a : b) | Select from 2 sources based on test value                         |
| CONJG                      | Conjugate of complex number                                       |
| COS                        | Cosine                                                            |
| COSD                       | Cosine (degrees)                                                  |
| COSH                       | Hyperbolic Cosine                                                 |
| CVT                        | Convert to other data type                                        |
| DBLE                       | Double the precision, 8-bit values become 16-bit etc.             |
| DECOMPILE                  | Convert to text representation                                    |
| DIAGONAL                   | Create a diagonal matrix                                          |
| DIGITS                     | Number of significant digits                                      |
| DIM (/)                    | Positive difference                                               |
| DIVIDE (/)                 | Divide                                                            |
| DOT\_PRODUCT               | Dot-product multiplication                                        |
| DPROD (/)                  | Double precision product                                          |
| DTYPE\_RANGE (/)           | Build range                                                       |
| D\_COMPLEX (/)             | DTYPE\_D Complex                                                  |
| D\_FLOAT (/)               | Convert to DTYPE\_D                                               |
| ELBOUND                    | Lower bound of array                                              |
| EPSILON (/)                | Smallest positive value                                           |
| EQ (==)                    | Tests for equality                                                |
| ESHAPE                     | Return shape of array or scalar                                   |
| ESIZE                      | Total number of elements of array                                 |
| EUBOUND                    | Upper bound of array                                              |
| EXP                        | Exponential                                                       |
| EXPONENT                   | Exponent                                                          |
| FINITE                     | True if not missing or reserved value                             |
| FIX\_ROPRAND               | Replace missing or reserved value                                 |
| FLOAT                      | Convert to floating point                                         |
| FLOOR                      | Largest whole number less than or equal to arg                    |
| FRACTION                   | Fractional part                                                   |
| F\_COMPLEX                 | Convert to DTYPE\_F complex                                       |
| F\_FLOAT                   | Convert to DTYPE\_F                                               |
| FS\_COMPLEX                | Convert to DTYPE\_FS complex                                      |
| FS\_FLOAT                  | Convert to DTYPE\_FS                                              |
| FT\_COMPLEX                | Convert to DTYPE\_FT complex                                      |
| FT\_FLOAT                  | Convert to DTYPE\_FT                                              |
| GE (&gt;=)                 | Test for first greater or equal to second                         |
| GT (&gt;)                  | Test for first greater than second                                |
| G\_COMPLEX                 | Convert to DTYPE\_G complex                                       |
| G\_FLOAT                   | Convert to DTYPE\_G                                               |
| HUGE                       | Largest value of this data type                                   |
| INT                        | Convert to signed integer                                         |
| INT\_UNSIGNED              | Convert to unsigned integer                                       |
| LASTLOC                    | Locate trailing edges of a set of true elements of a logical mask |
| LBOUND                     | Lower bound of array                                              |
| LE (&lt;=)                 | Test for first less than or equal to second                       |
| LOG                        | Natural logarithm                                                 |
| LOG10                      | Logarithm base 10                                                 |
| LOG2               | Logarithm base 2                          |
| LONG               | Convert to 32-bit signed integer          |
| LONG\_UNSIGNED     | Convert to 32-bit unsigned integer        |
| LT (&lt;)          | Test for first less than second           |
| MAP                | Element selection from an array           |
| MAX                | Maximum of argument list                  |
| MAXEXPONENT        | Maximum exponent                          |
| MAXLOC             | Determine location of maximum value       |
| MAXVAL             | Maximum value in an array                 |
| MEAN               | Average value of the elements of an array |
| MERGE              | Merge two arrays                          |
| MIN                | Minimum of argument list                  |
| MINEXPONENT        | Minimum exponent                          |
| MINLOC             | Determine location of minimum value       |
| MINVAL             | Minimum value in an array                 |
| MOD                | Remainder                                 |
| MULTIPLY (\*)      | Multiply                                  |
| NE (!=)            | Test for inequality                       |
| OCTAWORD           | Convert to 128-bit value                  |
| OCTAWORD\_UNSIGNED | Convert to 128-bit unsigned value         |
| PACK               | Pack and array under control of mask      |
| POWER              | Raise number to power                     |
| PRECISION          | The decimal precision                     |
| PRODUCT            | Product of all elements of an array       |
| QUADWORD           | Convert to 64-bit integer                 |
| QUADWORD\_UNSIGNE  | Convert to 64-bit unsigned integer        |
| RADIX              | The base of the datatype                  |
| RANDOM             | Random number                             |
| RANGE              | The range of the datatype                 |
| RANK               | Number of dimensions                      |
| REAL               | Convert to real                           |
| RRSPACING          | The reciprocal of the spacing of datatype |
| SCALE              | Change exponent                           |
| SET\_EXPONENT      | Change exponent                           |
| SET\_RANGE         | Set array bounds                          |
| SHAPE              | Shape of array                            |
| SIGNED             | Convert to signed integer                 |
| SIN                | Sine                                      |
| SIND               | Sine (degrees)                            |
| SINH               | Hyperbolic Sine                           |
| SIZE               | Total number of elements in an array      |
| SIZEOF             | Total number of bytes                     |
| SORT               | Make index list of ascending array        |
| SORTVAL            | Rearrange array in ascending order        |
| SPACING            | Absolute spacing near arg                 |
| SPREAD             | Replicate an array by adding a dimension  |
| SQRT               | Square root                               |
| SQUARE             | Product of number with itself             |
| SUBSCRIPT          | Pick certain elements of array            |
| SUBTRACT (-)       | Subtract                                  |
| SUM                | Sum all elements of array                 |
| TAN                | Tangent                                   |
| TAND               | Tangent (degrees)                         |
| TANH               | Hyperbolic Tangent                        |
| TEXT               | Convert to text                           |
| TINY               | Smallest positive number                  |
| UBOUND             | Upper bound of array                      |
| UNION              | Union of sets keeping only unique values  |
| UNSIGNED           | Convert to unsigned value                 |
| WORD               | Convert to 16-bit integer                 |
| WORD\_UNSIGNED     | Convert to 16-bit unsigned integer        |
| ZERO               | Create array initialized to zero          |

\latexonly } \endlatexonly
