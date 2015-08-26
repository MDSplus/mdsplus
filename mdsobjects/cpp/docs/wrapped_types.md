Implemented Data Types  {#wrapped_types}
======================


MDSplus supports numerous data types, from simple numeric or text types to
complex entities which combine related information in a structure. Some of the
data types are used internally for representing things such as data acquisition
devices, compiled representations of expressions and data acquisition actions
and you may likely never have to interact with these items. Most users will
normally be accessing the primitive data types including all integer, floating
point or text data types as well as several of the more complicated MDSplus
data types such as signals, dimensions, with units, with errors and ranges.

>
> To have a complete overview of all the MDSplus data types please refer to:
>
> * \ref data_types "The Reference manual - Data Types section"
> * [The MDSplus site - Data Types section] (http://www.mdsplus.org/index.php?title=Documentation:Users:Datatypes)
>

 Data
------

Each of the data types that have been wrapped in the C++ interface is inherited
form the main class \ref MDSplus::Data. If you browse the manual for that
structure you will find the inheritance graph that shows about every class that
will follow here. The Data class defines many virtual function that are
specialized by overloaded types, it manages the reference counting of the
instanced objects and handle the type conversions from and to the low-level
MSDplus descriptors and TDI bindings.

All the underlying functions that perform the descriptors conversion and the
binding of the TDI function using inline script compilation of expressions is
grouped in the Modules sections: 
 
 - \ref Descrmap 
 - \ref TDIbind


 Integer Numbers
-----------------

All Integer Numbers derive from the \ref MDSplus::Scalar base class. MDSplus
C++ interface supports several formats of integers including 8-bit, 16-bit,
32-bit, 64-bit signed and unsigned integers. Integers can be simple scalar
values or regular arrays with one to seven dimensions. The following is a table
of the integer scalars defined in MDSplus with a link to the proper C++ object
definition.

| **Id**       | **Description**             | **Object**               |
|--------------|-----------------------------|--------------------------|
| DTYPE\_B     | Byte (8-bit)                | \ref MDSplus::Int8       |
| DTYPE\_BU    | Unsigned Byte (8-bit)       | \ref MDSplus::Uint8      |
| DTYPE\_W     | Word (16-bit)               | \ref MDSplus::Int16      |
| DTYPE\_WU    | Unsigned Word (16-bit)      | \ref MDSplus::Uint16     |
| DTYPE\_L     | Long (32-bit)               | \ref MDSplus::Int32      |
| DTYPE\_LU    | Unsigned Long (32-bit)      | \ref MDSplus::Uint32     |
| DTYPE\_Q     | Quadword (64-bit)           | \ref MDSplus::Int64      |
| DTYPE\_QU    | Unsigned Quadword (64-bit)  | \ref MDSplus::Uint64     |
| DTYPE\_O     | Octaword (128-bit)          | ( still missing in c++ ) |
| DTYPE\_OU    | Unsigned Octaword (128-bit) | ( still missing in c++ ) |




 Floating Point Numbers
------------------------

All Floating Point Numbers derive from the \ref MDSplus::Scalar base class.
MDSplus supports several formats of floating point values including several
types of floating values found on OpenVMS systems as well as the more common
IEEE standard floating representations. Floating point data can be stored in
any of the various types of floating point values. The floating point values
will be converted to “native” floating point types when application referent
the data.

Floating point data can be simple scalar values or regular arrays with one to
seven dimensions. When loaded into MDSplus trees, floating point arrays may be
compressed using a non-destructive delta compression algorithm automatically
depending on the characteristics of the node in the tree where the data is
being stored. Decompression of this data occurs automatically when the data is
referenced. While floating point data does not generally compress as well as
integer data, it is not uncommon to see a floating point data reduced to one
half of its original size.


The following is a table of the float scalars defined in MDSplus with a link to
the proper c++ object definition.

| **Id**        | **Description**           | **Scalar object**         |
|---------------|---------------------------|---------------------------|
| DTYPE\_FLOAT  | 32-bit IEEE Float         | \ref MDSplus::Float32     |
| DTYPE\_DOUBLE | 64-bit IEEE Double        | \ref MDSplus::Float64     |
| DTYPE\_FSC    | 32-bit IEEE Float complex | \ref MDSplus::Complex32   |
| DTYPE\_FTC    | 64-bit IEEE Float complex | \ref MDSplus::Complex64   |





 Strings and Text
------------------

The Text data in MDSplus derives from the \ref MDSplus::Scalar base class.
MDSplus supports scalars or arrays of text strings. Text strings are limited to
lengths of 64K characters. The MDSplus c++ string object that implements the
DTYPE_T is \ref MDSplus::String.

### example:

\code{.cpp}
#include <mdsobjects.h>
using namespace MDSplus;
{
  Data * string = new String("lorem ipsum");
  char *c_str = string->GetString();  
  
  Data * string2 = new String(c_str);
  TEST1( string2->equals(string) );
  
  delete []c_str;
  deleteData(string);
  deleteData(string2);
}
\endcode

| **Id**        | **Description**           | **Scalar object**         |
|---------------|---------------------------|---------------------------|
| DTYPE\_T      | Text string               | \ref MDSplus::String      |



 
 Signals
---------

MDSplus provides a signal data type which combines dimension descriptions with
the data. A signal is a structure of three or more fields. The first field is
the "value" field of the signal. This is followed by an optional raw data
field. These two fields can be followed by one or more dimension descriptions.
The number of dimension descriptions in general should match the dimensionality
of the value.

Another feature of MDSplus signals is that they can be subscripted in
expressions where the subscripts are expressed in the units of the dimension.
In the transient recorder example, such a signal could be subscripted to
extract the values within a certain time range. The data returned from a
subscripting operation on a signal is represented as another signal containing
a subset of the original data along with the matching dimensions of this
subset. Since each portion of a signal can contain any supported MDSplus
datatype, you can store structures such as "with units" datatypes in the parts
of a signal. This is done when MDSplus stores signals during data acquisition
so an application can find out the units of the various parts of the signal.


| **Id**        | **Description**           | **Scalar object**         |
|---------------|---------------------------|---------------------------|
| DTYPE\_SIGNAL | Signal                    | \ref MDSplus::Signal      |




 Action / Dispatch
-------------------


 Image
-------


 Call function
---------------
	

 Conglomerate / Device
-----------------------
	

 Dimension definition
-----------------------	



 Dispatch information
----------------------
	

  Function reference
----------------------
	
	
	

 Node Identifier
-----------------
	

 Node specifier (Path)
-----------------------
	

 Range
-------
	

 Routine
---------

	
	

 Window
--------
	



