#ifndef MDSOBJECTS_H
#define MDSOBJECTS_H
//#define NOMINMAX
#include <algorithm>
#include <complex>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
 #include <windows.h>
#else
 #include <sys/types.h>
 //#include <sys/ipc.h>
 //#include <sys/sem.h>
 //#include <semaphore.h>
#endif

#include <mdsplus/mdsconfig.h> // should be removed from here //
#include <dbidef.h>
#include <ncidef.h>
#include <mdstypes.h>
#include <usagedef.h>

#include <mdsplus/mdsplus.h>
#include <mdsplus/ConditionVar.hpp>
#include <mdsplus/Mutex.hpp>
#include <mdsplus/numeric_cast.hpp>
#include <mdsplus/AutoPointer.hpp>

///@{
///
///  NON TROWING EXCEPTION MACRO

# if __cplusplus >= 201103L
#  define NOEXCEPT noexcept
# else
#  define NOEXCEPT throw()
#  ifndef nullptr
#    define nullptr NULL
#  endif
# endif

///@}


///@{
///
/// DESCRIPTORS CODE DEFINITIONS


#define DTYPE_BU 2
#define DTYPE_WU 3
#define DTYPE_LU 4
#define DTYPE_QU 5
#define DTYPE_OU 25
#define DTYPE_B 6
#define DTYPE_W 7
#define DTYPE_L 8
#define DTYPE_Q 9
#define DTYPE_O 26
#define DTYPE_FLOAT 52
#define DTYPE_DOUBLE 53
#define DTYPE_F	10
#define DTYPE_D	11
#define DTYPE_G	27
#define DTYPE_H	28
#define DTYPE_FSC 54
#define DTYPE_FTC 55
#define DTYPE_T 14
#define DTYPE_IDENT 191
#define DTYPE_NID 192
#define DTYPE_PATH 193
#define DTYPE_PARAM 194
#define DTYPE_SIGNAL 195
#define DTYPE_DIMENSION 196
#define DTYPE_WINDOW 197
#define DTYPE_SLOPE 198
#define DTYPE_FUNCTION 199
#define DTYPE_CONGLOM 200
#define DTYPE_RANGE 201
#define DTYPE_ACTION 202
#define DTYPE_DISPATCH 203
#define DTYPE_PROGRAM 204
#define DTYPE_ROUTINE 205
#define DTYPE_PROCEDURE 206
#define DTYPE_METHOD 207
#define DTYPE_DEPENDENCY 208
#define DTYPE_CONDITION 209
#define DTYPE_EVENT 210
#define DTYPE_WITH_UNITS 211
#define DTYPE_CALL 212
#define DTYPE_WITH_ERROR 213
#define DTYPE_LIST 214
#define DTYPE_TUPLE 215
#define DTYPE_DICTIONARY 216
#define DTYPE_POINTER 51
#define DTYPE_DSC	24

///@}


#define TreeNEGATE_CONDITION 	7
#define TreeIGNORE_UNDEFINED 	8
#define TreeIGNORE_STATUS	9
#define TreeDEPENDENCY_AND	10
#define TreeDEPENDENCY_OR	11
#define CLASS_S 1
#define CLASS_D 2
#define CLASS_A 4
#define CLASS_R 194
#define CLASS_APD 196
#define MAX_DIMS 32

extern "C" char *MdsGetMsg(int status);

/*
extern "C" {
    void *convertToScalarDsc(int clazz, int dtype, int length, char *ptr);
    void *evaluateData(void *dscPtr, int isEvaluate, int *status);
    void freeDsc(void *dscPtr);
    void *convertFromDsc(void *dscPtr);
    char *decompileDsc(void *dscPtr);
    char *decompileDsc(void *dscPtr);
    void *compileFromExprWithArgs(char *expr, int nArgs, void *args, void *tree);
    void freeChar(void *);
    void *convertToArrayDsc(int clazz, int dtype, int length, int l_length, int nDims, int *dims, void *ptr);
    void *convertToCompoundDsc(int clazz, int dtype, int length, void *ptr, int ndescs, void **descs);
    void *convertToApdDsc(int type, int ndescs, void **ptr);
    void *deserializeData(char *serialized, int size);
    void * convertToByte(void *dsc);
    void * convertToShort(void *dsc);
    void * convertToInt(void *dsc);
    void * convertToLong(void *dsc);
    void * convertToFloat(void *dsc);
    void * convertToDouble(void *dsc);
    void * convertToShape(void *dcs);
}
*/


namespace MDSplus  {

// Required for handling dynamic memory allocated in a different DLL on windows
// in Debug configuration
// MUST revert to old version. Winodws VS crashes otherwise!! The code MUST be moved from include

class Connection;
class Tree;

EXPORT void setActiveTree(Tree *tree);
EXPORT Tree *getActiveTree();







////////////////////////////////////////////////////////////////////////////////
// MDS EXCEPTION ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief The MdsException class
/// Extends stl exception class and holds a message accessible via virtual funciont what()
///
class EXPORT MdsException : public std::exception {

    friend EXPORT std::ostream & operator << (std::ostream &outStream, MdsException &exc) {
        return outStream << exc.what();
    }

public:

    MdsException(const char *msg): msg(msg) { }
    //MdsException(int status): msg(MdsGetMsg(status)) { }

    MdsException(int status);

    virtual ~MdsException() NOEXCEPT { }

    ///
    /// \brief virtual function to get exception message
    /// \return error message
    virtual const char* what() const NOEXCEPT
    {
        return msg;
    }

protected:

    const char *msg;
};



////////////////////////////////////////////////////////////////////////////////
// MDS DATA CLASS //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class Data;

/// Decrements the data reference counter and if last ref is reached deletes
/// the data instance and free the allocated memory. This function must be
/// called each time the code is responsible for the object deletion.
///
EXPORT void deleteData(Data *);

///
/// \brief Data objects abstract baseclass class
///
/// Every data in a tree is also a subclass of the generic Data class.
/// Subinstances of units, error, help and validation Data are also managed.
///
/// This class implements also a reference counting feature of Data objects
/// though the overloading of new and delete operators.
///
///
class EXPORT Data
{
public:

    /// Default constructor,s reference counting is set to 1 and the data to
    /// "changed" state.
    ///
    Data():
        refCount(1),
        units(nullptr),
        error(nullptr),
        help(nullptr),
        validation(nullptr)
    {}

    virtual ~Data();

    //    void *operator new(size_t sz);
    //    void operator delete(void *p);

    static void incRefCount(Data * d);
    static void decRefCount(Data * d);
    void incRefCount();
    void decRefCount();

    virtual Data *getUnits();
    virtual Data *getError();
    virtual Data *getHelp();
    virtual Data *getValidation();
    virtual void setUnits(Data *in);
    virtual void setError(Data *in);
    virtual void setHelp(Data *in);
    virtual void setValidation(Data *in);

private:

    /// Protect the use of copy constructor not permitted for Data objects, use
    /// \ref clone() function instead.
    Data (const Data &) {}

    ///@{
    /// Friendship declaration for TDI expression compilation
    friend EXPORT Data *compile(const char *expr);
    friend EXPORT Data *compileWithArgs(const char *expr, int nArgs ...);
    friend EXPORT Data *compile(const char *expr, Tree *tree);
    friend EXPORT Data *compileWithArgs(const char *expr, Tree *tree, int nArgs ...);
    friend EXPORT Data *execute(const char *expr);
    friend EXPORT Data *executeWithArgs(const char *expr, int nArgs ...);
    friend EXPORT Data *execute(const char *expr, Tree *tree);
    friend EXPORT Data *executeWithArgs(const char *expr, Tree *tree, int nArgs ...);
    friend EXPORT Data *deserialize(char const * serialized);
    friend EXPORT Data *deserialize(Data *serialized);
    ///@}

    virtual void propagateDeletion(){}

public:
    // NOTE: [andrea] why public?
    int clazz;    ///< data class member
    int dtype;    ///< data type member
    int refCount; ///< reference counting member

    /// Pure virtual function matching data to a specific MDS descriptor
    /// \return MDS descriptor structure
    /// \ingroup Descrmap
    virtual void *convertToDsc() = 0;

    /// Complete conversion to csc by condsidering help, units and error
    /// implementations of convertToDsc() calls this completion function
    /// \ingroup Descrmap
    void * completeConversionToDsc(void *dsc);

    /// c style access to internal data members
    virtual void getInfo(char *clazz, char *dtype,
                         short *length = NULL,
                         char *nDims = NULL,
                         int **dims = NULL,
                         void **ptr = NULL)
    {
        *clazz = this->clazz;
        *dtype = this->dtype;
        if(length) *length = 0;
        if(nDims) *nDims = 0;
        if(dims) *dims = 0;
        if(ptr) *ptr = 0;
    }

    /// \return true if this and data match, false otherwise and as default
    virtual bool equals(Data *data UNUSED_ARGUMENT) { return false; }

    /// \return Return the result of TDI evaluate
    Data *evaluate();

    /// eports TDI data functionality, i.e. returns a native type
    /// (scalar or array).
    virtual Data *data();

    /// Return the result of TDI decompile
    char *decompile();

    /// Make a dymanically allocated copy of the Data instance Tee
    Data *clone();

    /// serialize data into non terminated char array using serializeData
    /// function, if serializeData fails MdsException is trown.
    ///
    /// \see serializeData(void *dsc, int *retSize, void **retDsc)
    /// \param size set to serialized data length
    /// \return c string representing serialized data
    char *	serialize(int *size);

    ///@{ \ingroup NativeTypes
    ///
    /// Access data as native type using getData() conversion function. This
    /// function might be overloaded by derived data classes to improve
    /// conversion.
    ///
    virtual char getByte();
    virtual short getShort();
    virtual int getInt();
    virtual int64_t getLong();
    virtual unsigned char getByteUnsigned();
    virtual unsigned short getShortUnsigned();
    virtual unsigned int getIntUnsigned();
    virtual uint64_t getLongUnsigned();
    virtual float getFloat();
    virtual double getDouble();
    virtual std::complex<double> getComplex() {
        throw MdsException("getComplex() not supported for non Complex data types"); }
    virtual char * getString(){return decompile();}
    virtual int * getShape(int *numDim);
    virtual char *getByteArray(int *numElements);
    virtual std::vector<char> getByteArray();
    virtual short * getShortArray(int *numElements);
    virtual std::vector<short> getShortArray();
    virtual int * getIntArray(int *numElements);
    virtual std::vector<int> getIntArray();
    virtual int64_t * getLongArray(int *numElements);
    virtual std::vector<int64_t> getLongArray();
    virtual float * getFloatArray(int *numElements);
    virtual std::vector<float> getFloatArray();
    virtual unsigned char *getByteUnsignedArray(int *numElements);
    virtual std::vector<unsigned char> getByteUnsignedArray();
    virtual unsigned short *getShortUnsignedArray(int *numElements);
    virtual std::vector<unsigned  short> getShortUnsignedArray();
    virtual unsigned int *getIntUnsignedArray(int *numElements);
    virtual std::vector<unsigned int> getIntUnsignedArray();
    virtual uint64_t *getLongUnsignedArray(int *numElements);
    virtual std::vector<uint64_t> getLongUnsignedArray();
    virtual double * getDoubleArray(int *numElements);
    virtual std::vector<double> getDoubleArray();
    virtual std::complex<double> * getComplexArray(int *numElements UNUSED_ARGUMENT) {
        throw MdsException("getComplexArray() not supported for non Complex data types"); }
    virtual std::vector<std::complex<double> > getComplexArray();
    virtual char ** getStringArray(int *numElements) {
        *numElements = 0; return NULL; }
    ///@}

    virtual Data *getDimensionAt(int dimIdx);

    virtual int getSize() { return 1; }

    /// Instantiate a Scope object and plot data into.
    virtual void plot();

    friend EXPORT std::ostream & operator << (std::ostream &outStream, MDSplus::Data *data);

protected:

    /// readonly access
    virtual bool isImmutable() { return true; }

    /// Set accessory data (utits, error, help and validation)
    void setAccessory(Data *units, Data *error, Data *help, Data *validation)
    {
        setUnits(units);
        setError(error);
        setHelp(help);
        setValidation(validation);
    }

private:

    /// Execute data conversion selecting proper descriptor conversion function.
    /// If conversion fails a MdsException is thrown.
    /// \param classType target class type
    /// \param dataType target data type
    Data * getData(int classType, int dataType);

    /// convert data to array type (CLASS_A) \see getData()
    Data * getArrayData(int dataType);

    /// convert data to scalar type (CLASS_S) \see getData()
    Data * getScalarData(int dataType);

    ///
    /// @{
    /// Accessory information added on this data
    friend void deleteData(Data *data);
    Data * units;
    Data * error;
    Data * help;
    Data * validation;
    //// @}
};



////////////////////////////////////////////////////////////////////////////////
// SCALAR DATA /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Scalar class
///
/// Scalar is the common base class for all scalar data types
/// (descriptor class CLASS_S).
class EXPORT Scalar: public Data
{
    friend class Array;
protected:

    int length;
    char *ptr;

public:
    Scalar(): length(0), ptr(0) { clazz = CLASS_S; }


    virtual ~Scalar()
    {
        delete []ptr;
    }

    /// convert data to scalar descriptor
    virtual void *convertToDsc();

    virtual void getInfo(char *clazz, char *dtype,
                         short *length = NULL,
                         char *nDims = NULL,
                         int **dims = NULL,
                         void **ptr = NULL)
    {
        *clazz = this->clazz;
        *dtype = this->dtype;
        if(length) *length = this->length;
        if(nDims) *nDims = 0;
        if(dims) *dims = 0;
        if(ptr) *ptr = this->ptr;
    }

}; //ScalarData


////////////////////////////////////////////////////////////////////////////////
// INT8 SCALAR /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int8 class object description of DTYPE_B
///
class  EXPORT Int8 : public Scalar
{
public:
    Int8(char val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_B;
        length = 1;
        ptr = new char[1];
        ptr[0] = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(ptr[0]);}
    short getShort() {return numeric_cast<short>(ptr[0]);}
    int getInt() {return numeric_cast<int>(ptr[0]);}
    int64_t getLong() {return numeric_cast<int64_t>(ptr[0]);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(ptr[0]);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(ptr[0]);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(ptr[0]);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(ptr[0]);}
    float getFloat() {return numeric_cast<float>(ptr[0]);}
    double getDouble() {return numeric_cast<double>(ptr[0]);}

    ///@}
};

///
/// \brief The Uint8 class object description of DTYPE_BU
///
class  EXPORT Uint8 : public Scalar
{
public:
    Uint8(unsigned char val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_BU;
        length = 1;
        ptr = new char[1];
        ptr[0] = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(unsigned char *)ptr);}
    short getShort() {return numeric_cast<short>(*(unsigned char *)ptr);}
    int getInt() {return numeric_cast<int>(*(unsigned char *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(unsigned char *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(unsigned char *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(unsigned char *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(unsigned char *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(unsigned char *)ptr);}
    float getFloat() {return numeric_cast<float>(*(unsigned char *)ptr);}
    double getDouble() {return numeric_cast<double>(*(unsigned char *)ptr);}

    ///@}
};


////////////////////////////////////////////////////////////////////////////////
// INT16 SCALAR ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int16 class object description of DTYPE_W
///
class  EXPORT Int16 : public Scalar
{
public:
    Int16(short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_W;
        length = 2;
        ptr = new char[2];
        *(short *)ptr = val;
        setAccessory(units, error, help, validation);
    }


    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(short *)ptr);}
    short getShort() {return numeric_cast<short>(*(short *)ptr);}
    int getInt() {return numeric_cast<int>(*(short *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(short *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(short *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(short *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(short *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(short *)ptr);}
    float getFloat() {return numeric_cast<float>(*(short *)ptr);}
    double getDouble() {return numeric_cast<double>(*(short *)ptr);}

    ///@}
};

///
/// \brief The Uint16 class object description of DTYPE_WU
///
class  EXPORT Uint16 : public Scalar
{
public:
    Uint16(unsigned short val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_WU;
        length = 2;
        ptr = new char[2];
        *(unsigned short *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(unsigned short *)ptr);}
    short getShort() {return numeric_cast<short>(*(unsigned short *)ptr);}
    int getInt() {return numeric_cast<int>(*(unsigned short *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(unsigned short *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(unsigned short *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(unsigned short *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(unsigned short *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(unsigned short *)ptr);}
    float getFloat() {return numeric_cast<float>(*(unsigned short *)ptr);}
    double getDouble() {return numeric_cast<double>(*(unsigned short *)ptr);}

    ///@}
};




////////////////////////////////////////////////////////////////////////////////
// INT32 SCALAR ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int32 class object description of DTYPE_L
///
class  EXPORT Int32 : public Scalar
{
public:
    Int32(int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_L;
        length = sizeof(int);
        ptr = new char[sizeof(int)];
        *(int *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(int *)ptr);}
    short getShort() {return numeric_cast<short>(*(int *)ptr);}
    int getInt() {return numeric_cast<int>(*(int *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(int *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(int *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(int *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(int *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(int *)ptr);}
    float getFloat() {return numeric_cast<float>(*(int *)ptr);}
    double getDouble() {return numeric_cast<double>(*(int *)ptr);}

    ///@}
};

///
/// \brief The Uint32 class object description of DTYPE_LU
///
class  EXPORT Uint32 : public Scalar
{
public:
    Uint32(unsigned int val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_LU;
        length = sizeof(int);
        ptr = new char[sizeof(int)];
        *(unsigned int *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(unsigned int *)ptr);}
    short getShort() {return numeric_cast<short>(*(unsigned int *)ptr);}
    int getInt() {return numeric_cast<int>(*(unsigned int *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(unsigned int *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(unsigned int *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(unsigned int *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(unsigned int *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(unsigned int *)ptr);}
    float getFloat() {return numeric_cast<float>(*(unsigned int *)ptr);}
    double getDouble() {return numeric_cast<double>(*(unsigned int *)ptr);}

    ///@}
};



////////////////////////////////////////////////////////////////////////////////
// INT64 SCALAR ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int64 class object description of DTYPE_Q
///
class EXPORT Int64 : public Scalar
{
public:
    Int64(int64_t val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_Q;
        length = sizeof(int64_t);
        ptr = new char[sizeof(int64_t)];
        *(int64_t *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(int64_t *)ptr);}
    short getShort() {return numeric_cast<short>(*(int64_t *)ptr);}
    int getInt() {return numeric_cast<int>(*(int64_t *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(int64_t *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(int64_t *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(int64_t *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(int64_t *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(int64_t *)ptr);}
    float getFloat() {return numeric_cast<float>(*(int64_t *)ptr);}
    double getDouble() {return numeric_cast<double>(*(int64_t *)ptr);}

    ///@}
};

///
/// \brief The Uint64 class object description of DTYPE_QU
///
class EXPORT Uint64 : public Scalar
{
public:
    Uint64(uint64_t val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_QU;
        length = sizeof(int64_t);
        ptr = new char[sizeof(int64_t)];
        *(uint64_t *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(uint64_t *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(uint64_t *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(uint64_t *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(uint64_t *)ptr);}
    char getByte() {return numeric_cast<char>(*(uint64_t *)ptr);}
    short getShort() {return numeric_cast<short>(*(uint64_t *)ptr);}
    int getInt() {return numeric_cast<int>(*(uint64_t *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(uint64_t *)ptr);}
    float getFloat() {return numeric_cast<float>(*(uint64_t *)ptr);}
    double getDouble() {return numeric_cast<double>(*(uint64_t *)ptr);}
    char *getDate();

    ///@}
};




////////////////////////////////////////////////////////////////////////////////
// FLOAT32 SCALAR //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Float32 class object description of DTYPE_FLOAT
///
class  EXPORT Float32 : public Scalar
{
public:
    Float32(float val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_FLOAT;
        length = sizeof(float);
        ptr = new char[sizeof(float)];
        *(float *)ptr = val;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(float *)ptr);}
    short getShort() {return numeric_cast<short>(*(float *)ptr);}
    int getInt() {return numeric_cast<int>(*(float *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(float *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(float *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(float *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(float *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(float *)ptr);}
    float getFloat() {return numeric_cast<float>(*(float *)ptr);}
    double getDouble() {return numeric_cast<double>(*(float *)ptr);}

    ///@}
};



////////////////////////////////////////////////////////////////////////////////
// FLOAT64 SCALAR //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


///
/// \brief The Float64 class object description of DTYPE_DOUBLE
///
class  EXPORT Float64 : public Scalar
{
public:
    Float64(double val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_DOUBLE;
        length = sizeof(double);
        ptr = new char[sizeof(double)];
        *(double *)ptr = val;
        setAccessory(units, error, help, validation);
    }


    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    char getByte() {return numeric_cast<char>(*(double *)ptr);}
    short getShort() {return numeric_cast<short>(*(double *)ptr);}
    int getInt() {return numeric_cast<int>(*(double *)ptr);}
    int64_t getLong() {return numeric_cast<int64_t>(*(double *)ptr);}
    unsigned char getByteUnsigned() {return numeric_cast<unsigned char>(*(double *)ptr);}
    unsigned short getShortUnsigned() {return numeric_cast<unsigned short>(*(double *)ptr);}
    unsigned int getIntUnsigned() {return numeric_cast<unsigned int>(*(double *)ptr);}
    uint64_t getLongUnsigned() {return numeric_cast<uint64_t>(*(double *)ptr);}
    float getFloat() {return numeric_cast<float>(*(double *)ptr);}
    double getDouble() {return numeric_cast<double>(*(double *)ptr);}

    ///@}
};



////////////////////////////////////////////////////////////////////////////////
// COMPLEX32 SCALAR ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Complex32 class object description of DTYPE_FSC
///
class  EXPORT Complex32 : public Scalar
{
public:
    Complex32(float re, float im, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_FSC;
        length = 2 * sizeof(float);
        ptr = new char[2*sizeof(float)];
        ((float *)ptr)[0] = re;
        ((float *)ptr)[1] = im;
        setAccessory(units, error, help, validation);
    }


    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    float getReal() {return ((float *)ptr)[0];}
    float getImaginary() {return ((float *)ptr)[1];}
    std::complex<double> getComplex() {return std::complex<double>(((float *)ptr)[0], ((float *)ptr)[1]);}

    ///@}
};


////////////////////////////////////////////////////////////////////////////////
// COMPLEX64 SCALAR ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Complex64 class object description of DTYPE_FTC
///
class  EXPORT Complex64 : public Scalar
{
public:
    Complex64(double re, double im, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_FTC;
        length = 2 * sizeof(double);
        ptr = new char[2*sizeof(double)];
        ((double *)ptr)[0] = re;
        ((double *)ptr)[1] = im;
        setAccessory(units, error, help, validation);
    }

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    double getReal() {return ((double *)ptr)[0];}
    double getImaginary() {return ((double *)ptr)[1];}
    std::complex<double> getComplex() {return std::complex<double>(((double *)ptr)[0], ((double *)ptr)[1]);}

    ///@}
};




////////////////////////////////////////////////////////////////////////////////
// STRING SCALAR ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The String class  object description of DTYPE_T
///
class  EXPORT String : public Scalar
{
public:
    String(const char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        init(val, std::string(val).size(), units, error, help, validation);
    }

    String(unsigned char *uval, int len UNUSED_ARGUMENT, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        // FIXME: Hack to handle broken LabView types that use unsigned char (as uint8) instead of char
        // FIXME: Warning: Do not use this constructor in user code
        char * val = reinterpret_cast<char *>(uval);
        init(val, std::string(val).size(), units, error, help, validation);
    }

    //GAB  definition in order to avoid breaking LabVIEW
    String(unsigned char *uval, int numDims UNUSED_ARGUMENT, int *dims UNUSED_ARGUMENT, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        char * val = reinterpret_cast<char *>(uval);
        init(val, std::string(val).size(), units, error, help, validation);
    }

    String(const char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        init(val, len, units, error, help, validation);
    }

    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member
    char *getString()
    {
        char *res = new char[length + 1];
        std::copy(&ptr[0], &ptr[length + 1], res);
        res[length] = 0;
        return res;
    }

    /// get a string array of a single element
    char **getStringArray(int *numElements)
    {
        char **res = new char*[1];
	res[0] = new char[length + 1];
        std::copy(&ptr[0], &ptr[length + 1], res[0]);
        res[0][length] = 0;
	*numElements = 1;
        return res;
    }

    /// check if two strings have the same content.
    bool equals(Data *data);

private:
    virtual void init(char const * val, int len, Data * units, Data * error, Data * help, Data * validation) {
        dtype = DTYPE_T;
        length = len;
        ptr = new char[length+1];
        std::copy(&val[0], &val[length], ptr);
        ptr[length] = 0;
        setAccessory(units, error, help, validation);
    }
};

////////////////////////////////////////////////////////////////////////////////
// IDENT SCALAR ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Ident class is the object description of DTYPE_IDENT and is used to describe TDI variables
///
class  EXPORT Ident : public String
{
public:
    Ident(const char *val, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):String(val,units,error,help, validation) {
	dtype = DTYPE_IDENT;
    }

    Ident(unsigned char *uval, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):String(uval,len, units,error,help, validation) {
	dtype = DTYPE_IDENT;
    }

    //GAB  definition in order to avoid breaking LabVIEW
    Ident(unsigned char *uval, int numDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):String(uval, numDims, dims,units,error,help, validation)
    {
	dtype = DTYPE_IDENT;
    }

    Ident(const char *val, int len, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):String(val,len, units,error,help, validation) {
	dtype = DTYPE_IDENT;
    }
};





////////////////////////////////////////////////////////////////////////////////
// ARRAY DATA //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Array class object description of DTYPE_A
///
/// Common base class for arrays (CLASS_A). It contains dimension informationThe
/// contained data is assumed to be row-first ordered.
/// Array - derived classes will hold actual data in language-specific structures.
/// This allows for a direct implementation of operations such as getElementAt()
/// and setElementAt() which would be difficult or impossible to implement via TDI.
///
/// For the remaining Data methods, the generic mechanism of the base class'
/// implementation (conversion to MDSplus descriptor, TDI operation conversion back
/// to class instances) is retained.

class EXPORT Array: public Data
{
protected:
    int length;          ///< individual size of stored data element
    int arsize;          ///< total array data storage size
    int nDims;           ///< number of dimensions of the array access tuple
    int dims[MAX_DIMS];  ///< store each dimension size
    char *ptr;

    void setSpecific(void const * data, int length, int dtype, int nData) {
        setSpecific(data, length, dtype, 1, &nData);
    }

    void setSpecific(void const * data, int length, int dtype, int nDims, int *dims)
    {
        this->dtype = dtype;
        this->nDims = nDims;
        this->length = length;
        arsize = length;
        for(int i = 0; i < nDims; i++)
        {
            arsize *= dims[i];
            this->dims[i] = dims[i];
        }

        char const * dataBytes = reinterpret_cast<char const *>(data);
        ptr = new char[arsize];
        std::copy(&dataBytes[0], &dataBytes[arsize], ptr);
    }

public:
    Array(): length(0), arsize(0), nDims(0), ptr(0) {
        clazz = CLASS_A;
    }

    ~Array() {
        //			deleteNativeArray(ptr);
        delete[] ptr;
    }

    /// returns total array storage size as product of dimensions
    virtual int getSize()
    {
        int retSize = 1;
        for(int i = 0; i < nDims; i++)
            retSize *= dims[i];
        return retSize;
    }

    virtual void getInfo(char *clazz, char *dtype,
                         short *length = NULL,
                         char *nDims = NULL,
                         int **dims = NULL,
                         void **ptr = NULL)
    {
        *clazz = this->clazz;
        *dtype = this->dtype;
        if(length) *length = this->length;
        if(nDims) *nDims = this->nDims;
        if(dims)
        {
            *dims = new int[this->nDims];
            for(int i = 0; i < this->nDims; i++)
                (*dims)[i] = this->dims[i];
        }
        if(ptr) *ptr = this->ptr;
    }

    /// Provides a method to get the array shape, i.e. the array of dimension
    /// sizes and the total number of dimensions.
    virtual int *getShape(int *numDims);


    /// Retrieve a single element from the array matrix at the getDims position,
    /// if the passed number of dimension is equals to the dimension of dims
    /// field. Otherwise it returns the reshaped array corresponding to the
    /// passed dimension array. For example, if the array is bi-dimensional, a
    /// single dimension i will specify the i-th row.
    ///
    /// If the position exceeds the array dimension boundaries an exception is
    /// thrown
    ///
    /// \param getDims array of integers identifying the position tuple of the array
    /// \param getNumDims the size of the position tuple
    Data *getElementAt(int *getDims, int getNumDims);

    // TODO: [andrea] finire ...
    ///
    /// If the position exceeds the array dimension boundaries an exception is
    /// thrown
    ///
    /// \param getDims array of integers identifying the position tuple of the array
    /// \param getNumDims the size of the position tuple
    /// \param data the data object to set
    void setElementAt(int *getDims, int getNumDims, Data *data);

    /// Retrieve an element considering the array as one dimension shaped.
    Data *getElementAt(int dim) {
        return getElementAt(&dim, 1);
    }

    /// Set an element considering the array as one dimension shaped.
    void setElementAt(int dim, Data *data) {
        setElementAt(&dim, 1, data);
    }

    Array *getSubArray(int startDim, int nSamples);

    /// Convert data to array descriptor
    void * convertToDsc();

    ///@{
    /// inline overloaded conversion access returning result of cast operator of
    /// the Scalar object data member

    void *getArray() {return ptr;}
    char *getByteArray(int *numElements);
    short *getShortArray(int *numElements);
    int *getIntArray(int *numElements);
    int64_t *getLongArray(int *numElements);
    unsigned char *getByteUnsignedArray(int *numElements);
    unsigned short *getShortUnsignedArray(int *numElements);
    unsigned int *getIntUnsignedArray(int *numElements);
    uint64_t *getLongUnsignedArray(int *numElements);
    float *getFloatArray(int *numElements);
    double *getDoubleArray(int *numElements);
    std::complex<double> *getComplexArray(int *numElements);
    virtual char **getStringArray(int *numElements);

    ///@}
};


////////////////////////////////////////////////////////////////////////////////
// INT8 ARRAY DATA /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int8Array class, array of objects of descriptor type DTYPE_B
///
class Int8Array: public Array
{
public:
    Int8Array(char const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 1, DTYPE_B, nData);
        setAccessory(units, error, help, validation);
    }
    Int8Array(char const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 1, DTYPE_B, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Uint8Array class, array of objects of descriptor type DTYPE_BU
///
class EXPORT Uint8Array: public Array
{
    friend Data *deserialize(Data *serializedData);
public:
    Uint8Array(unsigned char const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 1, DTYPE_BU, nData);
        setAccessory(units, error, help, validation);
    }
    Uint8Array(unsigned char const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 1, DTYPE_BU, nDims, dims);
        setAccessory(units, error, help, validation);
    }
    Data* deserialize();
};



////////////////////////////////////////////////////////////////////////////////
// INT16 ARRAY DATA ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int16Array class, array of objects of descriptor type DTYPE_W
///
class Int16Array: public Array
{
public:
    Int16Array(short const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 2, DTYPE_W, nData);
        setAccessory(units, error, help, validation);
    }
    Int16Array(short const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 2, DTYPE_W, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Uint16Array class, array of objects of descriptor type DTYPE_WU
///
class Uint16Array: public Array
{
public:
    Uint16Array(unsigned short const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 2, DTYPE_WU, nData);
        setAccessory(units, error, help, validation);
    }
    Uint16Array(unsigned short const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 2, DTYPE_WU, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};



////////////////////////////////////////////////////////////////////////////////
// INT32 ARRAY DATA ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int32Array class, array of objects of descriptor type DTYPE_L
///
class Int32Array: public Array
{
public:
    Int32Array(int const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(int), DTYPE_L, nData);
        setAccessory(units, error, help, validation);
    }
    Int32Array(int const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(int), DTYPE_L, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Uint32Array, array of objects of descriptor type DTYPE_LU
///
class Uint32Array: public Array
{
public:
    Uint32Array(unsigned int const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(int), DTYPE_LU, nData);
        setAccessory(units, error, help, validation);
    }
    Uint32Array(unsigned int const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(int), DTYPE_LU, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
// INT64 ARRAY DATA ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Int64Array class, array of objects of descriptor type DTYPE_Q
///
class Int64Array: public Array
{
public:
    Int64Array(int64_t const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 8, DTYPE_Q, nData);
        setAccessory(units, error, help, validation);
    }
    Int64Array(int64_t const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 8, DTYPE_Q, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Uint64Array class object, array of objects of descriptor type DTYPE_QU
///
class Uint64Array: public Array
{
public:
    Uint64Array(uint64_t const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 8, DTYPE_QU, nData);
        setAccessory(units, error, help, validation);
    }
    Uint64Array(uint64_t const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, 8, DTYPE_QU, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
// FLOAT32 ARRAY DATA //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Float32Array class object, array of objects of descriptor type DTYPE_FLOAT
///
class Float32Array: public Array
{
public:
    Float32Array(float const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(float), DTYPE_FLOAT, nData);
        setAccessory(units, error, help, validation);
    }
    Float32Array(float const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(float), DTYPE_FLOAT, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Float64Array class object, array of objects of descriptor type DTYPE_DOUBLE
///
class Float64Array: public Array
{
public:
    Float64Array(double const * data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(double), DTYPE_DOUBLE, nData);
        setAccessory(units, error, help, validation);
    }
    Float64Array(double const * data, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(data, sizeof(double), DTYPE_DOUBLE, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
// COMPLEX32 ARRAY DATA ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Complex32Array class, array of objects of descriptor type DTYPE_FSC
///
class Complex32Array: public Array
{
public:
    Complex32Array(float const * reIm, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(reIm, 2*sizeof(float), DTYPE_FSC, nData);
        setAccessory(units, error, help, validation);
    }
    Complex32Array(float const * reIm, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(reIm, 2*sizeof(float), DTYPE_FSC, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};

///
/// \brief The Complex64Array class, array of objects of descriptor type DTYPE_FTC
///
class Complex64Array: public Array
{
public:
    Complex64Array(double const * reIm, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(reIm, 2*sizeof(double), DTYPE_FTC, nData);
        setAccessory(units, error, help, validation);
    }
    Complex64Array(double const * reIm, int nDims, int *dims, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        setSpecific(reIm, 2*sizeof(double), DTYPE_FTC, nDims, dims);
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
// STRING ARRAY DATA ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The StringArray class, array of objects of descriptor type DTYPE_T
///
class StringArray: public Array
{
public:
    /// StringArray constructor from array of c strings
    StringArray(char **data, int nData, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        //Pad all string to longest
        int maxLen = 0;
        for(int i = 0; i < nData; i++)
            maxLen = std::max(std::string(data[i]).size(), (std::size_t)maxLen);

        char *padData = new char[nData * maxLen];
        for(int i = 0; i < nData; i++) {
            std::size_t currLen = std::string(data[i]).size();
            std::copy(&data[i][0], &data[i][currLen], &padData[i * maxLen]);
            std::fill(&padData[i*maxLen + currLen], &padData[i*maxLen + maxLen], ' ');
        }
        setSpecific(padData, maxLen, DTYPE_T, nData);
        setAccessory(units, error, help, validation);
        delete[] padData;
    }

    /// StringArray constructor from single c string
    StringArray(char const * data, int nStrings, int stringLen, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) //For contiuguous
    {
        setSpecific(data, stringLen, DTYPE_T, nStrings);
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
//  COMPOUND DATA   ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Compound class object description of DTYPE_R
///
/// Compound is the common base class for all CLASS_R types. Its fields contain
/// all the required information (Descriptor array, keyword). Its getter/setter
/// methods allow to read/replace descriptors, based on their index. Derived
/// classes will only define methods with appropriate names for reading/writing
/// descriptors (i.e. Data objects).

class EXPORT Compound: public Data {
public:
	Compound():
		opcode(0)
	{
		setClass();
	}

	Compound(int dtype, int length, void * ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
		setClass();
		setAccessory(units, error, help, validation);
		this->dtype = dtype;

		switch(length) {
			case 1:  opcode = *(static_cast<char *>(ptr)); break;
			case 2:  opcode = *(static_cast<short *>(ptr)); break;
			case 4:  opcode = *(static_cast<int *>(ptr)); break;
			default: opcode = 0;
		}

		for(int i = 0; i < nDescs; ++i) {
			this->descs.push_back((Data *)descs[i]);
			if (this->descs[i])
				this->descs[i]->incRefCount();
		}
	}

    virtual void propagateDeletion() {
      for(std::size_t i = 0; i < descs.size(); ++i)
            if (descs[i])
            {
                descs[i]->decRefCount();
                deleteData(descs[i]);
            }
    }

    void * convertToDsc();

    virtual ~Compound()
    {
        propagateDeletion();
    }

protected:
    short opcode;                ///< used only by some derived classes.
    std::vector<Data *> descs;   ///< descriptors storage member

    /// increment reference of each stored data
    void incrementRefCounts() {
        for(std::size_t i = 0; i < descs.size(); ++i)
            if(descs[i])
                descs[i]->refCount++;
    }

public:
    /// set Data at idx element of contained descriptors
    void assignDescAt(Data *data, int idx) {
      if ( idx < 0 || idx >= (int)descs.size() ) {
            if(data) deleteData(data);
            throw (MdsException("Index out of bounds accessing arguments"));
        }

        if (descs.at(idx)) {
            descs.at(idx)->decRefCount();
            deleteData(descs[idx]);
        }

        descs.at(idx) = data;
        if (data) data->refCount++;
    }

    /// retrieve Data at the idx position of contained descriptors
    Data * getDescAt(int idx) {
      if ( idx < 0 || idx >= (int)descs.size() ) {
            throw (MdsException("Index out of bounds accessing arguments"));
        }

        if (descs.at(idx))
            descs[idx]->incRefCount();
        return descs[idx];
    }
    int getNumDescs()
    {
	return descs.size();
    }

private:
	void setClass() {
		clazz = CLASS_R;
	}
};



////////////////////////////////////////////////////////////////////////////////
//  SIGNAL COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Signal class object description of DTYPE_SIGNAL
///
/// MDSplus provides a signal data type which combines dimension descriptions
/// with the data. While it was initially designed to be used for efficient
/// storage of data acquired from measurement devices attached to an experiment
/// it has been found to be a very useful way of storing additional information
/// such as results from data analysis or modeling data. A signal is a
/// structure of three or more fields. The first field is the "value" field of
/// the signal. This is followed by an optional raw data field (explained
/// later). These two fields can be followed by one or more dimension
/// descriptions. The number of dimension descriptions in general should match
/// the dimensionality of the value. MDSplus stores data from a transient
/// recorder which is a device which measures voltage as a function of time.
/// Typically a transient recorder uses a analog to digital converter and
/// records the data as a series of integer values. A linear conversion can be
/// done to convert these integer values into input voltages being measured.
/// When MDSplus stores the data for such a device it uses a signal datatype to
/// record the data. The signal structure is constructed by putting an
/// expression for converting the integer data to volts in the "value" portion,
/// the integer data in the raw data portion, and a representation of the
/// timebase in the single dimension portion.
///
/// \note This class represents a data descriptor of a signal intended as an
/// ordered sequence of data. Nothing in common with software objects
/// signalling systems.

class Signal: public Compound {
public:
#ifndef DOXYGEN // hide this part from documentation
    Signal(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
        Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Signal(Data *data, Data *raw, Data *dimension, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        descs.push_back(data);
        descs.push_back(raw);
        descs.push_back(dimension);
        init(units, error, help, validation);
    }

    Signal(Data *data, Data *raw, Data *dimension1, Data *dimension2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        descs.push_back(data);
        descs.push_back(raw);
        descs.push_back(dimension1);
        descs.push_back(dimension2);
        init(units, error, help, validation);
    }

    Signal(Data *data, Data *raw, int nDims, Data **dimensions, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        descs.push_back(data);
        descs.push_back(raw);
        for(int i = 0; i < nDims; i++)
            descs.push_back(dimensions[i]);
        init(units, error, help, validation);
    }

    Data *getData() {
        return getDescAt(0);
    }

    Data *getRaw() {
        return getDescAt(1);
    }

    Data *getDimension() {
        return getDescAt(2);
    }

    Data *getDimensionAt(int idx) {
        return getDescAt(idx + 2);
    }

    int getNumDimensions() {
        return descs.size() - 2;
    }

    void setData(Data *data) {assignDescAt(data, 0);}
    void setRaw(Data *raw){assignDescAt(raw, 1);}
    void setDimension(Data *dimension) {assignDescAt(dimension, 2);}
    void setDimensionAt(Data *dimension, int idx) {assignDescAt(dimension, 2 + idx);}

private:
    void init(Data * units, Data * error, Data * help, Data * validation) {
        dtype = DTYPE_SIGNAL;
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }
};


////////////////////////////////////////////////////////////////////////////////
//  DIMENSION COMPOUND  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
/// \brief The Dimension class object description of DTYPE_DIMENSION
///
/// MDSplus provides a dimension data type which provides a compact mechanism
/// for expressing signal dimensions (See: DTYPE_SIGNAL). The dimension data
/// type was implemented as a way to represent data such as the timebase for
/// signals recorded by data acquisition equipment such as transient recorders.
///
/// A dimension data type is a structure which has two parts, a window and an
/// axis. The axis part is a representation of a series of values (i.e. time
/// stamps) and is generally represented by a DTYPE_RANGE data item. If this
/// was a single speed clock, for example, the axis would be represented by a
/// range consisting or an optional start time, an optional end time and a
/// single delta time value. This range could represent a series of clock
/// pulses which began some time infinitely in the past and continuing to some
/// time infinitely in the future. The window portion of the dimention is used
/// to select a set of these infinite stream of clock pulses that represent
/// those clock pulses which match the samples that were recorded in the
/// digitizers internal memory. The window portion is usually represented by a
/// DTYPE_WINDOW data item. A window consists of a start index, and end index
/// and a value at index 0. For a transient digitizer, the samples in the
/// memory were recorded at a known number of clock pulses before the trigger
/// (the start index) and continuing a known number of clock pulses after the
/// trigger. The time the module was triggered is the value at index 0 part of
/// the window.
///
/// >
/// > See \ref dt_dimension for further details.
/// >
///

class Dimension: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Dimension(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
        Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Dimension(Data *window, Data *axis, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_DIMENSION;
        descs.push_back(window);
        descs.push_back(axis);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    /// Get the dimension window
    Data *getWindow(){
        return getDescAt(0);
    }

    /// Get the dimension axis
    Data *getAxis(){
        return getDescAt(1);
    }

    /// Set the dimension window
    void setWindow(Data *window) { assignDescAt(window, 0); }

    /// Set the dimension axis
    void setAxis(Data *axis){ assignDescAt(axis, 1); }
};


////////////////////////////////////////////////////////////////////////////////
//  WINDOW COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Window class object description of DTYPE_WINDOW
///
/// MDSplus provides a window data type which is used exclusively in
/// conjunction with a dimension data item (\ref Dimension). The window
/// provides a means for bracketing a potentially infinite vector of values
/// generated by a range data item (\ref Range). A window data type is a
/// structure containing three fields: the start index, the end index and the
/// value at index 0. The window brackets the axis portion of a dimension item
/// by finding the nearest element in the axis to the "value at index 0" value.
/// The subset of the axis elements are selected using the start index and end
/// index using this starting element as the index 0. For example if the window
/// was BUILD_WINDOW(-5,10,42.), MDSplus would find the element in the axis
/// portion of the dimension closest to a value of 42 (assuming the axis is
/// always increasing) and call that element, element number 0. The subset
/// would be this element and the 5 elements before it and the 10 elements
/// after it. For a more detailed explanation see the description of the
/// dimension data type.
///
/// >
/// > See \ref dt_window for further details.
/// >
///

class Window: public Compound
{
public:

#   ifndef DOXYGEN // hide this part from documentation
    Window(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
        Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#   endif // DOXYGEN end of hidden code

    Window(Data *startidx, Data *endidx, Data *value_at_idx0, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_WINDOW;
        descs.push_back(startidx);
        descs.push_back(endidx);
        descs.push_back(value_at_idx0);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getStartIdx() {
        return getDescAt(0);
    }

    Data *getEndIdx() {
        return getDescAt(1);
    }

    Data *getValueAt0() {
        return getDescAt(2);
    }

    void setStartIdx(Data *startidx) {assignDescAt(startidx, 0);}
    void setEndIdx(Data *endidx){assignDescAt(endidx, 1);}
    void setValueAt0(Data *value_at_idx0) {assignDescAt(value_at_idx0, 2);}
};


////////////////////////////////////////////////////////////////////////////////
//  FUNCTION COMPOUND  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Function class object description of DTYPE_FUNCTION
///
/// MDSplus provides a function data type used for describing references to
/// built in native TDI functions. When you specify an expression such as "a +
/// b", MDSplus will compile this into a DTYPE_FUNCTION data item. The function
/// data type consists of a opcode and a list of operands. The opcode is stored
/// as a 16 bit code and the operands can be any MDSplus data type.
///
/// >
/// > See \ref dt_function for further details.
/// >
///

class Function: public Compound
{
public:

#   ifndef DOXYGEN // hide this part from documentation
    Function(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
        Compound(dtype, length, ptr, nDescs, descs)
    { setAccessory(units, error, help, validation); }
#   endif // DOXYGEN end of hidden code

    Function(short opcode, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_FUNCTION;
        this->opcode = opcode;
        for(int i = 0; i < nargs; i++)
            descs.push_back(args[i]);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    /// Get operation code from compiled TDI expression
    short getOpcode() {
        return opcode;
    }

    /// Get number of arguments for this expression.
    int getNumArguments() { return descs.size(); }

    /// Get data argument at position idx
    Data *getArgumentAt(int idx) {
        return getDescAt(idx);
    }

    /// Set idx data argument
    void setArgAt(Data *arg, int idx) {assignDescAt(arg, idx);}
};


////////////////////////////////////////////////////////////////////////////////
//  CONGLOM COMPOUND  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Conglom class object description of DTYPE_CONGLOM
///
/// MDSplus provides a conglomerate data type used for describing data
/// acquisition or analysis devices. A device in MDSplus is implemented as a
/// collection of adjacent tree nodes (or a conglomerate). The first node
/// (located at the top of the node structure of this collection) contains a
/// conglomerate data type. The conglomerate data type is a structure
/// containing an image part, a model part, a name part and a qualifiers part.
/// MDSplus implements device support by providing a mechanism for performing
/// "methods" on devices. When a device method is performed on a device in a
/// MDSplus tree either through an action or using the TCL command DO/METHOD,
/// MDSplus invokes a procedure determined by the information in the head node
/// of the device which contains a conglomerate data type. If there is an image
/// part of the conglomerate structure, MDSplus will attempt to call a routine
/// called "model-part"__"method" in the shared library specified by the image
/// part. If the routine can not be found, MDSplus will try to invoke the TDI
/// function called "model-part"__"method". Normally, the conglomerate data is
/// loaded into the head node of a device automatically when you add it to the
/// tree so you may not need to access this data type directly.
///
/// The name part of the device is often a reference to a sub node of the
/// device which contains the data acquisition module identification such as
/// the CAMAC module name. This portion of the device is not used during method
/// invokation and was simply added as a convenience in building tools for
/// gathering information about device definitions in a tree. The qualifiers
/// part of the structure could be used by device support for making variant
/// device implementations. This field along with the name field are generally
/// not used any longer.
///
/// >
/// > See \ref dt_conglom for further details.
/// >
///

class Conglom: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Conglom(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Conglom(Data *image, Data *model, Data *name, Data *qualifiers, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_CONGLOM;
        descs.push_back(image);
        descs.push_back(model);
        descs.push_back(name);
        descs.push_back(qualifiers);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }


    Data *getImage() {
        return getDescAt(0);
    }

    Data *getModel() {
        return getDescAt(1);
    }

    Data *getName() {
        return getDescAt(2);
    }

    Data *getQualifiers() {
        return getDescAt(3);
    }

    void setImage(Data *image) {assignDescAt(image, 0);}
    void setModel(Data *model){assignDescAt(model, 1);}
    void setName(Data *name) {assignDescAt(name, 2);}
    void setQualifiers(Data *qualifiers) {assignDescAt(qualifiers, 3);}
};


////////////////////////////////////////////////////////////////////////////////
//  RANGE COMPOUND  ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Range class, object description of DTYPE_RANGE
///
/// MDSplus provides a range data type which provides a compact mechanism for
/// expressing series of values with fixed intervals. The range data type is a
/// structure consisting of three parts; begin, end, and delta. This data type
/// is used often in MDSplus for representing data such as the output of clock
/// devices used in data acquisition. The following example shows three simple
/// instances of ranges:
///
/// \code{.cpp}
///     // _tdi_range1 = 1 : 10
///     Data *range1 = new Range(new Int32(1),new Int32(10),NULL);
///
///     // _tdi_range2 = 1 : 10 : .5
///     Data *range2 = new Range(new Float32(1),new Float32(10),new Float32(0.5));
///
///     // _range = * : * : 1E-6
///     Data *range3 = new Range(NULL,NULL,new Float32(1E-6));
/// \endcode
///
/// In the above examples, we show two formats for creating ranges. The MDSplus
/// TDI expression evaluator has a built in syntax for specifying ranges which
/// is one of the following:
///
/// \code{.cpp}
///     begin : end [: delta]
///     _range = build_range(begin,end,delta)
/// \endcode
///
/// >
/// > See \ref dt_range for further details.
/// >
///

class Range: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Range(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Range(Data *begin, Data *ending, Data *deltaval, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_RANGE;
        descs.push_back(begin);
        descs.push_back(ending);
        descs.push_back(deltaval);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    /// Get range begin
    Data *getBegin() {
        return getDescAt(0);
    }

    /// Get range end
    Data *getEnding() {
        return getDescAt(1);
    }

    /// Get Range delta
    Data *getDeltaVal() {
        return getDescAt(2);
    }

    void setBegin(Data *begin) {assignDescAt(begin, 0);}
    void setEnding(Data *ending){assignDescAt(ending, 1);}
    void setDeltaVal(Data *deltaval) {assignDescAt(deltaval, 2);}
};


////////////////////////////////////////////////////////////////////////////////
//  ACTION COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Action class, object description of type DTYPE_ACTION
///

/// MDSplus provides an action data type used for describing actions to be
/// dispatched to action servers during a normal experiment cycle. An action
/// consists of 5 parts but the current implementations of action dispatching
/// only refernce the first three parts, a dispatch part, a task part and an
/// errorlogs part (OpenVMS only). The dispatch part (See: DTYPE_DISPATCH)
/// includes information such as which server should perform the task, when
/// during the course of the shot cycle it should be executed, what MDSplus
/// event should be issued when the task completes. The task part describes
/// what is to be done when the action is executed. There are a couple
/// different types of task types currently support (See: DTYPE_METHOD and
/// DTYPE_ROUTINE). The errorlogs part is implemented only on OpenVMS. If this
/// part contains a string consisting of a comma delimited list of user
/// accounts, each user on the list will receive a OpenVMS cluster-wide
/// broadcast message if this action fails during execution. The remaining
/// parts were originally defined to hold statistics from the execution of the
/// action but since this would require rewriting the action data (which is
/// usually considered setup information and not writable in the pulse files)
/// this feature was never used. This unused parts are called
/// completion_message and performance.
///
/// >
/// > See \ref dt_action for further details.
/// >
///

class Action: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Action(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Action(Data *dispatch, Data *task, Data *errorlogs, Data *completion_message, Data *performance, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_ACTION;
        descs.push_back(dispatch);
        descs.push_back(task);
        descs.push_back(errorlogs);
        descs.push_back(completion_message);
        descs.push_back(performance);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getDispatch() {
        return getDescAt(0);
    }

    Data *getTask() {
        return getDescAt(1);
    }

    Data *getErrorLogs() {
        return getDescAt(2);
    }

    Data *getCompletionMessage() {
        return getDescAt(3);
    }

    Data *getPerformance() {
        return getDescAt(4);
    }

    void setDispatch(Data *dispatch) {assignDescAt(dispatch, 0);}
    void setTask(Data *task){assignDescAt(task, 1);}
    void setErrorLogs(Data *errorlogs){assignDescAt(errorlogs, 2);}
    void setCompletionMessage(Data *completion_message) {assignDescAt(completion_message, 3);}
    void setPerformance(Data *performance){assignDescAt(performance, 4);}
};


////////////////////////////////////////////////////////////////////////////////
//  DISPATCH COMPOUND  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Dispatch class, object description of DTYPE_DISPATCH
///
/// MDSplus provides a dispatch data type used for specifying the dispatching
/// information of an action. This determines where and when an automatic
/// action should be executed during the course of a experiment cycle.
///
/// A dispatch item is a structure consisting of 5 parts; dispatch type,
/// server, phase, when, and completion. The dispatch type part specifies the
/// type of scheduling to be done. Currently only one type of dispatching is
/// supported in MDSplus and that is sequential scheduling (value=2). This
/// field should contain an integer value of 2.
///
/// Server
/// ------
/// The server part specifies the server that should execute the action. For
/// tcpip based action servers this field should contain a string such as
/// host:port where host is the tcpip hostname of the computer where the action
/// server is running and the port is the port number on which the action
/// server is listening for actions to perform. For DECNET based action servers
/// (OpenVMS only), this should be a string such as host::object where the host
/// is the DECNET node name and the object is the DECNET object name of the
/// action server.
///
/// Phase
/// -----
/// The phase part of a dispatch item is either the name or number
/// corresponding to the phase of the experiment cycle. These would normally be
/// phases such as "store", "init","analysis" etc. but the names and numbers of
/// the phases can be customized by the MDSplus system administrator by
/// modifying the TDI function phase_table().
///
/// When
/// ----
/// The when part normally contains either an integer value or an expression
/// which evaluates to an integer value representing a sequence number. When
/// the dispatcher (implemented by a set of mdstcl dispatch commands) builds a
/// dispatching table, it finds all the actions defined in a tree and then
/// sorts these actions first by phase and then by sequence number. Actions are
/// then dispatched to servers during a phase in order of their sequence
/// numbers (except for actions with sequence numbers less than or equal to
/// zero which are not dispatched). There is a special case for the when part
/// which enables you to set up dependencies on other actions. If instead of
/// specifying a sequence number for the when part, you specify an expression
/// which references other action nodes in the tree, this action will not be
/// dispatched until all action nodes referenced in the expression have
/// completed. When all the actions referenced in the expression have
/// completed, the expression is then evaluated substituting the completion
/// status of the referenced actions instead of the action node itself. If the
/// result of the evaluation yields an odd number (low bit set) then this
/// action will be dispatched. If the result is an even value then this action
/// is not dispatched but instead assigned a failure completion status in case
/// other actions have when expressions refering to it. Using this mechanism
/// you can configure fairly complex conditional dispatching.
///
/// Completion
/// ----------
/// The completion part can hold a string defining the name of an MDSplus event
/// to be declared upon completion of this action. These events are often used
/// to trigger updates on visualization tools such as dwscope when this action
/// completes indicating availability of the data.
///
/// >
/// > See \ref dt_dispatch for further details.
/// >
///

class Dispatch: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Dispatch(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Dispatch(Data *ident, Data *phase, Data *when, Data *completion, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_DISPATCH;
        descs.push_back(ident);
        descs.push_back(phase);
        descs.push_back(when);
        descs.push_back(completion);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getIdent(){
        return getDescAt(0);
    }

    Data *getPhase(){
        return getDescAt(1);
    }

    Data *getWhen() {
        return getDescAt(2);
    }

    Data *getCompletion() {
        return getDescAt(3);
    }

    void setIdent(Data *ident) {assignDescAt(ident, 0);}
    void setPhase(Data *phase){assignDescAt(phase, 1);}
    void setWhen(Data *when) {assignDescAt(when, 2);}
    void setCompletion(Data *completion) {assignDescAt(completion, 3);}
};


////////////////////////////////////////////////////////////////////////////////
//  PROGRAM COMPOUND  /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Program class object description of DTYPE_PROGRAM
///

class Program: public Compound
{
protected:
    bool isImmutable() { return false;}
public:
    Program(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
    Program(Data *timeout, Data *program, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_PROGRAM;
        descs.push_back(timeout);
        descs.push_back(program);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getTimeout() {
        return getDescAt(0);
    }

    Data *getProgram() {
        return getDescAt(1);
    }

    void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
    void setProgram(Data *program){assignDescAt(program, 1);}
};

////////////////////////////////////////////////////////////////////////////////
//  ROUTINE COMPOUND  //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Routine class object description of DTYPE_ROUTINE
///

class Routine: public Compound
{
protected:
    bool isImmutable() { return false;}
public:
    Routine(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
    Routine(Data *timeout, Data *image, Data *routine, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_ROUTINE;
        descs.push_back(timeout);
        descs.push_back(image);
        descs.push_back(routine);
        for(int i = 0; i < nargs; i++)
            descs.push_back(args[i]);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getTimeout() {
        return getDescAt(0);
    }

    Data *getImage(){
        return getDescAt(1);
    }

    Data *getRoutine() {
        return getDescAt(2);
    }

    Data *getArgumentAt(int idx) {
        return getDescAt(idx + 3);
    }

    void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
    void setImage(Data *image){assignDescAt(image, 1);}
    void setRoutine(Data *routine) {assignDescAt(routine, 2);}
    void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
};




////////////////////////////////////////////////////////////////////////////////
//  PROCEDURE COMPOUND  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Procedure class object description of DTYPE_PROCEDURE
///

class Procedure: public Compound
{
protected:
    bool isImmutable() { return false;}
public:
    Procedure(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
    Procedure(Data *timeout, Data *language, Data *procedure, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_PROCEDURE;
        descs.push_back(timeout);
        descs.push_back(language);
        descs.push_back(procedure);
        for(int i = 0; i < nargs; i++)
            descs.push_back(args[i]);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getTimeout(){
        return getDescAt(0);
    }

    Data *getLanguage(){
        return getDescAt(1);
    }

    Data *getProcedure() {
        return getDescAt(2);
    }

    Data *getArgumentAt(int idx) {
        return getDescAt(idx + 3);
    }

    void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
    void setLanguage(Data *language){assignDescAt(language, 1);}
    void seProcedure(Data *procedure) {assignDescAt(procedure, 2);}
    void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
};


////////////////////////////////////////////////////////////////////////////////
//  METHOD COMPOUND  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Method class object description of DTYPE_METHOD
///
/// MDSplus provides a method data type for describing an operation to be
/// performed on an MDSplus device to be used as the task portion of an action
/// data item. MDSplus devices support various "methods" which can be performed
/// on the device. For example, transient digitizer device may support an
/// "INIT" method to prepare the device to acquire data and a "STORE" method
/// which would retrieve the data from the physical device and store it into
/// the device nodes in the MDSplus tree.
///
/// The method data type is a structure consisting of three fields followed by
/// optional arguments to the method being performed.
///
/// The first field is a timeout value specified in seconds. When the method is
/// being performed on the device and a timeout is specified, a timer is
/// started when the operation is started and if the operation is not completed
/// before the specified timeout, the operation will be aborted abruptly. This
/// field can either be omitted or should contain a value or expression which
/// when evaluated will result in a scalar numeric value.
///
/// The next field in the method data type is the name of the method to be
/// performed. This should contain a value or expression which when evaluated
/// will result in a scalar text value representing a supported method for the
/// device being operated on (i.e. "INIT", "STORE","EXECUTE"). The set of
/// available methods is device specific.
///
/// The next field is the object on which this operation is to be performed.
/// This should be a node reference to one of the nodes which make up the
/// device, usually the top node in the device node structure.
///
/// These first three fields can be followed by optional parameters which will
/// be interpretted by the implementation of the method. These fields are
/// specific to the implementation of the various methods.
///
/// >
/// > See \ref dt_method for further details.
/// >
///

class Method: public Compound
{
public:
#ifndef DOXYGEN // hide this part from documentation
    Method(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#endif // DOXYGEN end of hidden code

    Method(Data *timeout, Data *method, Data *object, int nargs, Data **args, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_METHOD;
        descs.push_back(timeout);
        descs.push_back(method);
        descs.push_back(object);
        for(int i = 0; i < nargs; i++)
            descs.push_back(args[i]);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    Data *getTimeout(){
        return getDescAt(0);
    }

    Data *getMethod() {
        return getDescAt(1);
    }

    Data *getObject() {
        return getDescAt(2);
    }

    Data *getArgumentAt(int idx) {
        return getDescAt(idx + 3);
    }

    void setTimeout(Data *timeout) {assignDescAt(timeout, 0);}
    void setMethod(Data *method){assignDescAt(method, 1);}
    void setObject(Data *object) {assignDescAt(object, 2);}
    void setArgumentAt(Data *argument, int idx) {assignDescAt(argument, 3 + idx);}
};


////////////////////////////////////////////////////////////////////////////////
//  DEPENDENCY COMPOUND  ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Dependency class object description of DTYPE_DEPENDENCY
///

class Dependency: public Compound
{
public:
    Dependency(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
    Dependency(short opcode, Data *arg1, Data *arg2, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_DEPENDENCY;
        this->opcode = opcode;
        descs.push_back(arg1);
        descs.push_back(arg2);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    short getOpcode() {
        return opcode;
    }

    Data *getArg1(){
        return getDescAt(0);
    }

    Data *getArg2(){
        return getDescAt(1);
    }

    void setOpcode(short opcode) {
        this->opcode = opcode;
    }

    void setArg1(Data *arg1) {assignDescAt(arg1, 0);}
    void setArg2(Data *arg2) {assignDescAt(arg2, 0);}
};



////////////////////////////////////////////////////////////////////////////////
//  CONDITION COMPOUND  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Condition class object description of DTYPE_CONDITION
///

class Condition: public Compound
{
public:
    Condition(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
    Condition(short opcode, Data *arg, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_CONDITION;
        this->opcode = opcode;
        descs.push_back(arg);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    short getOpcode() {
        return opcode;
    }

    Data *getArg(){
        return getDescAt(0);
    }

    void setOpcode(short opcode) {
        this->opcode = opcode;
    }

    void setArg(Data *arg) {assignDescAt(arg, 0);}
};


////////////////////////////////////////////////////////////////////////////////
//  CALL COMPOUND  /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Call class object description of DTYPE_CALL
///
/// MDSplus provides a call data type used for describing references to
/// external functions. The MDSplus expression evaluator has the ability to
/// call external functions in shared libararies or DLL's. The call datatype
/// describes the type of the return value of the function, the name of the
/// library or DLL where the function can be found, the name of the function
/// and the list of arguments to present to the function when calling it. The
/// expression evaluator in MDSplus has syntax for easily building instances of
/// this data type as will as builtin functions for constructing them. The call
/// to the external function occurs when the expression containing a call item
/// is evaluated. The following example shows an example of calling an external
/// function in TDI:
///
///     mysharedlib->myfunction(42)
///
/// The code above is the most common form of the function reference. When
/// evaluated, the function called "myfunction" found in the library
/// "mysharedlib" will be called with one argument, 42. The return value of the
/// function is assumed to be a 32-bit signed integer. The code following shows
/// how to instance a binary call, the actual function call is done when the
/// evaluation of the expression is triggered by the data() member.
///
/// \code{.cpp}
/// #include <mdsobjects.h>
/// using namespace MDSplus;
/// {
///     // test for binary function call //
///     Data *args[2] = { new Int32(5552367), new Int32(1) };
///     AutoData<Call> call2 = new Call(new String("./testutils/libMdsTestDummy.so"),
///                                     new String("test_addint"),
///                                     2, (Data**)args);
///     TEST1( AutoData<Data>(call2->data())->getInt() == 5552368 );
///     deleteData(args[0]);
///     deleteData(args[1]);
/// }
/// \endcode
///
/// \note MDSplus uses operating system specific rules to locate shared
/// libraries when the full file specification of the library is not given. On
/// OpenVMS, the system would first look for a logical name matching the name
/// of the library and if one is found it would activate the image defined by
/// the logical name. If their is no such logical name, the system would then
/// look in the directory search path defined by SYS$SHARE for the library. The
/// file type for shared libraries on OpenVMS is ".EXE". On Unix systems the
/// system looks for a library path environment variable such as
/// LD_LIBRARY_PATH or SHLIB_PATH and looks in these directories. If there is
/// no such environment variable, the system looks in system library
/// directories (usually /usr/lib or /usr/local/lib) for the shared library.
/// The file type for shared libraries on Unix is generally ".so",".sl" or
/// sometimes ".a". On Windows systems, the system looks for shared libraries
/// in the directory where the application was run or in the directory list
/// specified by the "PATH" environment variable. The file type of shared
/// libraries on Windows is ".dll".
///

class Call: public Compound {
public:

#   ifndef DOXYGEN // hide this part from documentation
    Call(int dtype, int length, char *ptr, int nDescs, char **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) :
        Compound(dtype, length, ptr, nDescs, descs)
    {
        setAccessory(units, error, help, validation);
    }
#   endif // DOXYGEN end of hidden code

    /// Build a call object secifying the image and routine entries, together
    /// with the routine data arguments and return type. If no return type is
    /// specified the DTYPE_L is assumed as default.
    ///
    /// \param image A new \ref String containing the routine to be called
    /// \param routine A new \ref String of the called image symbol routine.
    /// \param nargs The number of arguments that will be the routine input arguments.
    /// \param args The array of the data arguments of the routine.
    /// \param retType The data type (DTYPE_xxx) that the Call will return.
    ///
    Call(Data *image, Data *routine, int nargs, Data **args, char retType = DTYPE_L, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0)
    {
        dtype = DTYPE_CALL;
        opcode = retType;
        descs.push_back(image);
        descs.push_back(routine);
        for(int i = 0; i < nargs; i++)
            descs.push_back(args[i]);
        incrementRefCounts();
        setAccessory(units, error, help, validation);
    }

    /// Get the code associated to the Call return type. This is the data type
    /// that will be associated to the returned data descriptor. If no return
    /// type was specified in the instance construction the DTYPE_L is assumed
    /// as default.
    ///
    char getRetType() {
        return opcode;
    }

    /// Set the return type code as one of the DTYPE_xx codes defined. If no
    /// return type was specified in the instance construction the DTYPE_L is
    /// assumed as default.
    ///
    void setRetType(char retType) {
        opcode = retType;
    }

    /// Get the Call image as Data object. The image is usually a string that
    /// specifies the current path to find the library that contains the symbol
    /// to be called.
    ///
    Data *getImage() {
        return getDescAt(0);
    }

    /// Get the routine that the Call instance will look for in the image. This
    /// is usually a string containing the symbol within the image library.
    /// \note MDSplus does not manages the symbol mangling used by compilers to
    /// identify class member functions, so that only C compiled function will
    /// be reached by Call.
    ///
    Data *getRoutine(){
        return getDescAt(1);
    }

    /// Get the idx argument of the routine input argument list.
    ///
    Data *getArgumentAt(int idx) {
        return getDescAt(idx + 2);
    }

    /// Set the call image. The image is usually a string that specifies the
    /// current path to find the library that contains the symbol to be called.
    ///
    void setImage(Data *image) { assignDescAt(image, 0); }

    /// Set the routine that the Call instance will look for in the image. This
    /// is usually a string containing the symbol within the image library.
    /// \note MDSplus does not manages the symbol mangling used by compilers to
    /// identify class member functions, so that only C compiled function will
    /// be reached by Call.
    ///
    void setRoutine(Data *routine) { assignDescAt(routine, 1); }

    /// Set the idx argument of the routine input argument list.
    ///
    void setArgumentAt(Data *argument, int idx) { assignDescAt(argument, 2 + idx); }
};



////////////////////////////////////////////////////////////////////////////////
//  APD DATA  //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Apd class object description of DTYPE_DSC
///

class EXPORT Apd: public Data
{
public:
    Apd(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        clazz = CLASS_APD;
        dtype = DTYPE_DSC;
        setAccessory(units, error, help, validation);
    }

    Apd(std::size_t nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0) {
        clazz = CLASS_APD;
        dtype = DTYPE_DSC;
        for(std::size_t i = 0; i < nData; ++i) {
            if(descs[i]) descs[i]->incRefCount();
            this->descs.push_back(descs[i]);
        }
        setAccessory(units, error, help, validation);
    }


    virtual void propagateDeletion()
    {
        for(std::size_t i = 0; i < descs.size(); ++i)
        {
            if (descs[i])
            {
                descs[i]->decRefCount();
                deleteData(descs[i]);
            }
        }
    }

    virtual ~Apd() {
        propagateDeletion();
    }

    virtual std::size_t len() {
        return descs.size();
    }

    Data ** getDscs() {
        return &descs[0];
    }

    std::size_t getDimension() {
        return Apd::len();
    }

    Data ** getDscArray() {
        return getDscs();
    }

    Data * getDescAt(std::size_t i) {
        //descs.at(i)->incRefCount();
        return descs.at(i);
    }

    bool hasDescAt(std::size_t i) {
        return descs.size() > i && descs[i] != NULL;
    }
    void setDescAt(std::size_t i, Data * data) {
        if(descs.size() <= i)
        {
            for(size_t j = descs.size(); j < i; j++)
                descs.push_back(NULL);
            descs.push_back(data);
        }
        else
            //INSERT AND BEGIN() WORK ONLY IF VECTOR NON EMPTY!!
            descs.insert(descs.begin() + i, data);
            //data->incRefCount();
    }
    void appendDesc(Data * data)
    {
        descs.push_back(data);
        //if(data) data->incRefCount();
    }
    void *convertToDsc();

protected:
    std::vector<Data*> descs;

};



////////////////////////////////////////////////////////////////////////////////
//  LIST DATA  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class EXPORT List: public Apd
{
public:
    List(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
        Apd(units, error, help, validation)
    {
        dtype = DTYPE_LIST;
    }

    List(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
        Apd(nData, descs, units, error, help, validation)
    {
        dtype = DTYPE_LIST;
    }

    void append(Data * data) {
        appendDesc(data);
    }

    void remove(Data * data) {
        descs.erase(std::remove(descs.begin(), descs.end(), data), descs.end());
        data->decRefCount();
    }

    void remove(std::size_t i) {
        descs.at(i)->decRefCount();
        descs.erase(descs.begin() + i);
    }

    void insert(std::size_t i, Data *data) {
        descs.insert(descs.begin() + i, data);
        data->incRefCount();
    }

    Data * getElementAt(std::size_t i) {
        descs.at(i)->incRefCount();
        return descs[i];
    }
};

////////////////////////////////////////////////////////////////////////////////
//  DICTIONARY DATA  ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class EXPORT Dictionary: public Apd
{
public:
    Dictionary(Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
        Apd(units, error, help, validation)
    {
        dtype = DTYPE_DICTIONARY;
    }

    Dictionary(int nData, Data **descs, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0):
        Apd(nData, descs, units, error, help, validation)
    {
        dtype = DTYPE_DICTIONARY;
    }

    Data * getItem(String *strData) {
        for(std::size_t i = 0; i < descs.size(); i += 2) {
            if(strData->equals(descs[i])) {
                descs[i+1]->incRefCount();
                return descs[i+1];
            }
        }
        return 0;
    }

    void setItem(String *strData, Data *data) {
        for(std::size_t i = 0; i < descs.size(); i += 2) {
            if(strData->equals(descs[i])) {
                descs[i]->decRefCount();
                descs[i] = strData;
                strData->incRefCount();

                descs[i+1]->decRefCount();
                descs[i+1] = data;
                data->incRefCount();
                return;
            }
        }

        descs.push_back(strData);
        descs.push_back(data);
        strData->incRefCount();
        data->incRefCount();
    }

    std::size_t len() {
        return Apd::len()/2;
    }
};





////////////////////////////////////////////////////////////////////////////////
//  TREE DATA   ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class Tree;
class TreeNode;
class TreeNodeArray;

///
/// \brief The TreeNode class object description of DTYPE_NID
///
class  EXPORT TreeNode: public Data
{
    friend	EXPORT std::ostream &operator<<(std::ostream &stream, TreeNode *treeNode)
    {
        stream << treeNode->getFullPathStr();
        return stream;
    }
protected:

    Tree *tree;
    int   nid;

    //From Data
    virtual bool isImmutable() {return false;}
    virtual void *convertToDsc();
    virtual bool getFlag(int flagOfs);
    virtual void setFlag(int flagOfs, bool val);

    virtual char getNciChar(int itm);
    virtual int getNciInt(int itm);
    virtual int64_t getNciInt64(int itm);
    virtual std::string getNciString(int itm);

public:
    virtual Data *data();

    ///
    /// \brief TreeNode constructor
    /// \param nid node id (required)
    /// \param tree the tree that this node belongs to
    ///
    TreeNode(int nid, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
    TreeNode() {tree = 0;}
    //Force new and delete in dll for windows
    void *operator new(size_t sz);
    void operator delete(void *p);

    /// Get the associated tree instance
    virtual Tree *getTree() { return tree; }

    /// Set the associated Tree instance
    virtual void setTree(Tree *tree) {this->tree = tree;}

    /// Get the path name for this node
    virtual char *getPath();

    /// Get the minimum path name for this node
    /// \ref getNci() with Nci code NciMINPATH
    virtual char *getMinPath();

    /// Get the full path name for this node
    /// \ref getNci() with code NciFULLPATH
    virtual char *getFullPath();

    /// Get the name of this node
    /// \ref getNci() with code NciNODE_NAME
    virtual char *getNodeName();

    /// Get the original path within the conglomerate to which the node belongs
    /// \ref getNci() with code NciORIGINAL_PART_NAME
    virtual char *getOriginalPartName();

    /// %String output version of getPath()
    virtual std::string getPathStr();

    /// %String output version of getMinPath()
    virtual std::string getMinPathStr();

    /// %String output version of getFullPath()
    virtual std::string getFullPathStr();

    /// %String output version of getNodeName()
    virtual std::string getNodeNameStr();

    /// %String output version of getOriginalPartName()
    virtual std::string getOriginalPartNameStr();


    /// Retrieve node from this tree by its realPath string
    virtual TreeNode *getNode(char const * relPath);

    /// Retrieve node from this tree by its realPath string
    virtual TreeNode *getNode(String *relPathStr);

    virtual Data *getData();
    virtual void putData(Data *data);
    virtual void deleteData();

    /// virtual function to resolve node id in the active tree.
    /// This is called each time a path to nid conversion is needed.
    virtual void resolveNid() {}

    virtual int getNid() { return nid;}

    /// Returns true if the node is On
    virtual bool isOn();

    /// Set on/off for that node
    virtual void setOn(bool on);

    /// Return the length in bytes of the contained data.
    /// \ref getNci() with code NciLENGTH
    virtual int getLength();

    /// Return the size in bytes of  (possibly compressed) associated data.
    /// \ref getNci() with code NciRLENGTH
    virtual int getCompressedLength();

    /// Return the Group/Id of the last writer for that node.
    /// \ref getNci() with code NciOWNER_ID
    virtual int getOwnerId();

    /// Return the status of the completed action (if the node contains ActionData).
    /// \ref getNci() with code NciSTATUSNciSTATUS
    virtual int getStatus();

    /// Get the time of the last data insertion
    /// \ref getNci() with code NciTIME_INSERTED
    virtual int64_t getTimeInserted();

    /// Do specified method for this node (valid only if it belongs to a conglomerate)
    virtual void doMethod(char *method);

    ///  Return true if this is setup data (i.e. present in the the model)
    virtual bool isSetup();


    ///@{
    ///  Nci Flags access
    virtual bool isWriteOnce();
    virtual void setWriteOnce(bool flag);
    virtual bool isCompressOnPut();
    virtual void setCompressOnPut(bool flag);
    virtual bool isNoWriteModel();
    virtual void setNoWriteModel(bool flag);
    virtual bool isNoWriteShot();
    virtual void setNoWriteShot(bool flag);
    virtual bool isEssential();
    virtual void setEssential(bool flag);
    virtual bool isIncludedInPulse();
    virtual void setIncludedInPulse(bool flag);
    virtual bool isIncludeInPulse();
    virtual void setIncludeInPulse(bool flag);
    virtual bool isMember();
    virtual bool isChild();
    ///@}

    /// Get the parent of this node.
    /// Ref to \ref getNci() with code NciPARENT
    virtual TreeNode *getParent();

    /// Return next sibling.
    /// Ref to \ref getNci() with code NciBROTHER
    virtual TreeNode *getBrother();

    /// Get the first child of this node.
    /// Ref to \ref getNci() with code NciCHILD
    virtual TreeNode *getChild();

    /// Get the first member of this node.
    /// Ref to \ref getNci() with code NciMEMBER
    virtual TreeNode *getMember();

    /// Get the number of members for this node.
    /// Ref to \ref getNci() with code NciNUMBER_OF_MEMBERS
    virtual int getNumMembers();

    /// Get the number of children for this node.
    /// Ref to \ref getNci() with code NciNUMBER_OF_CHILDREN
    virtual int getNumChildren();

    /// Get the number of all descendants (members + children)fir this node.
    /// Ref to \ref getNci with codes NciNUMBER_OF_MEMBERS and NciNUMBER_OF_MEMBERS
    virtual int getNumDescendants();

    // NOTE: [andrea] java implementation discrepancy (java uses TreeNodeArray instance)
    /// Get all che child nodes for this node.
    virtual TreeNode **getChildren(int *numChildren);

    /// Return  all the members of this node
    virtual TreeNode **getMembers(int *numChildren);

    /// Get all the descendant (members + children)for this node
    virtual TreeNode **getDescendants(int *numChildren);

    /// Return Nci class name (NciCLASS) as c string
    virtual const char *getClass();

    /// Return Nci descriptor type (NciDTYPE) as c string
    virtual const char *getDType();

    /// Return Nci node usage (NciUSAGE) as c string
    virtual const char *getUsage();

    /// Get index of the node in the corresponding conglomerate.
    /// Ref to \ref getNci() with code NciCONGLOMERATE_ELT
    virtual int getConglomerateElt();

    /// Return the number of the elements for the conglomerate to which the node
    ///  belongs
    virtual int getNumElts();

    /// Return the array of nodes corresponding to the elements of the
    /// conglomeate to which the node belongs
    virtual TreeNodeArray *getConglomerateNodes();

    /// Return the depth of the node in the tree.
    /// Ref to \ref getNci() with code NciDEPTH
    virtual int getDepth();

    /// Return true if the node contains versions (Nci flags)
    virtual bool containsVersions();

    // SEGMENTS //

    // NOTE: [andrea] there are missed members ( vs java impl )
    /// Begin a new data segment
    virtual void beginSegment(Data *start, Data *end, Data *time, Array *initialData);
	/// Begin and fill a new data segment
    virtual void makeSegment(Data *start, Data *end, Data *time, Array *initialData);

	//Begin and fill a new data segment. At the same time make a resampled minmax version (two samples (min and max) every 100 original samples)
    virtual void makeSegmentMinMax(Data *start, Data *end, Data *time, Array *initialData, TreeNode*resampledNode, int resFactor = 100);

	//Begin and fill a new data segment. At the same time make a resampled version
    virtual void makeSegmentResampled(Data *start, Data *end, Data *time, Array *initialData, TreeNode*resampledNode);

    /// Write (part of) data segment
    virtual void putSegment(Array *data, int ofs);

    /// Update start, end time and dimension for the last segment
    virtual void updateSegment(Data *start, Data *end, Data *time);

    /// Update start, end time and dimension for the specified segment
    virtual void updateSegment(int idx, Data *start, Data *end, Data *time);

    /// Get the number of segments
    virtual int getNumSegments();

    /// Instantiate two arrays with Segments starts and ends
    virtual void getSegmentLimits(int segmentIdx, Data **start, Data **end);

    /// Get data form the selected segment
    virtual Array *getSegment(int segIdx);

    /// Get the selected segment dimension
    virtual Data *getSegmentDim(int segIdx);

    /// Get both segment and segment dimension
    virtual void getSegmentAndDimension(int segIdx, Array *&segment, Data *&dimension);

    /// Set scale for segmented Node
    virtual void setSegmentScale(Data* scale);

    /// Get scale of segmented Node
    virtual Data* getSegmentScale();

    /// Begin a timestamted segment
    virtual void beginTimestampedSegment(Array *initData);

    /// Make a timestamped segment
    virtual void makeTimestampedSegment(Array *data, int64_t *times);

    /// Write (part of) data in a timestamped segment
    virtual void putTimestampedSegment(Array *data, int64_t *times);

    /// Writre a single row of timestamped data
    virtual void putRow(Data *data, int64_t *time, int size = 1024);

    /// Get info for the node identified by sefIdx, or the last node if -1 is
    /// passed as segIdx argument. The function returns dtype of the contained
    /// data and the row dimensions. The function provides: In dimct the number
    /// of dimensions is stored, in dims the array of dimensions, in nextRow
    /// the position of the next free row. The dims always reflects the segment
    /// dimension passed by beginSegment while the next will get the current
    /// position reached in this segment by the putSegment function.
    ///
    /// \note dtype, dimct and nextRow are pointer of size 1 array, while the
    /// dims dimension must be allocated array of size 8 integers! Furthermore,
    /// at the time of writing the returned dims are actually written int
    /// reverse order for a mismatch in the row/column order within the treeshr
    /// library
    ///
    virtual void getSegmentInfo(int segIdx, char *dtype, char *dimct, int *dims, int *nextRow);

    /// Retrieve node tags as array of strings
    virtual StringArray *findTags();

    // EDIT METHODS //

    /// This method adds a new node to the tree that this instance belongs to.
    /// The name specifies the relative or the full path of the new node and
    /// the usage is a string that defines the type of the new node. See \ref
    /// Tree::addNode for reference. If the relative path is chosen the new
    /// node will be set as child/member of the current.
    ///
    virtual TreeNode *addNode(char const * name, char const * usage);

    /// This removes a node indentified by its path from the tree that holds the
    /// current node instance. See \ref Tree::remove() for reference.
    ///
    virtual void remove(char const * name);

    /// Rename current node instance in tree setting a new path indentified by
    /// the newName argument that must be in form of an absolute path string.
    virtual void rename(std::string const & newName);

    /// Move the node setting a new parent and changing the node name
    virtual void move(TreeNode *parent, std::string const & newName);

    /// Move the node setting a new parent.
    virtual void move(TreeNode *parent);

    /// Add a device to the tree that holds current node instance. The argument
    /// name will be the name of the new created device and the type will be
    /// serched within the device library. See \ref Tree::addDevice for details.
    ///
    virtual TreeNode *addDevice(char const * name, char const * type);

    /// Add a tag to the current tree that hold this instance pointing to this
    /// node. See \ref Tree::addTag() for reference.
    ///
    virtual void addTag(std::string const & tagName);

    /// Removes a tag added to the current tree and indentified by tagName.
    virtual void removeTag(std::string const & tagName);

    /// Set this node to be a subtree of the tree that holds it.
    virtual void setSubtree(bool isSubtree);
};

/////////////////End Class TreeTreeNode///////////////

///
/// \brief The TreeNodeThinClient class provides (a subset of) TreeNode functionality using thin client mddip connection
///

class  EXPORT TreeNodeThinClient: public TreeNode
{
    friend	EXPORT std::ostream &operator<<(std::ostream &stream, TreeNodeThinClient *treeNode)
    {
        stream << treeNode->getFullPathStr();
        return stream;
    }
protected:

    Connection *connection;

    //From Data
    virtual bool isImmutable() {return false;}
    // virtual void *convertToDsc();  Use superclass implementation
    // bool getFlag(int flagOfs); Use superclass implementation
    // void setFlag(int flagOfs, bool val);  Use superclass implementation
    virtual char getNciChar(int itm);
    virtual int getNciInt(int itm);
    virtual int64_t getNciInt64(int itm);
    virtual std::string getNciString(int itm);

public:
    // virtual Data *data();  Use superclass implementation

    ///
    /// \brief TreeNodeThinClient constructor
    /// \param nid node id (required)
    /// \param Connection the Connection object used to communicate to mdsip server
    ///
    TreeNodeThinClient(int nid, Connection *connection, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);

    /// Get the associated tree instance
    virtual Tree *getTree() {throw MdsException("getTree() not supported for TreeNodeThinClient object"); return NULL; }

    /// Set the associated Tree instance
    virtual void setTree(Tree *tree) {(void)tree; throw MdsException("setTree() not supported for TreeNodeThinClient object"); }

    /// Get the path name for this node
     virtual char *getPath();

    /// Get the minimum path name for this node
    /// \ref getNci() with Nci code NciMINPATH
    // virtual char *getMinPath();   Use superclass implementation

    /// Get the full path name for this node
    /// \ref getNci() with code NciFULLPATH
    // virtual char *getFullPath();   Use superclass implementation

    /// Get the name of this node
    /// \ref getNci() with code NciNODE_NAME
    // virtual char *getNodeName();   Use superclass implementation

    /// Get the original path within the conglomerate to which the node belongs
    /// \ref getNci() with code NciORIGINAL_PART_NAME
    // virtual char *getOriginalPartName();   Use superclass implementation

    /// %String output version of getPath()
    // virtual std::string getPathStr();   Use superclass implementation

    /// %String output version of getMinPath()
    // virtual std::string getMinPathStr();   Use superclass implementation

    /// %String output version of getFullPath()
    // virtual std::string getFullPathStr();   Use superclass implementation

    /// %String output version of getNodeName()
    // virtual std::string getNodeNameStr();   Use superclass implementation

    /// %String output version of getOriginalPartName()
    // virtual std::string getOriginalPartNameStr();   Use superclass implementation


    /// Retrieve node from this tree by its realPath string
    virtual TreeNode *getNode(char const * relPath)
    {
	(void)relPath;
	throw MdsException("getNode() not supported for TreeNodeThinClient object"); return NULL;
    }
    /// Retrieve node from this tree by its realPath string
    virtual TreeNode *getNode(String *relPathStr)
    {
        (void)relPathStr;
	throw MdsException("getNode() not supported for TreeNodeThinClient object"); return NULL;
    }

    virtual Data *getData();
    virtual void putData(Data *data);
    virtual void deleteData();

    /// virtual function to resolve node id in the active tree.
    /// This is called each time a path to nid conversion is needed.
    virtual void resolveNid() {}

    virtual int getNid() { return nid;}

    /// Returns true if the node is On
    virtual bool isOn();

    /// Set on/off for that node
    virtual void setOn(bool on);

    /// Return the length in bytes of the contained data.
    /// \ref getNci() with code NciLENGTH
    // virtual int getLength();   Use superclass implementation

    virtual int getCompressedLength(){throw MdsException("getCompressedLength() not supported for TreeNodeThinClient object");}

    /// Return the Group/Id of the last writer for that node.
    /// \ref getNci() with code NciOWNER_ID
    // virtual int getOwnerId();   Use superclass implementation

    /// Return the status of the completed action (if the node contains ActionData).
    /// \ref getNci() with code NciSTATUSNciSTATUS
    // virtual int getStatus();   Use superclass implementation

    /// Get the time of the last data insertion
    /// \ref getNci() with code NciTIME_INSERTED
    // virtual int64_t getTimeInserted();   Use superclass implementation

    /// Do specified method for this node (valid only if it belongs to a conglomerate)
    virtual void doMethod(char *method){(void)method; throw MdsException("doMethod() not supported for TreeNodeThinClient object");}

    ///  Return true if this is setup data (i.e. present in the the model)
    // virtual bool isSetup();  Use superclass implementation


    ///@{
    ///  Nci Flags access
    // virtual bool isWriteOnce();  Use superclass implementation
    //virtual void setWriteOnce(bool flag);  Use superclass implementation
    //virtual bool isCompressOnPut();  Use superclass implementation
    //virtual void setCompressOnPut(bool flag);  Use superclass implementation
    //virtual bool isNoWriteModel();  Use superclass implementation
    //virtual void setNoWriteModel(bool flag);  Use superclass implementation
    //virtual bool isNoWriteShot();  Use superclass implementation
    //virtual void setNoWriteShot(bool flag);  Use superclass implementation
    //virtual bool isEssential();  Use superclass implementation
    //virtual void setEssential(bool flag);
    //virtual bool isIncludedInPulse();  Use superclass implementation
    //virtual void setIncludedInPulse(bool flag);  Use superclass implementation
    //virtual bool isIncludeInPulse();  Use superclass implementation
    //virtual void setIncludeInPulse(bool flag);  Use superclass implementation
    //virtual bool isMember();  Use superclass implementation
    //virtual bool isChild();  Use superclass implementation
    ///@}

    /// Get the parent of this node.
    /// Ref to \ref getNci() with code NciPARENT
    virtual TreeNode *getParent(){throw MdsException("getParent() not supported for TreeNodeThinClient object");}

    /// Return next sibling.
    /// Ref to \ref getNci() with code NciBROTHER
    virtual TreeNode *getBrother(){throw MdsException("getBrother() not supported for TreeNodeThinClient object");}

    /// Get the first child of this node.
    /// Ref to \ref getNci() with code NciCHILD
    virtual TreeNode *getChild(){throw MdsException("getChild() not supported for TreeNodeThinClient object");}

    /// Get the first member of this node.
    /// Ref to \ref getNci() with code NciMEMBER
    virtual TreeNode *getMember(){throw MdsException("getMember() not supported for TreeNodeThinClient object");}

    /// Get the number of members for this node.
    /// Ref to \ref getNci() with code NciNUMBER_OF_MEMBERS
    //virtual int getNumMembers();  Use superclass implementation

    /// Get the number of children for this node.
    /// Ref to \ref getNci() with code NciNUMBER_OF_CHILDREN
    //virtual int getNumChildren();  Use superclass implementation

    /// Get the number of all descendants (members + children)fir this node.
    /// Ref to \ref getNci with codes NciNUMBER_OF_MEMBERS and NciNUMBER_OF_MEMBERS
    //virtual int getNumDescendants();  Use superclass implementation

    // NOTE: [andrea] java implementation discrepancy (java uses TreeNodeArray instance)
    /// Get all che child nodes for this node.
    virtual TreeNode **getChildren(int *numChildren){(void)numChildren; throw MdsException("getChildren() not supported for TreeNodeThinClient object");}

    /// Return  all the members of this node
    virtual TreeNode **getMembers(int *numChildren){(void)numChildren; throw MdsException("getMembers() not supported for TreeNodeThinClient object");}

    /// Get all the descendant (members + children)for this node
    virtual TreeNode **getDescendants(int *numChildren){(void)numChildren;throw MdsException("getDescendants() not supported for TreeNodeThinClient object");}




    /// Return Nci class name (NciCLASS) as c string
    //virtual const char *getClass();  Use superclass implementation

    /// Return Nci descriptor type (NciDTYPE) as c string
    //virtual const char *getDType();  Use superclass implementation

    /// Return Nci node usage (NciUSAGE) as c string
    //virtual const char *getUsage();  Use superclass implementation




    /// Get index of the node in the corresponding conglomerate.
    /// Ref to \ref getNci() with code NciCONGLOMERATE_ELT
    //virtual int getConglomerateElt();  Use superclass implementation

    /// Return the number of the elements for the conglomerate to which the node
    ///  belongs
    //virtual int getNumElts();  Use superclass implementation

    /// Return the array of nodes corresponding to the elements of the
    /// conglomeate to which the node belongs
    virtual TreeNodeArray *getConglomerateNodes(){throw MdsException("getConglomerateNodes() not supported for TreeNodeThinClient object");}

    /// Return the depth of the node in the tree.
    /// Ref to \ref getNci() with code NciDEPTH
    // virtual int getDepth();  Use superclass implementation

    /// Return true if the node contains versions (Nci flags)
    // virtual bool containsVersions();  Use superclass implementation

    // SEGMENTS //

    // NOTE: [andrea] there are missed members ( vs java impl )
    /// Begin a new data segment
    virtual void beginSegment(Data *start, Data *end, Data *time, Array *initialData);
	/// Begin and fill a new data segment
    virtual void makeSegment(Data *start, Data *end, Data *time, Array *initialData);

	//Begin and fill a new data segment. At the same time make a resampled minmax version (two samples (min and max) every 100 original samples)
    virtual void makeSegmentMinMax(Data *start, Data *end, Data *time, Array *initialData, TreeNode*resampledNode, int resFactor = 100)
    {
      (void)start;
      (void)end;
      (void)time;
      (void)initialData;
      (void)resampledNode;
      (void)resFactor;
       throw MdsException("makeSegmentMinMax() not supported for TreeNodeThinClient object");
    }
 	//Begin and fill a new data segment. At the same time make a resampled version
    virtual void makeSegmentResampled(Data *start, Data *end, Data *time, Array *initialData, TreeNode*resampledNode)
    {
      (void)start;
      (void)end;
      (void)initialData;
      (void)time;
      (void)resampledNode;
      throw MdsException("makeSegmentResampled() not supported for TreeNodeThinClient object");
    }

    /// Write (part of) data segment
    virtual void putSegment(Array *data, int ofs);

    /// Update start, end time and dimension for the last segment
    virtual void updateSegment(Data *start, Data *end, Data *time)
    {
      (void)start;
      (void)end;
      (void)time;
      throw MdsException("updateSegment() not supported for TreeNodeThinClient object");
    }

    /// Update start, end time and dimension for the specified segment
    virtual void updateSegment(int idx, Data *start, Data *end, Data *time)
    {
      (void)idx;
      (void)start;
      (void)end;
      (void)time;
      throw MdsException("updateSegment() not supported for TreeNodeThinClient object");
    }

    /// Get the number of segments
    virtual int getNumSegments();

    /// Instantiate two arrays with Segments starts and ends
    virtual void getSegmentLimits(int segmentIdx, Data **start, Data **end);

    /// Get data form the selected segment
    virtual Array *getSegment(int segIdx);

    /// Get the selected segment dimension
    virtual Data *getSegmentDim(int segIdx);

    /// Get both segment and segment dimension
    virtual void getSegmentAndDimension(int segIdx, Array *&segment, Data *&dimension);

    /// Begin a timestamted segment
    virtual void beginTimestampedSegment(Array *initData);

    /// Make a timestamped segment
    virtual void makeTimestampedSegment(Array *data, int64_t *times);

    /// Write (part of) data in a timestamped segment
    virtual void putTimestampedSegment(Array *data, int64_t *times);

    /// Writre a single row of timestamped data
    virtual void putRow(Data *data, int64_t *time, int size = 1024);

    /// Get info for the node identified by sefIdx, or the last node if -1 is
    /// passed as segIdx argument. The function returns dtype of the contained
    /// data and the row dimensions. The function provides: In dimct the number
    /// of dimensions is stored, in dims the array of dimensions, in nextRow
    /// the position of the next free row. The dims always reflects the segment
    /// dimension passed by beginSegment while the next will get the current
    /// position reached in this segment by the putSegment function.
    ///
    /// \note dtype, dimct and nextRow are pointer of size 1 array, while the
    /// dims dimension must be allocated array of size 8 integers! Furthermore,
    /// at the time of writing the returned dims are actually written int
    /// reverse order for a mismatch in the row/column order within the treeshr
    /// library
    ///
    virtual void getSegmentInfo(int segIdx, char *dtype, char *dimct, int *dims, int *nextRow)
    {
      (void)segIdx;
      (void)dtype;
      (void)dimct;
      (void)dims;
      (void)nextRow;
      throw MdsException("getSegmentInfo() not supported for TreeNodeThinClient object");
    }

    /// Retrieve node tags as array of strings
    virtual StringArray *findTags();

    // EDIT METHODS //

    /// This method adds a new node to the tree that this instance belongs to.
    /// The name specifies the relative or the full path of the new node and
    /// the usage is a string that defines the type of the new node. See \ref
    /// Tree::addNode for reference. If the relative path is chosen the new
    /// node will be set as child/member of the current.
    ///
    virtual TreeNode *addNode(char const * name, char const * usage)
    {
      (void)name;
      (void)usage;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// This removes a node indentified by its path from the tree that holds the
    /// current node instance. See \ref Tree::remove() for reference.
    ///
    virtual void remove(char const * name)
    {
      (void)name;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Rename current node instance in tree setting a new path indentified by
    /// the newName argument that must be in form of an absolute path string.
    virtual void rename(std::string const & newName)
    {
      (void)newName;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Move the node setting a new parent and changing the node name
    virtual void move(TreeNode *parent, std::string const & newName)
    {
      (void)parent;
      (void)newName;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Move the node setting a new parent.
    virtual void move(TreeNode *parent)
    {
      (void)parent;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Add a device to the tree that holds current node instance. The argument
    /// name will be the name of the new created device and the type will be
    /// serched within the device library. See \ref Tree::addDevice for details.
    ///
    virtual TreeNode *addDevice(char const * name, char const * type)
    {
      (void)name;
      (void)type;
     throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Add a tag to the current tree that hold this instance pointing to this
    /// node. See \ref Tree::addTag() for reference.
    ///
    virtual void addTag(std::string const & tagName)
    {
      (void)tagName;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Removes a tag added to the current tree and indentified by tagName.
    virtual void removeTag(std::string const & tagName)
    {
      (void)tagName;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }


    /// Set this node to be a subtree of the tree that holds it.
    virtual void setSubtree(bool isSubtree)
    {
      (void)isSubtree;
      throw MdsException("edit operations not supported for TreeNodeThinClient object");
    }

};

/////////////////End Class TreeNodeThinClient///////////////











class EXPORT TreePath: public TreeNode
{
public:
    TreePath(std::string const & path, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
    TreePath(char const * path, int len, Tree *tree, Data *units = 0, Data *error = 0, Data *help = 0, Data *validation = 0);
    void *convertToDsc();
    void resolveNid();

private:
    std::string path;
};

////////////////Class TreeNodeArray///////////////////////
class EXPORT TreeNodeArray
{
    TreeNode **nodes;
    int numNodes;
public:
    void *operator new(size_t sz);
    void operator delete(void *p);
    TreeNodeArray(TreeNode **nodes, int numNodes);
    TreeNodeArray(int *nids, int numNodes, Tree *tree);
    ~TreeNodeArray();
    StringArray *getPath();
    StringArray *getFullPath();
    Int32Array *getNid();
    Int8Array *isOn();
    void setOn(Int8Array *info);
    Int8Array *isSetup();
    Int8Array *isWriteOnce();
    void setWriteOnce(Int8Array *info);
    Int8Array *isCompressOnPut();
    void setCompressOnPut(Int8Array *info);
    Int8Array *isNoWriteModel();
    void setNoWriteModel(Int8Array *info);
    Int8Array *isNoWriteShot();
    void setNoWriteShot(Int8Array *info);
    Int32Array *getLength();
    Int32Array *getCompressedLength();
    StringArray *getUsage();
    int getNumNodes() { return numNodes;}
    TreeNode *operator[](int idx)
    {
        if(idx >= 0 && idx < numNodes)
            return nodes[idx];
        else
            return 0;
    }
};




////////////////////////////////////////////////////////////////////////////////
//  Tree  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// MDSplus provides a data storage mechanism for recording a wide variety of
/// information pertaining to experiments or simulations including, but not
/// limited to, data acquisition settings, comments, physical measurements,
/// data acquisition and analysis task information and analysis results.
/// Keeping all this information organized can be difficult especially with
/// large experiments or codes where there may be many thousands of data items.
/// MDSplus provides a hierarchical tree structure in its data storage so that
/// users can organize their data much like one would organize files in a file
/// system with directories and subdirectories.
///
/// A tree is a collection of nodes which describe something. All data in
/// MDSplus are stored in trees. There are two different kinds of nodes in the
/// MDSplus tree structure called members and children. One could consider
/// members to be similar to files in a file system while children are similar
/// to directories. Members normally contain data, (i.e. a measurement, a
/// calibration coefficient, a comment) while a child node, used for structure
/// only, cannot contain data. However, unlike a file system, both members and
/// children nodes can have subnodes of either members or children.
///
///  Tree shots
///  ----------
///
/// The trees which contain the data pertaining to an experiment or simulation
/// are stored on an MDSplus server as files. Each tree consists of a model
/// (shot # -1) and one or more pulse files with positive shot numbers. Special
/// shot numbers:
///
///     -1 - model
///      0 - current shot
///     >1 - pulse files
///

class EXPORT Tree
{
    friend void setActiveTree(Tree *tree);
    friend Tree *getActiveTree();

protected:

    std::string name;
    int shot;
    void *ctx;
    bool fromActiveTree;

public:

    /// Builds a new Tree object instance creating or attaching to the named
    /// tree. The tree name has to match the path envoronment variable
    /// xxx_path, and the shot is the shot number the Tree instance will point
    /// to.
    ///
    Tree(char const * name, int shot);
    Tree(char const * name, int shot, void *ctx);


    /// Builds a new Tree object instance creating or attaching to the named
    /// tree. The tree name has to match the path envoronment variable
    /// xxx_path, and the shot is the shot number the Tree instance will point
    /// to. Four opening mode are availabe:
    ///
    /// | opening mode | description                                                     |
    /// |--------------|-----------------------------------------------------------------|
    /// | NEW          | create new tree with target parse files if not present          |
    /// | EDIT         | open tree in edit mode to access the tree structure for writing |
    /// | READONLY     | open tree in read only mode                                     |
    /// | NORMAL       | set the tree for normal operations reading and writing data     |
    ///
    Tree(char const * name, int shot, char const * mode);

    ~Tree();

    void *operator new(size_t sz);
    void operator delete(void *p);

    /// Set current shot number
    static void setCurrent(char const * treeName, int shot);

    /// Get current shot number
    static int getCurrent(char const * treeName);

    /// Return current tree context (see treeshr library)
    void *getCtx() {return ctx;}

    /// Reopen target tree in edit mode or in normal mode according to the
    /// value passed as argument. The default behavior is to reopen for edit.
    ///
    void edit(const bool st = true);

    /// \note This function has been discontinued and the TreeQuit is now
    /// handled by destructor only. Anyway the declaration is maintained here
    /// to keep the labview compatibility.
    ///
    void quit() {}

    /// writes tree changes to the target storage
    void write();

    /// Access treenode by path using const char string
    TreeNode *getNode(char const *path);

    /// Access treenode by path using a \ref TreePath mdsplus object
    TreeNode *getNode(TreePath *path);

    /// Access treenode by path using a \ref String mdsplus object
    TreeNode *getNode(String *path);

    /// Get the name of the tree as c string
    const char *getName() const { return name.c_str(); }

    /// Get the name of this tree as a std::string
    std::string getNameStr() const { return name; }

    /// Adds a node by path and usage. To each node we assign here a USAGE.
    /// This node characteristic defines and limits how a particular node can
    /// be used. Usage must be a strig matching one of the following:
    ///
    /// | usage    |   description                                             |
    /// |----------|-----------------------------------------------------------|
    /// | ACTION   | node that describes an \ref Action object                 |
    /// | ANY      | node scope not defined and it left here set for any use   |
    /// | AXIS     | node represent an axis object                             |
    /// | COMPOUND_DATA | node represents a \ref Compound object               |
    /// | DEVICE        | node is a device composite data used for acquisition |
    /// | DISPATCH      | node is a dispach of an action                       |
    /// | STRUCTURE     | used for tree structure - a "child" node definition  |
    /// | NUMERIC       | node contains simple numbers or arrays               |
    /// | SIGNAL        | node is a signal object                              |
    /// | SUBTREE       | node links to another tree                           |
    /// | TASK          | node represents a Task of a \ref Action              |
    /// | TEXT          | node will contain a text string                      |
    /// | WINDOW        | node is a window definition                          |
    ///
    TreeNode *addNode(char const * name, char const * usage);

    /// add device node to tree with name and device type
    TreeNode *addDevice(char const * name, char const * type);

    /// remove tree node by path
    void remove(char const *name);

    /// Get a new array of nodes in this tree or subtrees mathing the path name
    /// and usages given as arguments. Asterisk is available as a wildcard to
    /// set the search pattern while usages are found as a ordered bitmask.
    ///
    /// the usage codes are listed in \ref usagedef.h and the mask has to be
    /// compiled activating the related bit. For example to set all numeric and
    /// signals nodes in a tree:
    ///
    ///     AutoPointer<TreeNodeArray> nodes = tree->getNodeWild("***",
    ///                                                  1<<TreeUSAGE_NUMERIC &
    ///                                                  1<<TreeUSAGE_SIGNAL );
    ///
    TreeNodeArray *getNodeWild(char const *path, int usageMask);

    /// Get a new array of nodes in this tree or subtrees mathing the path
    /// name. Asterisk is available as a wildcard to set the search pattern.
    ///
    TreeNodeArray *getNodeWild(char const *path);

    /// Set the node that will be the default base for all the relative paths
    /// specified after this function is called. As a new tree is open the fist
    /// set default node is TOP (i.e the root of the tree).
    ///
    void setDefault(TreeNode *treeNode);

    /// Get the node that is the default base for all the relative paths
    /// specified using \ref SetDefault function. As a new tree is open the
    /// fist set default node is TOP (i.e the root of the tree).
    ///
    TreeNode *getDefault();

    /// This function returns true if the tree has been modified from the
    /// version written in the pulse file. See treeshr function \ref
    /// TreeGetDbi() called with code DbiMODIFIED.
    ///
    bool isModified();

    /// This function returns true if the tree has been opened in edit mode.
    /// See treeshr function \ref TreeGetDbi() called with code
    /// DbiOPEN_FOR_EDIT.
    ///
    bool isOpenForEdit();

    /// This function returns true if the tree has been opened for read only.
    /// See treeshr function \ref TreeGetDbi() called with code
    /// DbiOPEN_READONLY.
    ///
    bool isReadOnly();

    /// This function returns true if the tree has been created to keep stored
    /// data versioning. See treeshr function \ref TreeGetDbi() called with
    /// code DbiVERSIONS_IN_MODEL. \note This versioning feature will be not
    /// avaiable for segmented data.
    ///
    bool versionsInModelEnabled();

    /// This function returns true if the tree has been created to keep stored
    /// data versioning in the current pulse file. See treeshr function \ref
    /// TreeGetDbi() called with code DbiVERSIONS_IN_PULSE. \note The
    /// Versioning feature is not avaiable for segmented data.
    ///
    bool versionsInPulseEnabled();

    /// Activates the versioning feature in the tree model so each pulse that
    /// is created will have the versioning active. \note The Versioning
    /// feature is not avaiable for segmented data.
    ///
    void setVersionsInModel(bool enable);

    /// Activates the versioning feature for the current pulse so all data that
    /// is stored over the preexisting ones will be always accessible trhough a
    /// growing version id. \note This versioning feature will be not avaiable
    /// for segmented data.
    ///
    void setVersionsInPulse(bool enable);

    /// View data stored in tree from given start date when version control is
    /// enabled.
    ///
    void setViewDate(char *date);


    void setTimeContext(Data *start, Data *end, Data *delta);

    /// Create pulse makes a copy of the tree model file into a new pulse file
    /// ready to be filled with new acquired data. When this function is called
    /// MDSplus asks that the model file is not opened for edit so do not
    /// create pulses if you opened the tree in EDIT mode. For example a
    /// correct usage would be:
    ///
    ///     Tree * tree = new Tree("test_tree",-1);
    ///     tree->createPulse(1);
    ///     delete tree;
    ///     tree = new Tree("test_tree",1);
    ///
    /// Please note that as the createPulse makes a copy of the model file a
    /// plenty tree with all model nodes is written into the new shot. Another
    /// correct method to create a pulse file would be also to call constructor
    /// using "NEW" mode:
    ///
    ///     Tree * tree = new Tree("test_tree",1,"NEW");
    ///
    /// but in this case an empty tree will be written to file without any
    /// node defined form model.
    ///
    void createPulse(int shot);

    /// Delete pulse file identified by shot id. After this function is called
    /// all tree instance opened with delete shot number are no more valid and
    /// will throw exception for any content access. Note that the model file
    /// can not be deleted.
    ///
    void deletePulse(int shot);


    StringArray *findTags(char *wild);
    void removeTag(char const * tagName);
    int64_t getDatafileSize();
};

////////////////////////////////////////////////////////////////////////////////
//  Event  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
///  Asynchronous events in MDSplus
///  ------------------------------
/// In MDSplus it is possible to generate asynchronous events as well as
/// waiting for them. Events are often used to synchronize data visualization
/// (for instance an update event name can be defined in the jScope Setup Data
/// pop-up window, forcing an update of the displayed signal whenever such an
/// event is received). Data acquisition events in this case are generated by
/// the data acquisition routines (usually the methods of the devices involved
/// in data acquisition).
///
/// MDSplus defines this Event class, for event generation and reception. In
/// addition to notification, MDSplus events can bring data, either raw or
/// "packed" into Data instances. The event issuer can then optionally
/// associate data top the event, which will be received by every listener for
/// that event. Events can be generated by the following methods:
///
/// * setevent(name) to generate an event with no associated data
///
/// * seteventRaw(name, buffer) to generate an event carrying a buffer of bytes
///
/// * setevent(name, Data) to generate an event carrying a Data instance
///
/// To handle event reception it is necessary to extend Event class by
/// associating the actions to be executed upon event reception. Class Event in
/// fact defines the virtual function \ref run(), which is called after the
/// class instantiation whenever the event with the specified name has been
/// received. The overriden run() method can then retrieve received data by
/// calling superclass \ref getData() or \ref getRaw() methods.
///
/// The following example code shows how to define a new Event class called
/// MyEvent whose action is to print the content of the received buffer (it is
/// possible to test the programs using the command: setevent event string).
///
/// \code{.cpp}
/// class MyEvent : public Event
/// {
/// public:
///   MyEvent(char *name) : Event(name) {}
///
///   void run()
///   {
///     size_t bufSize;
///     char *name = getName();              //Get the name of the event
///     char *date = getTime()->getDate();   //Get the event reception time
///     const char *buf =  getRaw(&bufSize); //Get raw data
///     char *str = new char[bufSize+1];     //Make it a string
///     memcpy(str, buf, bufSize);
///     str[bufSize] = 0;
///     std::cout << "RECEIVED EVENT " << name
///               << " AT " << date
///               << " WITH DATA  " << str << "\n";
///   }
/// };
/// \endcode
///
///
/// \note There are also two MDSplus executables which can be called from a
/// console: <tt> wfevent Event_name </tt> and <tt> setevent event_name string
/// </tt>. The former suspends until an event with that name has been received;
/// the latter generates such an event.
///

class EXPORT Event {
public:

    Event(const char *name);

    virtual ~Event();

    const char * getRaw(std::size_t * size) const;

    Data * getData() const;

    Uint64 *getTime() const;

    const char *getName() const;

    void start();

    void stop();

    bool isStarted() const;

    void wait(std::size_t secs = 0);

    Data *waitData(std::size_t secs = 0) {
        wait(secs); return getData();
    }

    char const * waitRaw(std::size_t * size, std::size_t secs = 0) {
        wait(secs); return getRaw(size);
    }

    virtual void run() {}

    static void setEvent(const char *evName) { setEventRaw(evName, 0, NULL); }
    static void setEventRaw(const char *evName, int bufLen, char *buf);
    static void setEvent(const char *evName, Data *evData);

    //To keep them compatible with python
    static void setevent(char *evName) {setEvent(evName); }
    static void seteventRaw(char *evName, int bufLen, char *buf){setEventRaw(evName, bufLen, buf);}
    static void setevent(char *evName, Data *evData);


protected:
    void notify();
    virtual void connectToEvents();
    virtual void disconnectFromEvents();

private:
    Event() {}
    Event(const Event &){}
    friend void eventAst(void *arg, int len, char *buf);


    std::string eventName;
    std::string eventBuf;
    int eventId;
    int64_t eventTime;
    ConditionVar condition;
};







///////////////remote data access classes ////////////////
//
///  mdsip Connection in MDSplus
///  ------------------------------
///
/// Connection objects are used to establish a remote connection to a mdsip
/// server. Connection is carried out by the object constructor and then it is
/// possible to evaluate remote expressions The basic methods of Conneciton
/// class are:
///
/// * openTree(char *tree, int shot) Open a tree at the mdsip server side
/// * closeTree(char *tree, int shot) Close the remote tree
/// * setDefault(char *path) Set default position in remote tree
/// * Data *get(const char *expr) Remote expression evaluation
/// * Data *get(const char *expr, Data **args, int nArgs) Remote expression
///   evaluation with arguments
/// * put(const char *path, char *expr, Data **args, int nArgs) Put an
///   expression (with arguments) in remote tree
///
/// Class connection allows also the remote evaluation and put of multiple
/// expressions. Support classes GetMany and PutMany are used for this
/// purpose. GetMany and PutMany provide methods for inserting expressions to
/// be evaluated and stored, respectively. In addition both classes provide
/// method execute() that executes remote evaluation/store of the set of
/// associated expressions. GetMany and PutMany are not instantiated directly,
/// but are obtained by Connection Facotry methods getMany() and putMany()
/// Class connection provides also support for remote data streaming using
/// publish - subscribe pattern Clients can register to a connection instance
/// specifying a remote tree, shot and expression containing at least a
/// reference to a segmented node. Whenever new data are appended to the
/// segmented node at the server side registered listeners are notified and
/// receive the (chunk of) newly evaluated data and times A listener will
/// inherit abstract class DataStreamListener that defines a single method:
///
/// * void dataReceived(Data samples, Data times).
///
/// This method is asynchronously called by the connection frameworkwhen new
/// data are available. It is possible to register multiple listeners to the
/// same connection object. Once all listeners are registered, Connection
/// method startStreaming will start the streaming process. NOTE: when
/// streaming is started, the Connection instance is fully devoted to streaming
/// management. Therefore methods get and put cannot be called afterwards for
/// that Connection instance (they will freeze).
///
class Connection;


class EXPORT GetMany: public List
{
    Connection *conn;
    Dictionary *evalRes;
public:
    GetMany(Connection *conn)
    {
        this->conn = conn;
        evalRes = 0;
    }
    ~GetMany()
    {
        if(evalRes)
            delete evalRes;
    }
    void append(char *name, char *expr, Data **args, int nArgs);
    void insert(int idx, char *name, char *expr, Data **args, int nArgs);
    void insert(char *beforeName, char *name, char *expr, Data **args, int nArgs);
    void remove(char *name);
    void execute();
    Data *get(char *name);
};
class EXPORT PutMany: public List
{
    Connection *conn;
    Dictionary *evalRes;
public:
    PutMany(Connection *conn)
    {
        this->conn = conn;
        evalRes = 0;
    }
    ~PutMany()
    {
        if(evalRes)
            delete evalRes;
    }
    void append(char *name, char *expr, Data **args, int nArgs);
    void insert(int idx, char *name, char *expr, Data **args, int nArgs);
    void insert(char *beforeName, char *name, char *expr, Data **args, int nArgs);
    void remove(char *name);
    void execute();
    void checkStatus(char *name);
};


class EXPORT DataStreamListener
{
public:
    virtual void dataReceived(Data *samples, Data *times) = 0;
};



class EXPORT Connection
{
public:
    Connection(char *mdsipAddr, int clevel = 0);
    ~Connection();
    void openTree(char *tree, int shot);
    void closeAllTrees();
    void closeTree(char *tree UNUSED_ARGUMENT, int shot UNUSED_ARGUMENT)
    {
        closeAllTrees();
    }
    void setDefault(char *path);
    Data *get(const char *expr, Data **args, int nArgs);
    Data *get(const char *expr)
    {
        return get(expr, 0, 0);
    }
    void put(const char *path, char *expr, Data **args, int nArgs);
    PutMany *putMany()
    {
        return new PutMany(this);
    }
    GetMany *getMany()
    {
        return new GetMany(this);
    }
// Get TreeNode instance for (a subset of) TreeNode functionality in thin client configuration
    TreeNodeThinClient *getNode(char *path);
//Streaming stuff
    void registerStreamListener(DataStreamListener *listener, char *expr, char *tree, int shot);
    void unregisterStreamListener(DataStreamListener *listener);
    void startStreaming();
    void resetConnection();
    void checkDataAvailability();

private:
    MDS_DEBUG_ACCESS
    void lockLocal();
    void unlockLocal();
    void lockGlobal();
    void unlockGlobal();

	std::string mdsipAddrStr;
	int clevel;
    int sockId;
    Mutex mutex;
    static Mutex globalMutex;
	std::vector<DataStreamListener *> listenerV;
	std::vector<int> listenerIdV;
};

class EXPORT Scope
{
    int x, y, width, height;
    int idx;
public:
    Scope(const char *name, int x = 100, int y = 100, int width = 400, int height = 400);
    void plot(Data *x, Data *y , int row = 1, int col = 1, const char *color = "black");
    void oplot(Data *x, Data *y , int row = 1, int col = 1, const char *color = "black");
    void show();
};



//////////////Support functions////////
EXPORT Data *deserialize(char const * serialized);
EXPORT Data *deserialize(Data * serializedData);
EXPORT Data *compile(const char *expr);
EXPORT Data *compileWithArgs(const char *expr, int nArgs ...);
EXPORT Data *compile(const char *expr, Tree *tree);
EXPORT Data *compileWithArgs(const char *expr, Tree *tree, int nArgs ...);
EXPORT Data *execute(const char *expr);
EXPORT Data *executeWithArgs(const char *expr, int nArgs ...);
EXPORT Data *execute(const char *expr, Tree *tree);
EXPORT Data *executeWithArgs(const char *expr, Tree *tree, int nArgs ...);
//EXPORT Tree *getActiveTree();
//EXPORT void setActiveTree(Tree *tree);
//Required for handling dynamic memory allocated in a different DLL on windows
//in Debug configuration
EXPORT void deleteTreeNode(TreeNode *node);
EXPORT void deleteTree(Tree *tree);
EXPORT void deleteTreeNodeArray(TreeNodeArray *nodeArray);
EXPORT std::ostream &operator<<(std::ostream &stream, MDSplus::Data *data);


EXPORT void deleteNativeArray(char * array);
EXPORT void deleteNativeArray(unsigned char * array);
EXPORT void deleteNativeArray(short * array);
EXPORT void deleteNativeArray(unsigned short * array);
EXPORT void deleteNativeArray(int * array);
EXPORT void deleteNativeArray(unsigned int * array);
EXPORT void deleteNativeArray(int64_t * array);
EXPORT void deleteNativeArray(uint64_t * array);
EXPORT void deleteNativeArray(float * array);
EXPORT void deleteNativeArray(double * array);
EXPORT void deleteNativeArray(char ** array);
EXPORT void deleteNativeArray(Data ** array);





} // MDSplus

#endif // MDSOBJECTS_H
