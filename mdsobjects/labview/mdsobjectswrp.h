#ifndef MDSOBJECTSWRP_H
#define MDSOBJECTSWRP_H

#include <mdsobjects.h>
#include <stdint.h>
#include <stdint.h>
#ifdef __MINGW32__
#define _platdefines_H
#define _fundtypes_H
typedef uint8_t uChar;
typedef uint8_t uInt8;
typedef uint16_t uInt16;
typedef uint32_t uInt32;
typedef uint64_t uInt64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef float float32;
typedef double float64;
typedef uint32_t Bool32;
#ifndef __MINGW64__
#pragma pack(1)  //!!! Possible origin of memory misalignments in 64 bit architectures?
#endif

#endif

#include  <platdefines.h>
#include <extcode.h>
#include <fundtypes.h>

/* lv_prolog.h and lv_epilog.h set up the correct alignment for LabVIEW data. */
#include <lv_prolog.h>

typedef struct {
  LVBoolean status;
  int32 code;
  LStrHandle source;
} ErrorCluster;

typedef struct {
  int32 dimSize;
  int8 elt[1];
} LByteArr;
typedef LByteArr **LByteArrHdl;

typedef struct {
  int32 dimSize;
  uInt8 elt[1];
} LUByteArr;
typedef LUByteArr **LUByteArrHdl;

typedef struct {
  int32 dimSize;
  double elt[1];
} LDblArr;
typedef LDblArr **LDblArrHdl;

typedef struct {
  int32 dimSize;
  float elt[1];
} LFltArr;
typedef LFltArr **LFltArrHdl;

typedef struct {
  int32 dimSize;
  int32 elt[1];
} LIntArr;
typedef LIntArr **LIntArrHdl;

typedef struct {
  int32 dimSize;
  uInt32 elt[1];
} LUIntArr;
typedef LUIntArr **LUIntArrHdl;

typedef struct {
  int32 dimSize;
  int64 elt[1];
} LLngArr;
typedef LLngArr **LLngArrHdl;

typedef struct {
  int32 dimSize;
  uInt64 elt[1];
} LULngArr;
typedef LULngArr **LULngArrHdl;

typedef struct {
  int32 dimSize;
  int16 elt[1];
} LShtArr;
typedef LShtArr **LShtArrHdl;

typedef struct {
  int32 dimSize;
  uInt16 elt[1];
} LUShtArr;
typedef LUShtArr **LUShtArrHdl;

typedef struct {
  int32 dimSize;
  LStrHandle elm[];
} LStrArr;
typedef LStrArr **LStrArrHdl;

typedef struct {
  int32 dimSize;
  void *elt[1];
} LPtrArr;
typedef LPtrArr **LPtrArrHdl;

#include <lv_epilog.h>

void fillErrorCluster(MgErr code, const char *source, const char *message, ErrorCluster * error);

#ifdef __cplusplus
extern "C" {
#endif

/********** ARRAY **********/
  EXPORT void mdsplus_array_constructor(void **lvArrayPtrOut, ErrorCluster * error);
  EXPORT void mdsplus_array_destructor(void **lvArrayPtr);
  EXPORT void mdsplus_array_getByteArray(const void *lvArrayPtr, LByteArrHdl lvByteArrHdlOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_array_getDoubleArray(const void *lvArrayPtr, LDblArrHdl lvDblArrHdlOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_array_getElementAt(const void *lvArrayPtr, void **lvDataPtrOut, int dimIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_array_getElementAt_dims(const void *lvArrayPtr, void **lvDataPtrOut,
						 const LIntArrHdl lvIntArrHdlIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_array_getFloatArray(const void *lvArrayPtr, LFltArrHdl lvFltArrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_array_getIntArray(const void *lvArrayPtr, LIntArrHdl lvIntArrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_array_getInfo(const void *lvArrayPtr, char *clazzOut, char *dtypeOut,
				       short *lengthOut, LIntArrHdl lvIntArrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_array_getLongArray(const void *lvArrayPtr, LLngArrHdl lvLngArrHdlOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_array_getShape(const void *lvArrayPtr, LIntArrHdl lvIntArrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_array_getShortArray(const void *lvArrayPtr, LShtArrHdl lvShtArrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_array_getSize(const void *lvArrayPtr, int *sizeOut, ErrorCluster * error);
  EXPORT void mdsplus_array_getStringArray(const void *lvArrayPtr, LStrArrHdl lvStrArrHdlOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_array_setElementAt(const void *lvArrayPtr, int dimIn,
					    const void *lvDataPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_array_setElementAt_dims(const void *lvArrayPtr,
						 const LIntArrHdl lvIntArrHdlIn,
						 const void *lvDataPtrIn, ErrorCluster * error);

/********** COMPOUND **********/
  EXPORT void mdsplus_compound_constructor(void **lvCompoundPtrOut, ErrorCluster * error);
  EXPORT void mdsplus_compound_destructor(void **lvCompoundPtr);

/********** DATA **********/
  EXPORT void mdsplus_data_compile(void **lvDataPtrOut, const char *exprIn,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_compile_tree(void **lvDataPtrOut, const char *exprIn,
					   const void *lvTreePtrIn, ErrorCluster * error);
  EXPORT void mdsplus_data_data(const void *lvDataPtr, void **lvDataPtrOut,
				   ErrorCluster * error);
  EXPORT void mdsplus_data_decompile(const void *lvDataPtr, LStrHandle lvStrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_data_deserialize(void **lvDataPtrOut, const char *serializedIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_data_deserialize_data(void **lvDataPtrOut, const void *lvDataPtrIn,
					       ErrorCluster * error);
  EXPORT void mdsplus_data_destructor(void **lvDataPtr);
  EXPORT void mdsplus_data_evaluate(const void *lvDataPtr, void **lvDataPtrOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_execute(void **lvDataPtrOut, const char *exprIn,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_execute_tree(void **lvDataPtrOut, const char *exprIn,
					   const void *lvTreePtrIn, ErrorCluster * error);
  EXPORT void mdsplus_data_getByte(const void *lvDataPtr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_data_getByteArray(const void *lvDataPtr, LByteArrHdl lvByteArrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_data_getDouble(const void *lvDataPtr, double *doubleOut,
					ErrorCluster * error);
  EXPORT void mdsplus_data_getDoubleArray(const void *lvDataPtr, LDblArrHdl lvDblArrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_data_getError(const void *lvDataPtr, void **lvDataPtrOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_getFloat(const void *lvDataPtr, float *floatOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_getFloatArray(const void *lvDataPtr, LFltArrHdl lvFltArrHdlOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_data_getHelp(const void *lvDataPtr, void **lvDataPtrOut,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_getInfo(const void *lvDataPtr, char *clazzOut, char *dtypeOut,
				      short *lengthOut, LIntArrHdl lvIntArrHdlOut,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_getInt(const void *lvDataPtr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_data_getIntArray(const void *lvDataPtr, LIntArrHdl lvIntArrHdlOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_data_getLong(const void *lvDataPtr, int64_t * longOut,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_getLongArray(const void *lvDataPtr, LLngArrHdl lvLngArrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_data_getShape(const void *lvDataPtr, LIntArrHdl lvIntArrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_getShort(const void *lvDataPtr, short *shortOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_getShortArray(const void *lvDataPtr, LShtArrHdl lvShtArrHdlOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_data_getSize(const void *lvDataPtr, int *sizeOut, ErrorCluster * error);
  EXPORT void mdsplus_data_getString(const void *lvDataPtr, LStrHandle lvStrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_data_getStringArray(const void *lvDataPtr, LStrArrHdl lvStrArrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_data_getUnits(const void *lvDataPtr, void **lvDataPtrOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_getValidation(const void *lvDataPtr, void **lvDataPtrOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_data_plot(const void *lvDataPtr, ErrorCluster * error);
  EXPORT void mdsplus_data_serialize(const void *lvDataPtr, LByteArrHdl lvByteArrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_data_setError(void *lvDataPtr, const void *lvDataPtrIn,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_setHelp(void *lvDataPtr, const void *lvDataPtrIn,
				      ErrorCluster * error);
  EXPORT void mdsplus_data_setUnits(void *lvDataPtr, const void *lvDataPtrIn,
				       ErrorCluster * error);
  EXPORT void mdsplus_data_setValidation(void *lvDataPtr, const void *lvDataPtrIn,
					    ErrorCluster * error);

/********** EMPTY **********/
  EXPORT void mdsplus_empty_destructor(void **lvEmptyPtr);

/********** EVENT **********/
  EXPORT void mdsplus_event_constructor(void **lvEventPtrOut, const char *evNameIn,
					   ErrorCluster * error);
  EXPORT void mdsplus_event_destructor(void **lvEventPtr);
  EXPORT void mdsplus_event_waitData(const void *lvEventPtr, void **lvDataPtrOut,
					int *timeoutOccurred, ErrorCluster * error);
  EXPORT void mdsplus_event_wait(const void *lvEventPtr, int *timeoutOccurred,
				    ErrorCluster * error);
  // EXPORT void mdsplus_event_abort(const void *lvEventPtr, ErrorCluster * error);
  EXPORT void mdsplus_event_getName(const void *lvEventPtr, LStrHandle lvStrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_event_waitRaw(const void *lvEventPtr, LByteArrHdl lvByteArrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_event_setEvent(const char *evNameIn, ErrorCluster * error);
  EXPORT void mdsplus_event_setEvent_data(const char *evNameIn, const void *lvDataPtrIn,
					     ErrorCluster * error);
  EXPORT void mdsplus_event_setEventRaw(const char *evNameIn, LByteArrHdl lvByteArrHdlIn,
					   ErrorCluster * error);
  EXPORT void mdsplus_revent_constructor(void **lvREventPtrOut, const char *evNameIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_revent_destructor(void **lvREventPtr);
  EXPORT void mdsplus_revent_getData(const void *lvREventPtr, void **lvDataPtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_getName(const void *lvREventPtr, LStrHandle lvStrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_getRaw(const void *lvREventPtr, LByteArrHdl lvByteArrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_revent_getTime(const void *lvREventPtr, void **lvUint64PtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_run(const void *lvREventPtr, ErrorCluster * error);
  EXPORT void mdsplus_revent_setEvent(const char *evNameIn, const void *lvDataPtrIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventAndWait(const char *evNameIn, const void *lvDataPtrIn,
						ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventRaw(const char *evNameIn, LByteArrHdl lvByteArrHdlIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventRawAndWait(const char *evNameIn, LByteArrHdl lvByteArrHdlIn,
						   ErrorCluster * error);

/********** FLOAT32 **********/
  EXPORT void mdsplus_float32_constructor(void **lvFloat32PtrOut, float valIn,
					     ErrorCluster * error);
  EXPORT void mdsplus_float32_destructor(void **lvFloat32Ptr);
  EXPORT void mdsplus_float32_getByte(const void *lvFloat32Ptr, char *byteOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_float32_getDouble(const void *lvFloat32Ptr, double *doubleOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_float32_getFloat(const void *lvFloat32Ptr, float *floatOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_float32_getInt(const void *lvFloat32Ptr, int *intOut,
					ErrorCluster * error);
  EXPORT void mdsplus_float32_getLong(const void *lvFloat32Ptr, int64_t * longOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_float32_getShort(const void *lvFloat32Ptr, short *shortOut,
					  ErrorCluster * error);

/********** FLOAT32ARRAY **********/
  EXPORT void mdsplus_float32array_constructor(void **lvFloat32ArrayPtrOut,
						  const LFltArrHdl lvFltArrHdlIn,
						  ErrorCluster * error);
  EXPORT void mdsplus_float32array_constructor_dims(void **lvFloat32ArrayPtrOut,
						       const LFltArrHdl lvFltArrHdlIn,
						       const LIntArrHdl lvIntArrHdlIn,
						       ErrorCluster * error);
  EXPORT void mdsplus_float32array_destructor(void **lvFloat32ArrayPtr);

/********** FLOAT64 **********/
  EXPORT void mdsplus_float64_constructor(void **lvFloat64PtrOut, double valIn,
					     ErrorCluster * error);
  EXPORT void mdsplus_float64_destructor(void **lvFloat64Ptr);
  EXPORT void mdsplus_float64_getByte(const void *lvFloat64Ptr, char *byteOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_float64_getDouble(const void *lvFloat64Ptr, double *doubleOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_float64_getFloat(const void *lvFloat64Ptr, float *floatOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_float64_getInt(const void *lvFloat64Ptr, int *intOut,
					ErrorCluster * error);
  EXPORT void mdsplus_float64_getLong(const void *lvFloat64Ptr, int64_t * longOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_float64_getShort(const void *lvFloat64Ptr, short *shortOut,
					  ErrorCluster * error);

/********** FLOAT64ARRAY **********/
  EXPORT void mdsplus_float64array_constructor(void **lvFloat64ArrayPtrOut,
						  const LDblArrHdl lvDblArrHdlIn,
						  ErrorCluster * error);
  EXPORT void mdsplus_float64array_constructor_dims(void **lvFloat64ArrayPtrOut,
						       const LDblArrHdl lvDblArrHdlIn,
						       const LIntArrHdl lvIntArrHdlIn,
						       ErrorCluster * error);
  EXPORT void mdsplus_float64array_destructor(void **lvFloat64ArrayPtr);

/********** INT16 **********/
  EXPORT void mdsplus_int16_constructor(void **lvInt16PtrOut, short valIn, ErrorCluster * error);
  EXPORT void mdsplus_int16_destructor(void **lvInt16Ptr);
  EXPORT void mdsplus_int16_getByte(const void *lvInt16Ptr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_int16_getDouble(const void *lvInt16Ptr, double *doubleOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_int16_getFloat(const void *lvInt16Ptr, float *floatOut,
					ErrorCluster * error);
  EXPORT void mdsplus_int16_getInt(const void *lvInt16Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_int16_getLong(const void *lvInt16Ptr, int64_t * longOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_int16_getShort(const void *lvInt16Ptr, short *shortOut,
					ErrorCluster * error);

/********** INT16ARRAY **********/
  EXPORT void mdsplus_int16array_constructor(void **lvInt16ArrayPtrOut,
						const LShtArrHdl lvShtArrHdlIn,
						ErrorCluster * error);
  EXPORT void mdsplus_int16array_constructor_dims(void **lvInt16ArrayPtrOut,
						     const LShtArrHdl lvShtArrHdlIn,
						     const LIntArrHdl lvIntArrHdlIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_int16array_destructor(void **lvInt16ArrayPtr);

/********** INT32 **********/
  EXPORT void mdsplus_int32_constructor(void **lvInt32PtrOut, int valIn, ErrorCluster * error);
  EXPORT void mdsplus_int32_destructor(void **lvInt32Ptr);
  EXPORT void mdsplus_int32_getByte(const void *lvInt32Ptr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_int32_getDouble(const void *lvInt32Ptr, double *doubleOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_int32_getFloat(const void *lvInt32Ptr, float *floatOut,
					ErrorCluster * error);
  EXPORT void mdsplus_int32_getInt(const void *lvInt32Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_int32_getLong(const void *lvInt32Ptr, int64_t * longOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_int32_getShort(const void *lvInt32Ptr, short *shortOut,
					ErrorCluster * error);

/********** INT32ARRAY **********/
  EXPORT void mdsplus_int32array_constructor(void **lvInt32ArrayPtrOut,
						const LIntArrHdl lvIntArrHdlIn,
						ErrorCluster * error);
  EXPORT void mdsplus_int32array_constructor_dims(void **lvInt32ArrayPtrOut,
						     const LIntArrHdl lvIntArrHdlIn,
						     const LIntArrHdl lvIntArrHdlDimIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_int32array_destructor(void **lvInt32ArrayPtr);

/********** INT64 **********/
  EXPORT void mdsplus_int64_constructor(void **lvInt64PtrOut, int64_t valIn,
					   ErrorCluster * error);
  EXPORT void mdsplus_int64_destructor(void **lvInt64Ptr);
  EXPORT void mdsplus_int64_getByte(const void *lvInt64Ptr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_int64_getDouble(const void *lvInt64Ptr, double *doubleOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_int64_getFloat(const void *lvInt64Ptr, float *floatOut,
					ErrorCluster * error);
  EXPORT void mdsplus_int64_getInt(const void *lvInt64Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_int64_getLong(const void *lvInt64Ptr, int64_t * longOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_int64_getShort(const void *lvInt64Ptr, short *shortOut,
					ErrorCluster * error);

/********** INT64ARRAY **********/
  EXPORT void mdsplus_int64array_constructor(void **lvInt64ArrayPtrOut,
						const LLngArrHdl lvLngArrHdlIn,
						ErrorCluster * error);
  EXPORT void mdsplus_int64array_constructor_dims(void **lvInt64ArrayPtrOut,
						     const LLngArrHdl lvLngArrHdlIn,
						     const LIntArrHdl lvIntArrHdlIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_int64array_destructor(void **lvInt64ArrayPtr);

/********** INT8 **********/
  EXPORT void mdsplus_int8_constructor(void **lvInt8PtrOut, char valIn, ErrorCluster * error);
  EXPORT void mdsplus_int8_destructor(void **lvInt8Ptr);
  EXPORT void mdsplus_int8_getByte(const void *lvInt8Ptr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_int8_getDouble(const void *lvInt8Ptr, double *doubleOut,
					ErrorCluster * error);
  EXPORT void mdsplus_int8_getFloat(const void *lvInt8Ptr, float *floatOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_int8_getInt(const void *lvInt8Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_int8_getLong(const void *lvInt8Ptr, int64_t * longOut,
				      ErrorCluster * error);
  EXPORT void mdsplus_int8_getShort(const void *lvInt8Ptr, short *shortOut,
				       ErrorCluster * error);

/********** INT8ARRAY **********/
  EXPORT void mdsplus_int8array_constructor(void **lvInt8ArrayPtrOut,
					       const LByteArrHdl lvByteArrHdlIn,
					       ErrorCluster * error);
  EXPORT void mdsplus_int8array_constructor_dims(void **lvInt8ArrayPtrOut,
						    const LByteArrHdl lvByteArrHdlIn,
						    const LIntArrHdl lvIntArrHdlIn,
						    ErrorCluster * error);
  EXPORT void mdsplus_int8array_destructor(void **lvInt8ArrayPtr);

/********** RANGE **********/
  EXPORT void mdsplus_range_constructor(void **lvRangePtrOut, const void *lvBeginDataPtrIn,
					   const void *lvEndingDataPtrIn,
					   const void *lvDeltaValDataPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_range_destructor(void **lvRangePtr, ErrorCluster * error);
  EXPORT void mdsplus_range_getBegin(const void *lvRangePtr, void **lvDataPtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_range_getEnding(const void *lvRangePtr, void **lvDataPtrOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_range_getDeltaVal(const void *lvRangePtr, void **lvDataPtrOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_range_setBegin(const void *lvRangePtr, const void *lvDataPtrIn,
					ErrorCluster * error);
  EXPORT void mdsplus_range_setEnding(const void *lvRangePtr, const void *lvDataPtrIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_range_setDeltaVal(const void *lvRangePtr, const void *lvDataPtrIn,
					   ErrorCluster * error);

/********** SIGNAL **********/
  EXPORT void mdsplus_signal_constructor(void **lvSignalPtrOut, const void *lvDataPtrIn,
					    const void *lvRawDataPtrIn, const void *lvDim0DataPtrIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_signal_destructor(void **lvSignalPtr, ErrorCluster * error);
  EXPORT void mdsplus_signal_getData(const void *lvSignalPtr, void **lvDataPtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_signal_getRaw(const void *lvSignalPtr, void **lvRawPtrOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_signal_getDim(const void *lvSignalPtr, void **lvDimPtrOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_signal_setData(const void *lvSignalPtr, const void *lvDataPtrIn,
					ErrorCluster * error);
  EXPORT void mdsplus_signal_setRaw(const void *lvSignalPtr, const void *lvRawPtrIn,
				       ErrorCluster * error);
  EXPORT void mdsplus_signal_setDim(const void *lvSignalPtr, const void *lvDimPtrIn,
				       ErrorCluster * error);

/********** REVENT **********/
  EXPORT void mdsplus_revent_constructor(void **lvREventPtrOut, const char *evNameIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_revent_destructor(void **lvREventPtr);
  EXPORT void mdsplus_revent_getData(const void *lvREventPtr, void **lvDataPtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_getName(const void *lvREventPtr, LStrHandle lvStrHdlOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_getRaw(const void *lvREventPtr, LByteArrHdl lvByteArrHdlOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_revent_getTime(const void *lvREventPtr, void **lvUint64PtrOut,
					ErrorCluster * error);
  EXPORT void mdsplus_revent_run(const void *lvREventPtr, ErrorCluster * error);
  EXPORT void mdsplus_revent_setEvent(const char *evNameIn, const void *lvDataPtrIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventAndWait(const char *evNameIn, const void *lvDataPtrIn,
						ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventRaw(const char *evNameIn, LByteArrHdl lvByteArrHdlIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_revent_setEventRawAndWait(const char *evNameIn, LByteArrHdl lvByteArrHdlIn,
						   ErrorCluster * error);

/********** SCALAR **********/
  EXPORT void mdsplus_scalar_constructor(void **lvScalarPtrOut, ErrorCluster * error);
  EXPORT void mdsplus_scalar_destructor(void **lvScalarPtr);
  EXPORT void mdsplus_scalar_getInfo(const void *lvScalarPtr, char *clazzOut, char *dtypeOut,
					short *lengthOut, LIntArrHdl lvIntArrHdlOut,
					ErrorCluster * error);

/********** STRING **********/
  EXPORT void mdsplus_string_constructor(void **lvStringPtrOut, const char *valIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_string_constructor_len(void **lvStringPtrOut,
						const LUByteArrHdl lvUByteArrHdlIn,
						ErrorCluster * error);
  EXPORT void mdsplus_string_destructor(void **lvStringPtr);
  EXPORT void mdsplus_string_equals(const void *lvStringPtr, LVBoolean * equalsOut,
				       const void *lvDataPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_string_getString(const void *lvStringPtr, LStrHandle lvStrHdlOut,
					  ErrorCluster * error);

/********** STRINGARRAY **********/
  EXPORT void mdsplus_stringarray_constructor(void **lvStringArrayPtrOut,
						 const LStrArrHdl lvLStrArrHdlIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_stringarray_constructor_stringLen(void **lvStringArrayPtrOut,
							   const char *dataIn, int nStringsIn,
							   int stringLenIn, ErrorCluster * error);
  EXPORT void mdsplus_stringarray_destructor(void **lvStringArrayPtr);

/********** TREE **********/
  EXPORT void mdsplus_tree_addDevice(const void *lvTreePtr, void **lvTreeNodePtrOut,
					const char *nameIn, const char *typeIn,
					ErrorCluster * error);
  EXPORT void mdsplus_tree_addNode(const void *lvTreePtr, void **lvTreeNodePtrOut,
				      const char *nameIn, const char *usageIn,
				      ErrorCluster * error);
  EXPORT void mdsplus_tree_constructor(void **lvTreePtrOut, const char *nameIn, int shotIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_tree_constructor_mode(void **lvTreePtrOut, const char *nameIn, int shotIn,
					       const char *modeIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_createPulse(const void *lvTreePtr, int shotIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_deletePulse(const void *lvTreePtr, int shotIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_destructor(void **lvTreePtr);
  EXPORT void mdsplus_tree_edit(const void *lvTreePtr, ErrorCluster * error);
  EXPORT void mdsplus_tree_findTags(const void *lvTreePtr, void **lvStringArrayPtrOut,
				       const char *wildIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_getActiveTree(void **lvTreePtrOut, ErrorCluster * error);
  EXPORT void mdsplus_tree_getCurrent(int *currentOut, const char *treeNameIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_tree_getDatafileSize(const void *lvTreePtr, int64_t * sizeOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_tree_getDefault(const void *lvTreePtr, void **lvTreeNodePtrOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_tree_getNode(const void *lvTreePtr, void **lvTreeNodePtrOut,
				      const char *pathIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_hasNode(const void *lvTreePtr, LVBoolean * lvhasNodeOut,
				      const char *pathIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_getNode_string(const void *lvTreePtr, void **lvTreeNodePtrOut,
					     const void *lvStringPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_getNode_treepath(const void *lvTreePtr, void **lvTreeNodePtrOut,
					       const void *lvTreePathPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_getNodeWild(const void *lvTreePtr, void **lvTreeNodeArrayPtrOut,
					  const char *pathIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_getNodeWild_usageMask(const void *lvTreePtr,
						    void **lvTreeNodeArrayPtrOut,
						    const char *pathIn, int usageMaskIn,
						    ErrorCluster * error);
  EXPORT void mdsplus_tree_isModified(const void *lvTreePtr, LVBoolean * lvIsModifiedOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_tree_isOpenForEdit(const void *lvTreePtr, LVBoolean * lvIsOpenForEditOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_tree_isReadOnly(const void *lvTreePtr, LVBoolean * lvIsReadOnlyOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_tree_quit(const void *lvTreePtr, ErrorCluster * error);
  EXPORT void mdsplus_tree_remove(const void *lvTreePtr, const char *nameIn,
				     ErrorCluster * error);
  EXPORT void mdsplus_tree_removeTag(const void *lvTreePtr, const char *tagNameIn,
					ErrorCluster * error);
  EXPORT void mdsplus_tree_setActiveTree(const void *lvTreePtrIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_setCurrent(const char *treeNameIn, int shotIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_setDefault(const void *lvTreePtr, const void *lvTreeNodePtrIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_tree_setTimeContext(const void *lvTreePtr, const void *lvStartDataPtrIn,
					     const void *lvEndDataPtrIn,
					     const void *lvDeltaDataPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_tree_setVersionsInModel(const void *lvTreePtr, LVBoolean * lvVerEnabledIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_tree_setVersionsInPulse(const void *lvTreePtr, LVBoolean * lvVerEnabledIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_tree_setViewDate(const void *lvTreePtr, const char *dateIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_tree_versionsInModelEnabled(const void *lvTreePtr,
						     LVBoolean * lvVerEnabledOut,
						     ErrorCluster * error);
  EXPORT void mdsplus_tree_versionsInPulseEnabled(const void *lvTreePtr,
						     LVBoolean * lvVerEnabledOut,
						     ErrorCluster * error);
  EXPORT void mdsplus_tree_write(const void *lvTreePtr, ErrorCluster * error);

/********** TREENODE **********/
  EXPORT void mdsplus_treenode_addDevice(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					    const char *nameIn, const char *typeIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_addNode(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					  const char *nameIn, const char *usageIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_addTag(const void *lvTreeNodePtr, const char *tagNameIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_treenode_beginSegment(const void *lvTreeNodePtr,
					       const void *lvStartDataPtrIn,
					       const void *lvEndDataPtrIn,
					       const void *lvTimeDataPtrIn,
					       const void *lvArrayPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_beginTimestampedSegment(const void *lvTreeNodePtr,
							  const void *lvArrayPtrIn,
							  ErrorCluster * error);
  EXPORT void mdsplus_treenode_containsVersions(const void *lvTreeNodePtr,
						   LVBoolean * lvContainsVersionsOut,
						   ErrorCluster * error);
  EXPORT void mdsplus_treenode_deleteData(const void *lvTreeNodePtr, ErrorCluster * error);
  EXPORT void mdsplus_treenode_doMethod(const void *lvTreeNodePtr, const char *methodIn,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_destructor(void **lvTreeNodePtr);
  EXPORT void mdsplus_treenode_findTags(const void *lvTreeNodePtr, void **lvStringArrayPtrOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getBrother(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_getChild(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getChildren(const void *lvTreeNodePtr, LPtrArrHdl lvPtrArrHdlOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_treenode_getClass(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getCompressedLength(const void *lvTreeNodePtr,
						      int *compressedLengthOut,
						      ErrorCluster * error);
  EXPORT void mdsplus_treenode_getConglomerateElt(const void *lvTreeNodePtr,
						     int *conglomerateEltOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getConglomerateNodes(const void *lvTreeNodePtr,
						       void **lvTreeNodeArrayPtrOut,
						       ErrorCluster * error);
  EXPORT void mdsplus_treenode_getDepth(const void *lvTreeNodePtr, int *depthOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getData(const void *lvTreeNodePtr, void **lvDataPtrOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_getDescendants(const void *lvTreeNodePtr,
						 LPtrArrHdl lvPtrArrHdlOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getDType(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getFullPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_treenode_getLength(const void *lvTreeNodePtr, int *lengthOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_getMember(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_getMembers(const void *lvTreeNodePtr, LPtrArrHdl lvPtrArrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_getMinPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNid(const void *lvTreeNodePtr, int *nidOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNode(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					  const char *relPathIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNode_string(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
						 const void *lvStringPtrIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNodeName(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					      ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNumChildren(const void *lvTreeNodePtr, int *numChildrenOut,
						 ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNumDescendants(const void *lvTreeNodePtr,
						    int *numDescendantsOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNumElts(const void *lvTreeNodePtr, int *numEltsOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNumMembers(const void *lvTreeNodePtr, int *numMembersOut,
						ErrorCluster * error);
  EXPORT void mdsplus_treenode_getNumSegments(const void *lvTreeNodePtr, int *numSegmentsOut,
						 ErrorCluster * error);
  EXPORT void mdsplus_treenode_getOriginalPartName(const void *lvTreeNodePtr,
						      LStrHandle lvStrHdlOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getOwnerId(const void *lvTreeNodePtr, int *ownerIdOut,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_getParent(const void *lvTreeNodePtr, void **lvTreeNodePtrOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_getPath(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_getSegment(const void *lvTreeNodePtr, void **lvArrayPtrOut,
					     int segIdxIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_getSegmentLimits(const void *lvTreeNodePtr, int segmentIdxIn,
						   void **lvStartDataPtrOut, void **lvEndDataPtrOut,
						   ErrorCluster * error);
  EXPORT void mdsplus_treenode_getStatus(const void *lvTreeNodePtr, int *statusOut,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_getTimeInserted(const void *lvTreeNodePtr,
						  int64_t * timeInsertedOut, LStrHandle lvStrHdlOut,
						  ErrorCluster * error);
  EXPORT void mdsplus_treenode_getTree(const void *lvTreeNodePtr, void **lvTreePtrOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_getUsage(const void *lvTreeNodePtr, LStrHandle lvStrHdlOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_isChild(const void *lvTreeNodePtr, LVBoolean * lvIsChildOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_isCompressOnPut(const void *lvTreeNodePtr,
						  LVBoolean * lvIsCompressOnPutOut,
						  ErrorCluster * error);
  EXPORT void mdsplus_treenode_isEssential(const void *lvTreeNodePtr,
					      LVBoolean * lvIsEssentialOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_isIncludeInPulse(const void *lvTreeNodePtr,
						LVBoolean * lvisIncludeInPulseOut,
						ErrorCluster * error);
  EXPORT void mdsplus_treenode_isIncludedInPulse(const void *lvTreeNodePtr,
						 LVBoolean * lvIsIncludedInPulseOut,
						 ErrorCluster * error);
  EXPORT void mdsplus_treenode_isMember(const void *lvTreeNodePtr, LVBoolean * lvIsMemberOut,
					   ErrorCluster * error);
  EXPORT void mdsplus_treenode_isNoWriteModel(const void *lvTreeNodePtr,
						 LVBoolean * lvIsNoWriteModelOut,
						 ErrorCluster * error);
  EXPORT void mdsplus_treenode_isNoWriteShot(const void *lvTreeNodePtr,
						LVBoolean * lvIsNoWriteShotOut,
						ErrorCluster * error);
  EXPORT void mdsplus_treenode_isOn(const void *lvTreeNodePtr, LVBoolean * lvIsOnOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_treenode_isSetup(const void *lvTreeNodePtr, LVBoolean * lvIsSetupOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_isWriteOnce(const void *lvTreeNodePtr,
					      LVBoolean * lvIsWriteOnceOut, ErrorCluster * error);
  EXPORT void mdsplus_treenode_makeSegment(const void *lvTreeNodePtr,
					      const void *lvStartDataPtrIn,
					      const void *lvEndDataPtrIn,
					      const void *lvTimeDataPtrIn, const void *lvArrayPtrIn,
					      ErrorCluster * error);
  EXPORT void mdsplus_treenode_makeTimestampedSegment(const void *lvTreeNodePtr,
							 const void *lvArrayPtrIn,
							 const LLngArrHdl lvLngArrHdlIn,
							 ErrorCluster * error);
  EXPORT void mdsplus_treenode_move(const void *lvTreeNodePtr, const void *lvTreeNodePtrIn,
				       ErrorCluster * error);
  EXPORT void mdsplus_treenode_move_newName(const void *lvTreeNodePtr,
					       const void *lvTreeNodePtrIn, const char *newNameIn,
					       ErrorCluster * error);
  EXPORT void mdsplus_treenode_putData(const void *lvTreeNodePtr, const void *lvDataPtrIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_putRow(const void *lvTreeNodePtr, const void *lvDataPtrIn,
					 int64_t * timeIn, int sizeIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_putSegment(const void *lvTreeNodePtr, const void *lvArrayPtrIn,
					     int ofsIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_putTimestampedSegment(const void *lvTreeNodePtr,
							const void *lvArrayPtrIn,
							const LLngArrHdl lvLngArrHdlIn,
							ErrorCluster * error);
  EXPORT void mdsplus_treenode_remove(const void *lvTreeNodePtr, const char *nameIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_treenode_removeTag(const void *lvTreeNodePtr, const char *tagNameIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_treenode_rename(const void *lvTreeNodePtr, const char *newNameIn,
					 ErrorCluster * error);
  EXPORT void mdsplus_treenode_setCompressOnPut(const void *lvTreeNodePtr,
						   LVBoolean * lvCompressOnPutIn,
						   ErrorCluster * error);
  EXPORT void mdsplus_treenode_setEssential(const void *lvTreeNodePtr, LVBoolean * lvEssentialIn,
					       ErrorCluster * error);
  EXPORT void mdsplus_treenode_setIncludeInPulse(const void *lvTreeNodePtr,
						     LVBoolean * lvIncludeInPulseIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_treenode_setIncludedInPulse(const void *lvTreeNodePtr,
						     LVBoolean * lvIncludedInPulseIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_treenode_setNoWriteModel(const void *lvTreeNodePtr,
						  LVBoolean * lvSetNoWriteModelIn,
						  ErrorCluster * error);
  EXPORT void mdsplus_treenode_setNoWriteShot(const void *lvTreeNodePtr,
						 LVBoolean * lvNoWriteShotIn, ErrorCluster * error);
  EXPORT void mdsplus_treenode_setOn(const void *lvTreeNodePtr, LVBoolean * lvOnIn,
					ErrorCluster * error);
  EXPORT void mdsplus_treenode_setSubTree(const void *lvTreeNodePtr, LVBoolean * lvSubTreeIn,
					     ErrorCluster * error);
  EXPORT void mdsplus_treenode_setTree(const void *lvTreeNodePtr, const void *lvTreePtrIn,
					  ErrorCluster * error);
  EXPORT void mdsplus_treenode_setWriteOnce(const void *lvTreeNodePtr, LVBoolean * lvWriteOnceIn,
					       ErrorCluster * error);
  EXPORT void mdsplus_treenode_updateSegment(const void *lvTreeNodePtr,
						const void *lvStartDataPtrIn,
						const void *lvEndDataPtrIn,
						const void *lvTimeDataPtrIn, ErrorCluster * error);

/********** TREENODEARRAY **********/
  EXPORT void mdsplus_treenodearray_destructor(void **lvTreeNodeArrayPtr);

/********** TREEPATH **********/
  EXPORT void mdsplus_treepath_destructor(void **lvTreePathPtr);

/********** UINT16 **********/
  EXPORT void mdsplus_uint16_constructor(void **lvUint16PtrOut, unsigned short valIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_uint16_destructor(void **lvUint16Ptr);
  EXPORT void mdsplus_uint16_getByte(const void *lvUint16Ptr, char *byteOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint16_getDouble(const void *lvUint16Ptr, double *doubleOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_uint16_getFloat(const void *lvUint16Ptr, float *floatOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_uint16_getInt(const void *lvUint16Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_uint16_getLong(const void *lvUint16Ptr, int64_t * longOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint16_getShort(const void *lvUint16Ptr, short *shortOut,
					 ErrorCluster * error);

/********** UINT16ARRAY **********/
  EXPORT void mdsplus_uint16array_constructor(void **lvUint16ArrayPtrOut,
						 const LUShtArrHdl lvUShtArrHdlIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_uint16array_constructor_dims(void **lvUint16ArrayPtrOut,
						      const LUShtArrHdl lvUShtArrHdlIn,
						      const LIntArrHdl lvIntArrHdlIn,
						      ErrorCluster * error);
  EXPORT void mdsplus_uint16array_destructor(void **lvUint16ArrayPtr);

/********** UINT32 **********/
  EXPORT void mdsplus_uint32_constructor(void **lvUint32PtrOut, unsigned int valIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_uint32_destructor(void **lvUint32Ptr);
  EXPORT void mdsplus_uint32_getByte(const void *lvUint32Ptr, char *byteOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint32_getDouble(const void *lvUint32Ptr, double *doubleOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_uint32_getFloat(const void *lvUint32Ptr, float *floatOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_uint32_getInt(const void *lvUint32Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_uint32_getLong(const void *lvUint32Ptr, int64_t * longOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint32_getShort(const void *lvUint32Ptr, short *shortOut,
					 ErrorCluster * error);

/********** UINT32ARRAY **********/
  EXPORT void mdsplus_uint32array_constructor(void **lvUint32ArrayPtrOut,
						 const LUIntArrHdl lvUIntArrHdlIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_uint32array_constructor_dims(void **lvUint32ArrayPtrOut,
						      const LUIntArrHdl lvUIntArrHdlIn,
						      const LIntArrHdl lvIntArrHdlDimIn,
						      ErrorCluster * error);
  EXPORT void mdsplus_uint32array_destructor(void **lvUint32ArrayPtr);

/********** UINT64 **********/
  EXPORT void mdsplus_uint64_constructor(void **lvUint64PtrOut, uint64_t valIn,
					    ErrorCluster * error);
  EXPORT void mdsplus_uint64_destructor(void **lvUint64Ptr);
  EXPORT void mdsplus_uint64_getByte(const void *lvUint64Ptr, char *byteOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint64_getDate(const void *lvUint64Ptr, LStrHandle lvStrHdlOut,
					ErrorCluster * error);

  EXPORT void mdsplus_uint64_getDouble(const void *lvUint64Ptr, double *doubleOut,
					  ErrorCluster * error);
  EXPORT void mdsplus_uint64_getFloat(const void *lvUint64Ptr, float *floatOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_uint64_getInt(const void *lvUint64Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_uint64_getLong(const void *lvUint64Ptr, int64_t * longOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint64_getShort(const void *lvUint64Ptr, short *shortOut,
					 ErrorCluster * error);

/********** UINT64ARRAY **********/
  EXPORT void mdsplus_uint64array_constructor(void **lvUint64ArrayPtrOut,
						 const LULngArrHdl lvULngArrHdlIn,
						 ErrorCluster * error);
  EXPORT void mdsplus_uint64array_constructor_dims(void **lvUint64ArrayPtrOut,
						      const LULngArrHdl lvULngArrHdlIn,
						      const LIntArrHdl lvIntArrHdlIn,
						      ErrorCluster * error);
  EXPORT void mdsplus_uint64array_destructor(void **lvUint64ArrayPtr);

/********** UINT8 **********/
  EXPORT void mdsplus_uint8_constructor(void **lvUint8PtrOut, unsigned char valIn,
					   ErrorCluster * error);
  EXPORT void mdsplus_uint8_destructor(void **lvUint8Ptr);
  EXPORT void mdsplus_uint8_getByte(const void *lvUint8Ptr, char *byteOut, ErrorCluster * error);
  EXPORT void mdsplus_uint8_getDouble(const void *lvUint8Ptr, double *doubleOut,
					 ErrorCluster * error);
  EXPORT void mdsplus_uint8_getFloat(const void *lvUint8Ptr, float *floatOut,
					ErrorCluster * error);
  EXPORT void mdsplus_uint8_getInt(const void *lvUint8Ptr, int *intOut, ErrorCluster * error);
  EXPORT void mdsplus_uint8_getLong(const void *lvUint8Ptr, int64_t * longOut,
				       ErrorCluster * error);
  EXPORT void mdsplus_uint8_getShort(const void *lvUint8Ptr, short *shortOut,
					ErrorCluster * error);

/********** UINT8ARRAY **********/
  EXPORT void mdsplus_uint8array_constructor(void **lvUint8ArrayPtrOut,
						const LUByteArrHdl lvUByteArrHdlIn,
						ErrorCluster * error);
  EXPORT void mdsplus_uint8array_constructor_dims(void **lvUint8ArrayPtrOut,
						     const LUByteArrHdl lvUByteArrHdlIn,
						     const LIntArrHdl lvIntArrHdlIn,
						     ErrorCluster * error);
  EXPORT void mdsplus_uint8array_deserialize(const void *lvUint8ArrayPtr, void **lvDataPtrOut,
						ErrorCluster * error);
  EXPORT void mdsplus_uint8array_destructor(void **lvUint8ArrayPtr);

/********  CONNECTION ***********/
  EXPORT void mdsplus_connection_constructor(void **lvConnectionPtrOut, const char *ipPortIn,
						ErrorCluster * error);
  EXPORT void mdsplus_connection_destructor(void **lvConnectionPtr);
  EXPORT void mdsplus_connection_getData(const void *lvConnectionPtr, void **lvDataPtrOut,
					    const char *expressionIn, ErrorCluster * error);
  EXPORT void mdsplus_connection_putData(const void *lvConnectionPtr, const void *lvDataPtrIn,
					    const char *pathIn, ErrorCluster * error);
  EXPORT void mdsplus_connection_openTree(const void *lvConnectionPtr, const char *tree,
					     int shot, ErrorCluster * error);
  EXPORT void mdsplus_connection_closeTree(const void *lvConnectionPtr, ErrorCluster * error);
  EXPORT void mdsplus_connection_getNode(const void *lvConnectionPtr, void **lvTreeNodePtrOut,
				      const char *pathIn, ErrorCluster * error);

#ifdef __cplusplus
}
#endif
#endif				/* MDSOBJECTSWRP_H */
