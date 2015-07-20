Getting Native Types from MDSplus Data Objects {#native_types}
==============================================


Even if a data item may be represented in MDSplus by a possible complex type,
this knowledge is not required in normal operation. Normally, when reading data
stored in a tree, we are interested in getting the final result, regardless of
the way data have been stored. So, for example, if we want to retrieve a data
item which is expected to represent an array of float values, we would like to
do it directly. This is carried out by the MDSplus object framework by defining
a set of basic data conversion methods in the Data class itself. They are the
following:

  *  \ref MDSplus::Data::getByte() Convert the Data instance into a byte integer
  *  \ref MDSplus::Data::getShort() Convert the Data instance into a short integer
  *  \ref MDSplus::Data::getInt() Convert the Data instance into an integer
  *  \ref MDSplus::Data::getLong() Convert the Data instance into a long integer
  *  \ref MDSplus::Data::getFloat() Convert the Data instance into a single precision float
  *  \ref MDSplus::Data::getDouble() Convert the Data instance into a double precision float
  *  \ref MDSplus::Data::getString() Convert the Data instance into a string 

and, when we are expecting arrays:

  *  \ref MDSplus::Data::getByteArray() Convert the Data instance into a byte integer array
  *  \ref MDSplus::Data::getShortArray() Convert the Data instance into a short integer array
  *  \ref MDSplus::Data::getIntArray() Convert the Data instance into an integer array
  *  \ref MDSplus::Data::getLongArray() Convert the Data instance into a long integer array
  *  \ref MDSplus::Data::getFloatArray() Convert the Data instance into a single precision float array
  *  \ref MDSplus::Data::getDoubleArray() Convert the Data instance into a double precision float array 


Suppose that we want to read in a float array the content of tree node NODE1.
This can be done as follows:

\code{cpp}
#include <mdsobjects.h>
using namespace MDSplus;

{
 Tree *myTree = new Tree("my_tree", -1);
 TreeNode *node = myTree ->getNode("NODE1");
 Data *nodeData = node->getData();
 int numElements;
 float *retArray = nodeData->getFloatArray(&numElements);
}
\endcode

In the above example, the data() method returns a scalar or array of native
numeric or string data.


Scalar conversions limits
-------------------------

For each possible scalar conversion, from one native type to another one, a
specific cast function was implemented to check if the conversion is not
meaningless. If no accepted conversion was found the library throws an
exception identifying the error status. For instance a Int8 n = -1 casted to
Uint32 will throw a std::underflow_error.
 
All conversion checks is managed by a template specialization of the
MDSplus::numeric_cast() function. This function uses the std::numeric_limits to
get boundaries for the source and target numeric domains of the specific cast,
so at the end the test is done using the standard template library numeric
traits. The implementation was done using very few type traits and the standard
std::numeric_limits class.
 
The traits first looks the source and the target types if they are integer
number or float, then it checks for these special type properties:
 
 - is_integer Target or Source
 - is_coercion → Source::digits > Target::digits 
 - is_u2s → Source::is_unsiged && Target::is_signed 
 - is_s2u → Source::is_signed && Target::is_unsigned
 
Four checks are selected by traits to be performed on values:
 
 - numeric_cast_max_rule 
 - numeric_cast_min_rule
 - numeric_cast_precision_rule (disabled at the moment) 
 - numeric_cast_nan_rule
 
The checks are performed according to the diagram below:
 
 \image html img/numeric_cast.jpg
 \image latex img/numeric_cast.jpg width=8cm 
 
 
 The possible exception that the function throws are:
 
 | Exception             | Description                              |
 |-----------------------|------------------------------------------|
 | std::overflow_error   | exceeded the upper limitof target domain |
 | std::underflow_error  | exceeded the lower limitof target domain |
 | std::range_error      | nan or inf converted to integer domain   |


For example :

\code{.cpp}
#include <mdsobjects.h>
using namespace MDSplus;

int main() {
    AutoData<Data> si = new Int32(-1);
    try { uint64_t ui = si->getLongUnsigned(); }
    catch(std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
\endcode








