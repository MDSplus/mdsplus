 MDSplus data types {#data_types}
-------------------

The followings are the MDSplus generic types

  @subpage dt_integer
, @subpage dt_float
, @subpage dt_string
, @subpage dt_call
, @subpage dt_dimension
, @subpage dt_conglom
, @subpage dt_action
, @subpage dt_dispatch
, @subpage dt_function
, @subpage dt_ident
, @subpage dt_method
, @subpage dt_range
, @subpage dt_routine
, @subpage dt_signal
, @subpage dt_window
, @subpage dt_with_error
, @subpage dt_with_units


Any data types used in MDSplus framework are identified by a type id ...

## Data Types for Numbers

\latexonly { \footnotesize \endlatexonly

| **Name**                      | **Description**          |
|-------------------------------|--------------------------|
| \ref dt_integer "DTYPE_BU" | Unsigned Byte (8-bit)       |
| \ref dt_integer "DTYPE_WU" | Unsigned Word (16-bit)      |
| \ref dt_integer "DTYPE_LU" | Unsigned Long (32-bit)      |
| \ref dt_integer "DTYPE_QU" | Unsigned Quadword (64-bit)  |
| \ref dt_integer "DTYPE_OU" | Unsigned Octaword (128-bit) |
| \ref dt_integer "DTYPE_B"  | Byte (8-bit)                |
| \ref dt_integer "DTYPE_W"  | Word (16-bit)               |
| \ref dt_integer "DTYPE_L"  | Long (32-bit)               |
| \ref dt_integer "DTYPE_Q"  | Quadword (64-bit)           |
| \ref dt_integer "DTYPE_O"  | Octaword (128-bit)          |
| \ref dt_float "DTYPE_F"   | 32-bit Vax F_FLOAT           |
| \ref dt_float "DTYPE_D"   | 64-bit Vax D_FLOAT           |
| \ref dt_float "DTYPE_G"   | 64-bit Vax G_FLOAT           |
| \ref dt_float "DTYPE_FC"  | 32-bit Vax F_FLOAT complex   |
| \ref dt_float "DTYPE_DC"  | 64-bit Vax D_FLOAT complex   |
| \ref dt_float "DTYPE_GC"  | 64-bit Vax G_FLOAT complex   |   
| \ref dt_float "DTYPE_FS"  | 32-bit IEEE Float            |
| \ref dt_float "DTYPE_FT"  | 64-bit IEEE Float            |
| \ref dt_float "DTYPE_FSC" | 32-bit IEEE Float complex    | 
| \ref dt_float "DTYPE_FTC" | 64-bit IEEE Float complex    |

\latexonly } \endlatexonly





## Data Types for other MDSplus descriptors

\latexonly { \footnotesize \endlatexonly

| **Name**                                | **Description**                   |
|-----------------------------------------|-----------------------------------|
| \ref dt_string "DTYPE_T"                | Text                              |
| \ref dt_action "DTYPE_ACTION"           | Action definition                 |
| \ref dt_call "DTYPE_CALL"               | Call function                     |
| \ref dt_conglom "DTYPE_CONGLOM"         | Conglomerate / Device             |
| \ref dt_dimension "DTYPE_DIMENSION"     | Dimension definition              |
| \ref dt_dispatch "DTYPE_DISPATCH"       | Dispatch information              |
| \ref dt_function "DTYPE_FUNCTION"       | Function reference                |
| \ref dt_ident "DTYPE_IDENT"             | MDSplus identifier “Ken variable” |
| \ref dt_method "DTYPE_METHOD"           | Invoke device method              |
| DTYPE_NID                               | 32-bit Node Identifier            |
| DTYPE_PARAM                             | Parameter                         |
| DTYPE_PATH                              | Node specifier (Path)             |
| \ref dt_range "DTYPE_RANGE"             | Range                             |
| \ref dt_routine "DTYPE_ROUTINE"         | Routine                           |
| \ref dt_signal "DTYPE_SIGNAL"           | Signal                            |
| \ref dt_window "DTYPE_WINDOW"           | Window                            |
| \ref dt_with_error "DTYPE_WITH_ERROR"   | Attach error to value             |
| \ref dt_with_units "DTYPE_WITH_UNITS"   | Attach units to value             |

\latexonly } \endlatexonly
