
Strings {#dt_string}
=======

MDSplus supports scalars or arrays of text strings. Text strings are limited to
lengths of 64K characters. 


Expression evaluator (TDI) operations used on Text Data Type
------------------------------------------------------------

**The following table lists some of the more common operations performed on the
text datatype:**

\latexonly { \tiny \endlatexonly


| **Function**       | **Description**                                |
|--------------------|------------------------------------------------|
| ADJUSTL            | Adjust string left                             |
| ADJUSTR            | Adjust string right                            |
| BSEARCH            | Binary search                                  |
| BUILD\_PATH        | Convert string to node reference               |
| BUILD\_WITH\_UNITS | Attach units to value                          |
| COMPILE            | Compile string into TDI expression             |
| CONCAT             | Concatenate strings                            |
| DATE\_TIME         | Convert quadword internal time to string       |
| DECOMPILE          | Convert compiled expression or value to string |
| ELEMENT            | Extract element of string                      |
| EQ \[==\]          | Test for equality                              |
| EXECUTE            | Compile and evaluate string                    |
| EXTRACT            | Extract portion of string                      |
| FOPEN              | Open a file                                    |
| GE \[&gt;=\]       | Test for greater or equal                      |
| GT \[&gt;\]        | Test for greater                               |
| INDEX              | Locate substring in string                     |
| LE \[&lt;=\]       | Test for less or equal                         |
| LEN                | Length of string                               |
| LEN\_TRIM          | Length of string without trailing white space  |
| LGE                | Test for greater or equal                      |
| LGT                | Test for greater                               |
| LLE                | Test for less or equal                         |
| LLT \[&lt;=\]      | Test for less                                  |
| LT \[&lt;\]        | Test for less                                  |
| MAKE\_WITH\_UNITS  | Attach units to value                          |
| NE \[!=\]          | Test for not equal                             |
| REPEAT \[&gt;\]    | Concatenate copies of string                   |
| SCAN               | Scan a string for character in set             |
| SORT               | Make index list of sorted elements in array    |
| SORTVAL            | Sort elements in array                         |
| TEXT               | Convert to text                                |
| TRANSLATE          | Replace matching characters                    |
| TRIM               | Remove trailing white space                    |
| UNION              | Reduce array to unique values                  |
| UNITS              | Get the units                                  |
| UNITS\_OF          | Get the units                                  |
| UPCASE             | Convert to uppercase                           |
| VERIFY             | Verify that a set of characters in string      |


\latexonly } \endlatexonly
