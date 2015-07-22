
Integers Numbers {#dt_integer}
================

MDSplus supports several formats of integers including 8-bit, 16-bit, 32-bit,
64-bit and 128-bit signed and unsigned integers. The data is stored in 2's
complement representation and little endian format. The endian is converted to
big or little endian depending on the computer platform being used to reference
the data. Like most programming languages, when operations are performed with
values of different size, the values are converted to a common size (the
largest of the values being operated on) before performing the operation. For
example, if you were to add a 16-bit integer with a 32-bit integer the 16-bit
integer would first be converted to a 32-bit integer before performing the add
operation.

Integers can be simple scalar values or regular arrays with one to seven
dimensions. When loaded into MDSplus trees, integer arrays may be compressed
using a non-destructive delta compression algorithm automatically depending on
the characteristics of the node in the tree where the data is being stored.
Decompression of this data occurs automatically when the data is referenced.
Raw data from data acquisition hardware is often stored in integer format and
often compresses to one third or less of its orginal size requiring less disk
space for storage and network bandwidth when accessing the data remotely.




Expression evaluator (TDI) operations used on Integer Data Types
----------------------------------------------------------------

**The following table lists some of the more common operations performed on
integer datatypes:**

\latexonly { \tiny \endlatexonly

| **Function**               | **Description**                                                   |
|----------------------------|-------------------------------------------------------------------|
| ABS                        | Absolute Value                                                    |
| ABSSQ                      | Absolute Value Squared                                            |
| ADD (+)                    | Add                                                               |
| ALL                        | Determine if all values are true                                  |
| AND                        | Logical And                                                       |
| AND\_NOT                   | Logical And with negation of second                               |
| ANY                        | Determine if any values are true                                  |
| ARRAY                      | Construct array (uninitialized values)                            |
| BIT\_SIZE                  | Size in bits of data type                                         |
| BSEARCH                    | Binary search in sorted table                                     |
| BTEST                      | Test a bit of a number                                            |
| BYTE                       | Convert to 8-bit signed data type                                 |
| BYTE\_UNSIGNED             | Convert to 8-bit unsigned data type                               |
| CHAR                       | Convert to text character                                         |
| CONDITIONAL (test ? a : b) | Select from 2 sources based on test value                         |
| COUNT                      | Count number of true elements                                     |
| CVT                        | Convert to other data type                                        |
| DATE\_TIME                 | Convert 64-bit internal date format to ascii date and time string |
| DBLE                       | Double the precision, 8-bit values become 16-bit etc.             |
| DECOMPILE                  | Convert to text representation                                    |
| DIAGONAL                   | Create a diagonal matrix                                          |
| DIGITS                     | Number of significant digits                                      |
| DIVIDE (/)                 | Divide                                                            |
| DOT\_PRODUCT               | Dot-product multiplication                                        |
| ELBOUND                    | Lower bound of array                                              |
| EQ (==)                    | Tests for equality                                                |
| EQV                        | Tests for logical equality                                        |
| ESHAPE                     | Return shape of array or scalar                                   |
| ESIZE                      | Total number of elements of array                                 |
| EUBOUND                    | Upper bound of array                                              |
| FLOAT                      | Convert to floating point                                         |
| GE (&gt;=)                 | Test for first greater or equal to second                         |
| GT (&gt;)                  | Test for first greater than second                                |
| HUGE                       | Largest value of this data type                                   |
| IAND (&)                   | Bit-by-bit intersection                                           |
| IAND\_NOT                  | Bit-by-bit intersection with complement of second                 |
| IBCLR                      | Clear one bit to zero                                             |
| IBSET                      | Set one bit to one                                                |
| IEOR                       | Bit-by-bit exclusive or                                           |
| IEOR\_NOT                  | Bit-by-bit exclusive or with complement of second                 |
| IF                         | IF statement                                                      |
| INAND                      | Complement of bit-by-bit intersection                             |
| INAND\_NOT                 | Complement of bit-by-bit intersection with complement of second   |
| INOR                       | Complement of bit-by-bit union                                    |
| INOR\_NOT                  | Complement of bit-by-bit union with complement of second          |
| INOT                       | Complement bit-by-bit                                             |
| INT                        | Convert to signed integer                                         |
| INT\_UNSIGNED              | Convert to unsigned integer                                       |
| IOR (\|)                   | Bit-by-bit inclusive or                                           |
| IOR\_NOT                   | Bit-by-bit union with complement of second                        |
| ISHFT                      | Logical shift                                                     |
| LASTLOC                    | Locate trailing edges of a set of true elements of a logical mask |
| LBOUND                     | Lower bound of array                                              |
| LE (&lt;=)                 | Test for first less than or equal to second                       |
| LOGICAL                    | Convert to logical                                                |
| LONG                       | Convert to 32-bit signed integer                                  |
| LONG\_UNSIGNED             | Convert to 32-bit unsigned integer                                |
| LT (&lt;)                  | Test for first less than second                                   |
| MAP                        | Element selection from an array                                   |
| MAX                        | Maximum of argument list                                          |
| MAXLOC                     | Determine location of maximum value                               |
| MAXVAL                     | Maximum value in an array                                         |
| MEAN                    | Average value of the elements of an array                |
| MERGE                   | Merge two arrays                                         |
| MIN                     | Minimum of argument list                                 |
| MINLOC                  | Determine location of minimum value                      |
| MINVAL                  | Minimum value in an array                                |
| MOD                     | Remainder                                                |
| MULTIPLY (\*)           | Multiply                                                 |
| NAND                    | Negation of logical intersection                         |
| NAND\_NOT               | Negation of logical intersection with negation of second |
| NE (!=)                 | Test for inequality                                      |
| NEQV                    | Test inequality of logical values                        |
| NOR                     | Negation of logical union of elements                    |
| NOR\_NOT                | Negation of logical union with negation of second        |
| NOT (!)                 | Negate a logical                                         |
| OCTAWORD                | Convert to 128-bit value                                 |
| OCTAWORD\_UNSIGNED      | Convert to 128-bit unsigned value                        |
| OR (\|\|)               | Logical union                                            |
| OR\_NOT                 | Logical union with negation of second                    |
| PACK                    | Pack and array under control of mask                     |
| PRODUCT                 | Product of all elements of an array                      |
| QUADWORD                | Convert to 64-bit integer                                |
| QUADWORD\_UNSIGNED      | Convert to 64-bit unsigned integer                       |
| SET\_RANGE              | Set array bounds                                         |
| SHAPE                   | Shape of array                                           |
| SHIFT\_LEFT (&lt;&lt;)  | Bitwise shift of value                                   |
| SHIFT\_RIGHT (&gt;&gt;) | Bitwise shift of value                                   |
| SIGNED                  | Convert to signed integer                                |
| SIZE                    | Total number of elements in an array                     |
| SIZEOF                  | Total number of bytes                                    |
| SORT                    | Make index list of ascending array                       |
| SORTVAL                 | Rearrange array in ascending order                       |
| SPREAD                  | Replicate an array by adding a dimension                 |
| SQUARE                  | Product of number with itself                            |
| SUBSCRIPT               | Pick certain elements of array                           |
| SUBTRACT (-)            | Subtract                                                 |
| SUM                     | Sum all elements of array                                |
| TEXT                    | Convert to text                                          |
| UBOUND                  | Upper bound of array                                     |
| UNION                   | Union of sets keeping only unique values                 |
| UNSIGNED                | Convert to unsigned value                                |
| WORD                    | Convert to 16-bit integer                                |
| WORD\_UNSIGNED          | Convert to 16-bit unsigned integer                       |
| ZERO                    | Create array initialized to zero                         |

\latexonly } \endlatexonly
