/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <string.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <libroutines.h>
#include <mds_stdarg.h>
//#include "mdsdata_Data.h"
//#include "mdstree_MdsTree.h"
#include <treeshr.h>
#include <ncidef.h>
#include <usagedef.h>

extern int TdiData(), TdiDecompile(), TdiCompile();
extern int CvtConvertFloat();
extern int TreeBeginSegment(int nid, struct descriptor *start, struct descriptor *end,
			    struct descriptor *dim, struct descriptor_a *data, int idx);
extern int TreePutRow(int nid, int bufsize, int64_t * timestamp, struct descriptor_a *data);

static jobject DescripToObject(JNIEnv * env, struct descriptor *desc);

static void freeDescrip(struct descriptor *desc);

static JavaVM *jvm;
static jobject localDataProviderInstance;
static JNIEnv *getJNIEnv()
{
  JNIEnv *jEnv;
  int retVal;

  retVal = (*jvm)->AttachCurrentThread(jvm, (void **)&jEnv, NULL);
  if (retVal)
    printf("AttachCurrentThread error %d\n", retVal);
  return jEnv;
}

static void releaseJNIEnv()
{
  (*jvm)->DetachCurrentThread(jvm);
}

//Debug functions
static void printDecompiled(struct descriptor *inD)
{
  int status;
  EMPTYXD(out_xd);
  char *buf;

  status = TdiDecompile(inD, &out_xd MDS_END_ARG);
  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  buf = malloc(out_xd.pointer->length + 1);
  memcpy(buf, out_xd.pointer->pointer, out_xd.pointer->length);
  buf[out_xd.pointer->length] = 0;
  out_xd.pointer->pointer[out_xd.pointer->length - 1] = 0;
  printf("%s\n", buf);
  free(buf);
  MdsFree1Dx(&out_xd, 0);
}

static jobject DescripToObject(JNIEnv * env, struct descriptor *desc)
{
  jclass cls, data_cls;
  jmethodID constr;
  int i, length, count, status;
  unsigned int opcode;
  jboolean is_unsigned = 0;
  jobject obj, exc, curr_obj;
  jvalue args[4];
  jbyteArray jbytes;
  jshortArray jshorts;
  jintArray jints;
  jlongArray jlongs;
  jfloatArray jfloats;
  jdoubleArray jdoubles;
  jobjectArray jobjects;
  jfieldID data_fid, opcode_fid;
  float *float_buf;
  double *double_buf;

  char message[64];
  struct descriptor_a *array_d;
  struct descriptor_r *record_d;
  char *buf;
  EMPTYXD(float_xd);
  EMPTYXD(ca_xd);
  int is_ca = 0;

  if (!desc) {
    return NULL;
  }
//printf("DescripToObject dtype = %d class = %d\n", desc->dtype, desc->class);

  if (desc->class == CLASS_XD)
    return DescripToObject(env, ((struct descriptor_xd *)desc)->pointer);
  switch (desc->class) {
  case CLASS_S:
    switch (desc->dtype) {
    case DTYPE_BU:
      is_unsigned = 1;
    case DTYPE_B:
      cls = (*env)->FindClass(env, "mdsdata/ByteData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(BZ)Lmdsdata/Data;");
      args[0].b = *(char *)desc->pointer;
      args[1].z = is_unsigned;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_WU:
      is_unsigned = 1;
    case DTYPE_W:
      cls = (*env)->FindClass(env, "mdsdata/ShortData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(SZ)Lmdsdata/Data;");
      args[0].s = *(short *)desc->pointer;
      args[1].z = is_unsigned;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_LU:
      is_unsigned = 1;
    case DTYPE_L:
      cls = (*env)->FindClass(env, "mdsdata/IntData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(IZ)Lmdsdata/Data;");
      args[0].i = *(int *)desc->pointer;
      args[1].z = is_unsigned;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_NID:
      cls = (*env)->FindClass(env, "mdsdata/NidData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(I)Lmdsdata/Data;");
      args[0].i = *(int *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_QU:
      is_unsigned = 1;
    case DTYPE_Q:
      cls = (*env)->FindClass(env, "mdsdata/LongData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(JZ)Lmdsdata/Data;");
      args[0].j = *(long *)desc->pointer;
      args[1].z = is_unsigned;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_OU:
      is_unsigned = 1;
    case DTYPE_O:
      cls = (*env)->FindClass(env, "mdsdata/LongArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)Lmdsdata/Data;");
      jlongs = (*env)->NewLongArray(env, 2);
      (*env)->SetLongArrayRegion(env, jlongs, 0, 2, (jlong *) desc->pointer);
      args[0].l = jlongs;
      args[1].z = is_unsigned;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_FS:
    case DTYPE_F:
    case DTYPE_FSC:
      cls = (*env)->FindClass(env, "mdsdata/FloatData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(F)Lmdsdata/Data;");
      if (desc->dtype != DTYPE_FS)
	CvtConvertFloat(desc->pointer, desc->dtype, &args[0].f, DTYPE_FS, 0);
      else
	args[0].f = *(float *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_FTC:
    case DTYPE_FT:
    case DTYPE_D:
    case DTYPE_G:
      cls = (*env)->FindClass(env, "mdsdata/DoubleData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(D)Lmdsdata/Data;");
      CvtConvertFloat(desc->pointer, desc->dtype, &args[0].d, DTYPE_DOUBLE, 0);
      args[0].d = *(double *)desc->pointer;
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_T:
      cls = (*env)->FindClass(env, "mdsdata/StringData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)Lmdsdata/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_IDENT:
      cls = (*env)->FindClass(env, "mdsdata/IdentData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)Lmdsdata/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_PATH:
      cls = (*env)->FindClass(env, "mdsdata/PathData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)Lmdsdata/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_EVENT:
      cls = (*env)->FindClass(env, "mdsdata/EventData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "(Ljava/lang/String;)Lmdsdata/Data;");
      buf = (char *)malloc(desc->length + 1);
      memcpy(buf, desc->pointer, desc->length);
      buf[desc->length] = 0;
      args[0].l = (*env)->NewStringUTF(env, buf);
      free(buf);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_MISSING:
      cls = (*env)->FindClass(env, "mdsdata/IntArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([I)Lmdsdata/Data;");
      jints = (*env)->NewIntArray(env, 0);
      args[0].l = jints;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    default:
      sprintf(message, "Datatype %d not supported for class CLASS_S", desc->dtype);
      exc = (*env)->FindClass(env, "mdsdata/DataException");
      (*env)->ThrowNew(env, exc, message);
    }
  case CLASS_CA:
    status = TdiData(desc, &ca_xd MDS_END_ARG);
    if (!(status & 1)) {
      printf("Cannot evaluate CA descriptor\n");
      return NULL;
    }
    is_ca = 1;
  case CLASS_A:
    if (is_ca)
      array_d = (struct descriptor_a *)ca_xd.pointer;
    else
      array_d = (struct descriptor_a *)desc;
    length = array_d->arsize / array_d->length;
    switch (array_d->dtype) {
    case DTYPE_MISSING:
      cls = (*env)->FindClass(env, "mdsdata/IntArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([I)Lmdsdata/Data;");
      jints = (*env)->NewIntArray(env, 0);
      args[0].l = jints;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_BU:
      is_unsigned = 1;
    case DTYPE_B:
      cls = (*env)->FindClass(env, "mdsdata/ByteArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([BZ)Lmdsdata/Data;");
      jbytes = (*env)->NewByteArray(env, length);
      (*env)->SetByteArrayRegion(env, jbytes, 0, length, (jbyte *) array_d->pointer);
      args[0].l = jbytes;
      args[1].z = is_unsigned;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_WU:
      is_unsigned = 1;
    case DTYPE_W:
      cls = (*env)->FindClass(env, "mdsdata/ShortArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([SZ)Lmdsdata/Data;");
      jshorts = (*env)->NewShortArray(env, length);
      (*env)->SetShortArrayRegion(env, jshorts, 0, length, (jshort *) array_d->pointer);
      args[0].l = jshorts;
      args[1].z = is_unsigned;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_LU:
      is_unsigned = 1;
    case DTYPE_L:
      cls = (*env)->FindClass(env, "mdsdata/IntArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([IZ)Lmdsdata/Data;");
      jints = (*env)->NewIntArray(env, length);
      (*env)->SetIntArrayRegion(env, jints, 0, length, (jint *) array_d->pointer);
      args[0].l = jints;
      args[1].z = is_unsigned;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_QU:
      is_unsigned = 1;
    case DTYPE_Q:
      cls = (*env)->FindClass(env, "mdsdata/LongArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)Lmdsdata/Data;");
      jlongs = (*env)->NewLongArray(env, length);
      (*env)->SetLongArrayRegion(env, jlongs, 0, length, (jlong *) array_d->pointer);
      args[0].l = jlongs;
      args[1].z = is_unsigned;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_OU:
      is_unsigned = 1;
    case DTYPE_O:
      cls = (*env)->FindClass(env, "mdsdata/LongArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([JZ)Lmdsdata/Data;");
      jlongs = (*env)->NewLongArray(env, 2 * length);
      (*env)->SetLongArrayRegion(env, jlongs, 0, 2 * length, (jlong *) array_d->pointer);
      args[0].l = jlongs;
      args[1].z = is_unsigned;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_FS:
    case DTYPE_F:
    case DTYPE_FSC:
      float_buf = malloc(sizeof(float) * length);
      if (array_d->dtype == DTYPE_FS)
	memcpy(float_buf, array_d->pointer, sizeof(float) * length);
      else
	for (i = 0; i < length; i++)
	  CvtConvertFloat(&((float *)array_d->pointer)[i], desc->dtype, &float_buf[i], DTYPE_FS, 0);
      cls = (*env)->FindClass(env, "mdsdata/FloatArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([F)Lmdsdata/Data;");
      jfloats = (*env)->NewFloatArray(env, length);
      (*env)->SetFloatArrayRegion(env, jfloats, 0, length, (jfloat *) float_buf);
      args[0].l = jfloats;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);

    case DTYPE_FTC:
    case DTYPE_FT:
    case DTYPE_D:
    case DTYPE_G:
      double_buf = malloc(sizeof(double) * length);
      for (i = 0; i < length; i++)
	CvtConvertFloat(&((double *)array_d->pointer)[i], desc->dtype, &double_buf[i], DTYPE_DOUBLE,
			0);
      cls = (*env)->FindClass(env, "mdsdata/DoubleArray");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "([D)Lmdsdata/Data;");
      jdoubles = (*env)->NewDoubleArray(env, length);
      (*env)->SetDoubleArrayRegion(env, jdoubles, 0, length, (jdouble *) double_buf);
      args[0].l = jdoubles;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    case DTYPE_T:
      cls = (*env)->FindClass(env, "mdsdata/StringArray");
      constr =
	  (*env)->GetStaticMethodID(env, cls, "getData", "([Ljava/lang/String;)Lmdsdata/Data;");
      data_cls = (*env)->FindClass(env, "java/lang/String");
      jobjects = (*env)->NewObjectArray(env, length, data_cls, 0);
      buf = malloc(array_d->length + 1);
      buf[array_d->length] = 0;
      for (i = 0; i < length; i++) {
	memcpy(buf, &array_d->pointer[i * array_d->length], array_d->length);
	(*env)->SetObjectArrayElement(env, jobjects, i, (jobject) (*env)->NewStringUTF(env, buf));
      }
      free(buf);
      args[0].l = jobjects;
      if (is_ca)
	MdsFree1Dx(&ca_xd, 0);
      return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    }
  case CLASS_R:
    record_d = (struct descriptor_r *)desc;
    switch (record_d->dtype) {
    case DTYPE_PARAM:
      cls = (*env)->FindClass(env, "mdsdata/ParamData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_SIGNAL:
      cls = (*env)->FindClass(env, "mdsdata/SignalData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_DIMENSION:
      cls = (*env)->FindClass(env, "mdsdata/DimensionData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_WINDOW:
      cls = (*env)->FindClass(env, "mdsdata/WindowData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_FUNCTION:
      cls = (*env)->FindClass(env, "mdsdata/FunctionData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_CONGLOM:
      cls = (*env)->FindClass(env, "mdsdata/ConglomData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_ACTION:
      cls = (*env)->FindClass(env, "mdsdata/ActionData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_DISPATCH:
      cls = (*env)->FindClass(env, "mdsdata/DispatchData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_PROGRAM:
      cls = (*env)->FindClass(env, "mdsdata/ProgramData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_ROUTINE:
      cls = (*env)->FindClass(env, "mdsdata/RoutineData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_PROCEDURE:
      cls = (*env)->FindClass(env, "mdsdata/ProcedureData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_METHOD:
      cls = (*env)->FindClass(env, "mdsdata/MethodData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_DEPENDENCY:
      cls = (*env)->FindClass(env, "mdsdata/DependencyData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_CONDITION:
      cls = (*env)->FindClass(env, "mdsdata/ConditionData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_WITH_UNITS:
      cls = (*env)->FindClass(env, "mdsdata/WithUnitsData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_CALL:
      cls = (*env)->FindClass(env, "mdsdata/CallData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_WITH_ERROR:
      cls = (*env)->FindClass(env, "mdsdata/WithErrorData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_RANGE:
      cls = (*env)->FindClass(env, "mdsdata/RangeData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    case DTYPE_SLOPE:
      cls = (*env)->FindClass(env, "mdsdata/SlopeData");
      constr = (*env)->GetStaticMethodID(env, cls, "getData", "()Lmdsdata/Data;");
      break;
    }
    obj = (*env)->CallStaticObjectMethodA(env, cls, constr, args);
    data_cls = (*env)->FindClass(env, "mdsdata/Data");
    jobjects = (*env)->NewObjectArray(env, record_d->ndesc, data_cls, 0);
    for (i = count = 0; count < record_d->ndesc; i++, count++) {
      (*env)->SetObjectArrayElement(env, jobjects, i, DescripToObject(env, record_d->dscptrs[i]));
    }
    data_fid = (*env)->GetFieldID(env, cls, "descs", "[Lmdsdata/Data;");
    (*env)->SetObjectField(env, obj, data_fid, jobjects);
    if (record_d->pointer
	&& (record_d->dtype == DTYPE_FUNCTION || record_d->dtype == DTYPE_DEPENDENCY
	    || record_d->dtype == DTYPE_CONDITION || record_d->dtype == DTYPE_CALL
	    || record_d->dtype == DTYPE_DISPATCH)) {
      opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
      opcode = 0;
      switch (record_d->length) {
      case 1:
	opcode = *(unsigned char *)record_d->pointer;
	break;
      case 2:
	opcode = *(unsigned short *)record_d->pointer;
	break;
      default:
	opcode = *(unsigned int *)record_d->pointer;
	break;
      }
      (*env)->SetIntField(env, obj, opcode_fid, opcode);
    }
    return obj;
  case CLASS_APD:
    array_d = (struct descriptor_a *)desc;
    length = array_d->arsize / array_d->length;
    cls = (*env)->FindClass(env, "mdsdata/ApdData");
    data_cls = (*env)->FindClass(env, "mdsdata/Data");
    constr = (*env)->GetStaticMethodID(env, cls, "getData", "([Lmdsdata/Data;)Lmdsdata/Data;");
    jobjects = (*env)->NewObjectArray(env, length, data_cls, 0);
    for (i = 0; i < length; i++) {
      if (curr_obj = DescripToObject(env, ((struct descriptor **)array_d->pointer)[i]))
	(*env)->SetObjectArrayElement(env, jobjects, i, curr_obj);
    }
    args[0].l = jobjects;
    return (*env)->CallStaticObjectMethodA(env, cls, constr, args);
  }
  return 0;
}

static struct descriptor *ObjectToDescrip(JNIEnv * env, jobject obj)
{
  jclass cls;
  jfieldID datum_fid, descs_fid, opcode_fid, dtype_fid, dclass_fid;
  static DESCRIPTOR_A(template_array, 0, 0, 0, 0);
  struct descriptor_a *array_d;
  static DESCRIPTOR_R(template_rec, 0, 1);
  struct descriptor_r *record_d;
  int i, ndescs, opcode, dtype, dclass;
  jobject jdescs;
  jsize length;
  jbyteArray jbytes;
  jshortArray jshorts;
  jintArray jints;
  jlongArray jlongs;
  jfloatArray jfloats;
  jdoubleArray jdoubles;
  jobjectArray jobjects;
  jbyte *bytes;
  jshort *shorts;
  jint *ints;
  jlong *longs;
  jfloat *floats;
  jdouble *doubles;
  jstring *jstrings;
  char **strings;
  int maxlen;
  jstring java_string;
  const char *string;
  struct descriptor *desc;

  if (!obj) {
    return NULL;
  }
  cls = (*env)->GetObjectClass(env, obj);
  dtype_fid = (*env)->GetFieldID(env, cls, "dtype", "I"),
      dclass_fid = (*env)->GetFieldID(env, cls, "clazz", "I");
  dtype = (*env)->GetIntField(env, obj, dtype_fid),
      dclass = (*env)->GetIntField(env, obj, dclass_fid);
  switch (dclass) {
  case CLASS_S:
    desc = (struct descriptor *)malloc(sizeof(struct descriptor));
    desc->class = dclass;
    desc->dtype = dtype;
    switch (dtype) {
    case DTYPE_B:
    case DTYPE_BU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "B");
      desc->length = sizeof(char);
      desc->pointer = (char *)malloc(desc->length);
      *desc->pointer = (*env)->GetByteField(env, obj, datum_fid);
      return desc;
    case DTYPE_W:
    case DTYPE_WU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "S");
      desc->length = sizeof(short);
      desc->pointer = (char *)malloc(desc->length);
      *(short *)desc->pointer = (*env)->GetShortField(env, obj, datum_fid);
      return desc;
    case DTYPE_L:
    case DTYPE_LU:
    case DTYPE_NID:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "I");
      desc->length = sizeof(int);
      desc->pointer = (char *)malloc(desc->length);
      *(int *)desc->pointer = (*env)->GetIntField(env, obj, datum_fid);
      return desc;
    case DTYPE_Q:
    case DTYPE_QU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "J");
      desc->length = sizeof(int64_t);
      desc->pointer = (char *)malloc(desc->length);
      *(int64_t *) desc->pointer = (*env)->GetLongField(env, obj, datum_fid);
      return desc;
    case DTYPE_O:
    case DTYPE_OU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      desc->length = 2 * sizeof(int64_t);
      desc->pointer = (char *)malloc(desc->length);
      *(int64_t *) desc->pointer = longs[0];
      *((int64_t *) desc->pointer + 1) = longs[1];
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return desc;
    case DTYPE_T:
    case DTYPE_PATH:
    case DTYPE_EVENT:
    case DTYPE_IDENT:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "Ljava/lang/String;");
      java_string = (*env)->GetObjectField(env, obj, datum_fid);
      string = (*env)->GetStringUTFChars(env, java_string, 0);
      desc->length = strlen(string);
      if (desc->length > 0) {
	desc->pointer = (char *)malloc(desc->length);
	memcpy(desc->pointer, string, desc->length);
      } else
	desc->pointer = 0;
      (*env)->ReleaseStringUTFChars(env, java_string, string);
      return desc;
    case DTYPE_FLOAT:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "F");
      desc->length = sizeof(float);
      desc->pointer = (char *)malloc(desc->length);
      *(float *)desc->pointer = (*env)->GetFloatField(env, obj, datum_fid);
      //flags_fid = (*env)->GetFieldID(env, cls, "flags", "I");
      //desc->dtype = (unsigned char)(*env)->GetIntField(env, obj, flags_fid);
      desc->dtype = DTYPE_FLOAT;
      CvtConvertFloat(desc->pointer, DTYPE_FS, desc->pointer, desc->dtype, 0);
      return desc;
    case DTYPE_DOUBLE:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "D");
      desc->length = sizeof(double);
      desc->pointer = (char *)malloc(desc->length);
      *(double *)desc->pointer = (*env)->GetDoubleField(env, obj, datum_fid);
      //flags_fid = (*env)->GetFieldID(env, cls, "flags", "I");
      //desc->dtype = (unsigned char)(*env)->GetIntField(env, obj, flags_fid);
      desc->dtype = DTYPE_DOUBLE;
      CvtConvertFloat(desc->pointer, DTYPE_DOUBLE, desc->pointer, desc->dtype, 0);
      return desc;
    default:
      printf("\nUnsupported type for CLASS_S: %d\n", dtype);
    }
    break;
  case CLASS_A:
    array_d = (struct descriptor_a *)malloc(sizeof(struct descriptor_a));
    memcpy(array_d, &template_array, sizeof(struct descriptor_a));
    array_d->dtype = dtype;
    switch (dtype) {
    case DTYPE_B:
    case DTYPE_BU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[B");
      jbytes = (*env)->GetObjectField(env, obj, datum_fid);
      bytes = (*env)->GetByteArrayElements(env, jbytes, 0);
      length = (*env)->GetArrayLength(env, jbytes);
      array_d->length = sizeof(char);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, bytes, array_d->arsize);
      (*env)->ReleaseByteArrayElements(env, jbytes, bytes, 0);
      return (struct descriptor *)array_d;
    case DTYPE_W:
    case DTYPE_WU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[S");
      jshorts = (*env)->GetObjectField(env, obj, datum_fid);
      shorts = (*env)->GetShortArrayElements(env, jshorts, 0);
      length = (*env)->GetArrayLength(env, jshorts);
      array_d->length = sizeof(short);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, shorts, array_d->arsize);
      (*env)->ReleaseShortArrayElements(env, jshorts, shorts, 0);
      return (struct descriptor *)array_d;
    case DTYPE_L:
    case DTYPE_LU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[I");
      jints = (*env)->GetObjectField(env, obj, datum_fid);
      ints = (*env)->GetIntArrayElements(env, jints, 0);
      length = (*env)->GetArrayLength(env, jints);
      array_d->length = sizeof(int);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, ints, array_d->arsize);
      (*env)->ReleaseIntArrayElements(env, jints, ints, 0);
      return (struct descriptor *)array_d;
    case DTYPE_Q:
    case DTYPE_QU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      length = (*env)->GetArrayLength(env, jlongs);
      array_d->length = sizeof(int64_t);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, longs, array_d->arsize);
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return (struct descriptor *)array_d;
    case DTYPE_O:
    case DTYPE_OU:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[J");
      jlongs = (*env)->GetObjectField(env, obj, datum_fid);
      longs = (*env)->GetLongArrayElements(env, jlongs, 0);
      length = (*env)->GetArrayLength(env, jlongs);
      array_d->length = 2 * sizeof(int64_t);
      array_d->arsize = array_d->length * length / 2;
      array_d->pointer = (char *)malloc(array_d->arsize);
      memcpy(array_d->pointer, longs, array_d->arsize);
      (*env)->ReleaseLongArrayElements(env, jlongs, longs, 0);
      return (struct descriptor *)array_d;
    case DTYPE_FLOAT:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[F");
      jfloats = (*env)->GetObjectField(env, obj, datum_fid);
      floats = (*env)->GetFloatArrayElements(env, jfloats, 0);
      length = (*env)->GetArrayLength(env, jfloats);
      array_d->length = sizeof(float);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      //flags_fid = (*env)->GetFieldID(env, cls, "flags", "I");
      //array_d->dtype = (unsigned char)(*env)->GetIntField(env, obj, flags_fid);
      array_d->dtype = DTYPE_FLOAT;
      for (i = 0; i < length; i++)
	CvtConvertFloat(&floats[i], DTYPE_FS, &((float *)array_d->pointer)[i], array_d->dtype, 0);
      (*env)->ReleaseFloatArrayElements(env, jfloats, floats, 0);
      return (struct descriptor *)array_d;
    case DTYPE_DOUBLE:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[D");
      jdoubles = (*env)->GetObjectField(env, obj, datum_fid);
      doubles = (*env)->GetDoubleArrayElements(env, jdoubles, 0);
      length = (*env)->GetArrayLength(env, jdoubles);
      array_d->length = sizeof(double);
      array_d->arsize = array_d->length * length;
      array_d->pointer = (char *)malloc(array_d->arsize);
      //flags_fid = (*env)->GetFieldID(env, cls, "flags", "I");
      //array_d->dtype = (unsigned char)(*env)->GetIntField(env, obj, flags_fid);
      array_d->dtype = DTYPE_DOUBLE;
      for (i = 0; i < length; i++)
	CvtConvertFloat(&doubles[i], DTYPE_DOUBLE, &((double *)array_d->pointer)[i], array_d->dtype,
			0);
      (*env)->ReleaseDoubleArrayElements(env, jdoubles, doubles, 0);
      return (struct descriptor *)array_d;
    case DTYPE_T:
      datum_fid = (*env)->GetFieldID(env, cls, "data", "[Ljava/lang/String;");
      jobjects = (*env)->GetObjectField(env, obj, datum_fid);
      length = (*env)->GetArrayLength(env, jobjects);
      strings = malloc(length * sizeof(char *));
      jstrings = malloc(length * sizeof(jstring));
      maxlen = 0;
      for (i = 0; i < length; i++) {
	jstrings[i] = (*env)->GetObjectArrayElement(env, jobjects, i);
	strings[i] = (char *)(*env)->GetStringUTFChars(env, jstrings[i], 0);
	if (maxlen < (int)strlen(strings[i]))
	  maxlen = (int)strlen(strings[i]);
      }
      array_d->pointer = (char *)malloc(length * maxlen);
      memset(array_d->pointer, ' ', length * maxlen);
      for (i = 0; i < length; i++) {
	memcpy(&array_d->pointer[i * maxlen], strings[i], maxlen);
	(*env)->ReleaseStringUTFChars(env, jstrings[i], strings[i]);
      }
      array_d->length = maxlen;
      array_d->arsize = array_d->length * length;
      return (struct descriptor *)array_d;
    default:
      printf("\nUnsupported type for CLASS_A: %d\n", dtype);
      break;
    }
  case CLASS_R:
    opcode_fid = (*env)->GetFieldID(env, cls, "opcode", "I");
    opcode = (*env)->GetIntField(env, obj, opcode_fid);
    descs_fid = (*env)->GetFieldID(env, cls, "descs", "[Lmdsdata/Data;");
    jdescs = (*env)->GetObjectField(env, obj, descs_fid);
    ndescs = (*env)->GetArrayLength(env, jdescs);
    record_d = (struct descriptor_r *)malloc(sizeof(struct descriptor_r) + ndescs * sizeof(void *));
    memcpy(record_d, &template_rec, sizeof(struct descriptor_r));
    record_d->dtype = dtype;
    record_d->length = 0;
    record_d->pointer = 0;
    if (dtype == DTYPE_FUNCTION || dtype == DTYPE_DEPENDENCY || dtype == DTYPE_CONDITION
	|| dtype == DTYPE_CALL || dtype == DTYPE_DISPATCH) {
      record_d->length = sizeof(short);
      record_d->pointer = (unsigned char *)malloc(sizeof(short));
      *(short *)record_d->pointer = opcode;
    }
    if (dtype == DTYPE_CALL) {
      record_d->length = sizeof(char);
      record_d->pointer = (unsigned char *)malloc(sizeof(char));
      *(char *)record_d->pointer = opcode;
    }
    record_d->ndesc = ndescs;
    for (i = 0; i < ndescs; i++) {
      record_d->dscptrs[i] = ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jdescs, i));
    }
    return (struct descriptor *)record_d;
  case CLASS_APD:

    descs_fid = (*env)->GetFieldID(env, cls, "descs", "[Lmdsdata/Data;");
    jdescs = (*env)->GetObjectField(env, obj, descs_fid);
    ndescs = (*env)->GetArrayLength(env, jdescs);

    array_d = (struct descriptor_a *)malloc(sizeof(struct descriptor_a));
    memcpy(array_d, &template_array, sizeof(struct descriptor_a));
    array_d->dtype = dtype;
    array_d->class = CLASS_APD;
    array_d->length = sizeof(struct descriptor *);
    array_d->arsize = array_d->length * ndescs;
    array_d->pointer = (char *)malloc(sizeof(void *) * ndescs);
    for (i = 0; i < ndescs; i++) {
      ((struct descriptor **)(array_d->pointer))[i] =
	  ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jdescs, i));
    }
    return (struct descriptor *)array_d;
  default:
    printf("\nUnsupported class: %d\n", dclass);
  }
  return 0;
}

static void freeDescrip(struct descriptor *desc)
{
  struct descriptor_r *record_d;
  struct descriptor_a *array_d;
  int i;

  if (!desc)
    return;

/*printf("freeDescrip class %d dtype %d\n", desc->class, desc->dtype);*/

  switch (desc->class) {
  case CLASS_S:
    if (desc->pointer)
      free(desc->pointer);
    break;
  case CLASS_A:
    if (desc->pointer)
      free(((struct descriptor_a *)desc)->pointer);
    break;
  case CLASS_R:
    record_d = (struct descriptor_r *)desc;
    if (record_d->length)
      free(record_d->pointer);
    for (i = 0; i < record_d->ndesc; i++)
      freeDescrip(record_d->dscptrs[i]);
    break;
  case CLASS_APD:
    array_d = (struct descriptor_a *)desc;
    for (i = 0; i < (int)array_d->arsize / array_d->length; i++)
      freeDescrip(((struct descriptor **)array_d->pointer)[i]);
    break;
  }
  free((char *)desc);
}

/*
 * Class:     mdsdata_Data
 * Method:    compileWithArgs
 * Signature: (Ljava/lang/String;[Lmdsdata/Data;)Lmdsdata/Data;
 */
JNIEXPORT jstring JNICALL Java_mdsdata_Data_decompile(JNIEnv * env, jobject obj) {

  EMPTYXD(out_xd);
  jstring ris;
  struct descriptor *dec_d;
  char *buf;
  int status;
  struct descriptor *data_d = ObjectToDescrip(env, obj);

  status = TdiDecompile(data_d, &out_xd MDS_END_ARG);
  if (!(status & 1)) {
    printf("Error decompiling expression: %s\n", MdsGetMsg(status));
    return NULL;
  }
  freeDescrip(data_d);
  dec_d = out_xd.pointer;
  buf = (char *)malloc(dec_d->length + 1);
  memcpy(buf, dec_d->pointer, dec_d->length);
  buf[dec_d->length] = 0;
  MdsFree1Dx(&out_xd, NULL);
  ris = (*env)->NewStringUTF(env, buf);
  free((char *)buf);
  return ris;

}

#define MAX_ARGS 128
/*
 * Class:     mdsdata_Data
 * Method:    evaluate
 * Signature: ()Lmdsdata/Data;
 */

JNIEXPORT jobject JNICALL Java_mdsdata_Data_compileWithArgs
    (JNIEnv * env, jclass cls, jstring jexpr, jobjectArray jargs) {
  EMPTYXD(outXd);
  int argLen, varIdx, i;
  const char *expr = (*env)->GetStringUTFChars(env, jexpr, 0);
  struct descriptor exprD = { 0, DTYPE_T, CLASS_S, 0 };
  jobject ris = 0, exc;
  struct descriptor *arglist[MAX_ARGS];

  exprD.length = strlen(expr);
  exprD.pointer = (char *)expr;

  argLen = (*env)->GetArrayLength(env, jargs);
  if (argLen > MAX_ARGS)
    argLen = MAX_ARGS;
  varIdx = 2;
  arglist[1] = &exprD;
  for (i = 0; i < argLen; i++) {
    arglist[varIdx++] = ObjectToDescrip(env, (*env)->GetObjectArrayElement(env, jargs, i));
  }
  arglist[varIdx++] = (struct descriptor *)&outXd;
  arglist[varIdx++] = MdsEND_ARG;
  *(int *)&arglist[0] = varIdx - 1;
  LibCallg(arglist, TdiCompile);
  for (i = 0; i < argLen; i++)
    freeDescrip(arglist[2 + i]);
  (*env)->ReleaseStringUTFChars(env, jexpr, expr);
  if (outXd.pointer)
    ris = DescripToObject(env, outXd.pointer);
  else {
    exc = (*env)->FindClass(env, "SyntaxException");
    (*env)->ThrowNew(env, exc, "Syntax error");
  }
  MdsFree1Dx(&outXd, 0);
  return ris;
}

/*
 * Class:     mdsdata_Data
 * Method:    evaluate
 * Signature: ()Lmdsdata/Data;
 */
JNIEXPORT jobject JNICALL Java_mdsdata_Data_evaluate(JNIEnv * env, jobject obj) {
  EMPTYXD(xd);
  int status;
  jobject ris = 0;
  struct descriptor *dataD = ObjectToDescrip(env, obj);
  jclass exc;

  status = TdiData(dataD, &xd MDS_END_ARG);
  if (!(status & 1) || !xd.pointer) {
    exc = (*env)->FindClass(env, "mdsdata/DataException");
    (*env)->ThrowNew(env, exc, MdsGetMsg(status));
  }

  ris = DescripToObject(env, xd.pointer);
  MdsFree1Dx(&xd, 0);
  return ris;
}

/////////////////////////////////mdstree stuff////////////////////////
static void RaiseException(JNIEnv * env, char *msg, int status)
{
  char fullMsg[2048];
  jclass exc = (*env)->FindClass(env, "mdstree/TreeException");
  sprintf(fullMsg, "%d:%s", status, msg);
  (*env)->ExceptionClear(env);
  (*env)->ThrowNew(env, exc, fullMsg);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getFullPath
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_mdstree_MdsTree_getPath(JNIEnv * env, jclass cls, jint nid) {
  char *path = TreeGetPath(nid);
  jstring jpath = (*env)->NewStringUTF(env, path);
  TreeFree(path);
  return jpath;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getMsg
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_mdstree_MdsTree_getFullPath(JNIEnv * env, jclass cls, jint nid) {
  char fullPath[512];
  int fullPathLen, status;
  jstring jFullPath;

  struct nci_itm nci_list[] = { {511, NciFULLPATH, fullPath, &fullPathLen},
  {NciEND_OF_LIST, 0, 0, 0}
  };

  status = TreeGetNci(nid, nci_list);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
    return NULL;
  }
  jFullPath = (*env)->NewStringUTF(env, fullPath);
  return jFullPath;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getData
 * Signature: (I)Lmdsdata/Data;
 */
JNIEXPORT jobject JNICALL Java_mdstree_MdsTree_getData(JNIEnv * env, jclass cls, jint nid) {
  EMPTYXD(xd);
  int status;
  jobject retObj;

  status = TreeGetRecord(nid, &xd);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
    return NULL;
  }

  retObj = DescripToObject(env, xd.pointer);
  MdsFree1Dx(&xd, 0);
  return retObj;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    deleteData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_deleteData(JNIEnv * env, jclass cls, jint nid) {
  EMPTYXD(xd);
  int status;

  status = TreePutRecord(nid, (struct descriptor *)&xd, 0);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
  }
}

/*
 * Class:     mdstree_MdsTree
 * Method:    putData
 * Signature: (ILmdsdata/Data;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_putData
    (JNIEnv * env, jclass cls, jint nid, jobject jData) {
  int status;
  struct descriptor *dataD = ObjectToDescrip(env, jData);

  status = TreePutRecord(nid, dataD, 0);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
  }
  freeDescrip(dataD);
}

 /*
  * Class:     mdstree_MdsTree
  * Method:    turnOn
  * Signature: (I)V
  */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_turnOn(JNIEnv * env, jclass cls, jint nid) {
  int status = TreeTurnOn(nid);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
  }
}

/*
 * Class:     mdstree_MdsTree
 * Method:    turnOff
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_turnOff(JNIEnv * env, jclass cls, jint nid) {
  int status = TreeTurnOff(nid);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
  }
}

/*
 * Class:     mdstree_MdsTree
 * Method:    isOn
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_mdstree_MdsTree_isOn(JNIEnv * env, jclass cls, jint nid)
{
  int status = TreeIsOn(nid);
  return (status & 1);
}

static void convertTime(int *time, char *retTime)
{
  char timeStr[512];
  int retLen;
  struct descriptor time_dsc = { 511, DTYPE_T, CLASS_S, timeStr };

  LibSysAscTim(&retLen, &time_dsc, time);
  timeStr[retLen] = 0;
  strcpy(retTime, timeStr);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getInsertionDate
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_mdstree_MdsTree_getInsertionDate(JNIEnv * env, jclass cls, jint nid) {

  int timeInserted[2];
  int timeLen;
  char ascTim[512];
  jstring jAscTim;
  struct nci_itm nciList[] = { {8, NciTIME_INSERTED, timeInserted, &timeLen},
  {NciEND_OF_LIST, 0, 0, 0}
  };
  int status = TreeGetNci(nid, nciList);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);

  convertTime(timeInserted, ascTim);

  jAscTim = (*env)->NewStringUTF(env, ascTim);
  return jAscTim;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    open
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_open
    (JNIEnv * env, jclass cls, jstring jexperiment, jint shot) {

  char *experiment = (char *)(*env)->GetStringUTFChars(env, jexperiment, 0);
  int status = TreeOpen(experiment, shot, 0);
  (*env)->ReleaseStringUTFChars(env, jexperiment, experiment);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    close
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_close
    (JNIEnv * env, jclass cls, jstring jexperiment, jint shot) {

  char *experiment = (char *)(*env)->GetStringUTFChars(env, jexperiment, 0);
  int status = TreeClose(experiment, shot);
  (*env)->ReleaseStringUTFChars(env, jexperiment, experiment);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    find
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_mdstree_MdsTree_find(JNIEnv * env, jclass cls, jstring jpath) {
  char *path = (char *)(*env)->GetStringUTFChars(env, jpath, 0);
  int nid = 0, status;

  status = TreeFindNode(path, &nid);
  (*env)->ReleaseStringUTFChars(env, jpath, path);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
  return nid;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    findWild
 * Signature: (Ljava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL Java_mdstree_MdsTree_findWild
    (JNIEnv * env, jclass cls, jstring jpath, jint usageMask) {

  jintArray jnids;
  int *nids;
  char *path = (char *)(*env)->GetStringUTFChars(env, jpath, 0);
  int nid = 0, status, i;
  int numNids = 0;
  void *ctx = 0;

  while ((status = TreeFindNodeWild(path, &nid, &ctx, usageMask)) & 1)
    numNids++;

  nids = (int *)malloc(sizeof(int) * numNids);
  ctx = 0;
  for (i = 0; i < numNids; i++) {
    TreeFindNodeWild(path, &nids[i], &ctx, usageMask);
  }
  jnids = (*env)->NewIntArray(env, numNids);
  (*env)->SetIntArrayRegion(env, jnids, 0, numNids, (jint *) nids);
  free((char *)nids);
  return jnids;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getMembersOf
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_mdstree_MdsTree_getMembersOf(JNIEnv * env, jclass cls, jint nid) {
  jintArray jnids;
  int numNids, numNidsLen, status;
  int *nids;
  struct nci_itm nci_list1[] = { {4, NciNUMBER_OF_MEMBERS, &numNids, &numNidsLen},
  {NciEND_OF_LIST, 0, 0, 0}
  }, nci_list2[] = { {
  0, NciMEMBER_NIDS, 0, &numNidsLen}, {
  NciEND_OF_LIST, 0, 0, 0}};

  status = TreeGetNci(nid, nci_list1);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
    return NULL;
  }

  if (numNids > 0) {
    nids = (int *)malloc(numNids * sizeof(nid));
    nci_list2[0].buffer_length = sizeof(int) * numNids;
    nci_list2[0].pointer = nids;
    status = TreeGetNci(nid, nci_list2);
    if (!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status), status);
      return NULL;
    }
  }
  jnids = (*env)->NewIntArray(env, numNids);
  (*env)->SetIntArrayRegion(env, jnids, 0, numNids, (jint *) nids);
  free((char *)nids);
  return jnids;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getChildrenOf
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_mdstree_MdsTree_getChildrenOf(JNIEnv * env, jclass cls, jint nid) {
  jintArray jnids;
  int *nids;
  int numNids, numNidsLen, status;
  struct nci_itm nci_list1[] = { {4, NciNUMBER_OF_CHILDREN, &numNids, &numNidsLen},
  {NciEND_OF_LIST, 0, 0, 0}
  }, nci_list2[] = { {
  0, NciCHILDREN_NIDS, 0, &numNidsLen}, {
  NciEND_OF_LIST, 0, 0, 0}};

  status = TreeGetNci(nid, nci_list1);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
    return NULL;
  }

  if (numNids > 0) {
    nids = (int *)malloc(numNids * sizeof(nid));
    nci_list2[0].buffer_length = sizeof(int) * numNids;
    nci_list2[0].pointer = nids;
    status = TreeGetNci(nid, nci_list2);
    if (!(status & 1))
    {
      RaiseException(env, MdsGetMsg(status), status);
      return NULL;
    }
  }
  jnids = (*env)->NewIntArray(env, numNids);
  (*env)->SetIntArrayRegion(env, jnids, 0, numNids, (jint *) nids);
  free((char *)nids);
  return jnids;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getParent
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_mdstree_MdsTree_getParent(JNIEnv * env, jclass cls, jint nid) {
  int parentNid;
  int nidLen = sizeof(int);
  struct nci_itm nciList[] = { {4, NciPARENT, &parentNid, &nidLen},
  {NciEND_OF_LIST, 0, 0, 0}
  };

  int status = TreeGetNci(nid, nciList);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
  return parentNid;
}

  /*
   * Class:     mdstree_MdsTree
   * Method:    getDefault
   * Signature: ()I
   */
JNIEXPORT jint JNICALL Java_mdstree_MdsTree_getDefault(JNIEnv * env, jclass cls) {
  int defaultNid, status;

  status = TreeGetDefaultNid(&defaultNid);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
  return defaultNid;
}

/*
 * Class:     mdstree_MdsTree
 * Method:    setDefault
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_setDefault(JNIEnv * env, jclass cls, jint nid) {
  int status = TreeSetDefaultNid(nid);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    beginSegment
 * Signature: (ILmdsdata/ArrayData;Lmdsdata/Data;Lmdsdata/Data;Lmdsdata/Data;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_beginSegment
    (JNIEnv * env, jclass cls, jint nid, jobject data, jobject start, jobject end, jobject dim) {
  struct descriptor *dataD, *startD, *endD, *dimD;
  int status;

  dataD = ObjectToDescrip(env, data);
  startD = ObjectToDescrip(env, start);
  endD = ObjectToDescrip(env, end);
  dimD = ObjectToDescrip(env, dim);
  status = TreeBeginSegment(nid, startD, endD, dimD, (struct descriptor_a *)dataD, -1);
  freeDescrip(dataD);
  freeDescrip(startD);
  freeDescrip(endD);
  freeDescrip(dimD);

  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    putRow
 * Signature: (ILmdsdata/Data;JZ)V
 */
#define BUFSIZE 1024
JNIEXPORT void JNICALL Java_mdstree_MdsTree_putRow
    (JNIEnv * env, jclass cls, jint nid, jobject data, jlong jtime, jboolean isFirst) {
  struct descriptor *dataD;
  int status;

  int64_t time = jtime;
  dataD = ObjectToDescrip(env, data);
  status = TreePutRow(nid, BUFSIZE, &time, (struct descriptor_a *)dataD);
  freeDescrip(dataD);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    beginSegment
 * Signature: (ILmdsdata/ArrayData;Lmdsdata/Data;Lmdsdata/Data;Lmdsdata/Data;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_beginCachedSegment
    (JNIEnv * env, jclass cls, jint nid, jobject data, jobject start, jobject end, jobject dim) {
  struct descriptor *dataD, *startD, *endD, *dimD;
  int status;

  dataD = ObjectToDescrip(env, data);
  startD = ObjectToDescrip(env, start);
  endD = ObjectToDescrip(env, end);
  dimD = ObjectToDescrip(env, dim);
  status =
      RTreeBeginSegment(nid, startD, endD, dimD, (struct descriptor_a *)dataD, -1, WRITE_BUFFER);
  freeDescrip(startD);
  freeDescrip(endD);
  freeDescrip(dimD);
  if (!(status & 1)) {
    freeDescrip(dataD);
    RaiseException(env, MdsGetMsg(status), status);
  }
  status = RTreePutSegment(nid, -1, (struct descriptor_a *)dataD, WRITE_BUFFER);
  freeDescrip(dataD);

  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    putRow
 * Signature: (ILmdsdata/Data;JZ)V
 */
#define BUFSIZE 1024
JNIEXPORT void JNICALL Java_mdstree_MdsTree_putCachedRow
    (JNIEnv * env, jclass cls, jint nid, jobject data, jlong jtime, jboolean isLast) {
  struct descriptor *dataD;
  int status;

  int64_t time = jtime;

  dataD = ObjectToDescrip(env, data);
  status =
      RTreePutRow(nid, BUFSIZE, &time, (struct descriptor_a *)dataD,
		  (isLast) ? WRITE_LAST : WRITE_BUFFER);
  freeDescrip(dataD);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    openCached
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_openCached
    (JNIEnv * env, jclass cls, jstring jexperiment, jint shot) {

  char *experiment = (char *)(*env)->GetStringUTFChars(env, jexperiment, 0);
  int status = RTreeOpen(experiment, shot);
  (*env)->ReleaseStringUTFChars(env, jexperiment, experiment);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    closeCached
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_closeCached
    (JNIEnv * env, jclass cls, jstring jexperiment, jint shot) {

  char *experiment = (char *)(*env)->GetStringUTFChars(env, jexperiment, 0);
  int status = TreeClose(experiment, shot);
  (*env)->ReleaseStringUTFChars(env, jexperiment, experiment);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    configureCache
 * Signature: (ZI)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_configureCache
    (JNIEnv * env, jclass cls, jboolean shared, jint size) {
  RTreeConfigure(shared, size);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    synchCache
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_synchCache(JNIEnv * env, jclass cls) {
  RTreeSynch();
}

static void callback(int nid, void *obj);

/*
 * Class:     mdstree_MdsTree
 * Method:    putCachedData
 * Signature: (ILmdsdata/Data;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_putCachedData
    (JNIEnv * env, jclass cls, jint nid, jobject data) {
  int status;
  struct descriptor *dataD;

  dataD = ObjectToDescrip(env, data);
  status = RTreePutRecord(nid, dataD, WRITE_BACK);
  freeDescrip(dataD);
  if (!(status & 1))
    RaiseException(env, MdsGetMsg(status), status);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    getCachedData
 * Signature: (I)Lmdsdata/Data;
 */
JNIEXPORT jobject JNICALL Java_mdstree_MdsTree_getCachedData(JNIEnv * env, jclass cls, jint nid) {
  EMPTYXD(xd);
  int status;
  jobject retObj;

  status = RTreeGetRecord(nid, &xd);
  if (!(status & 1)) {
    RaiseException(env, MdsGetMsg(status), status);
    return NULL;
  }

  retObj = DescripToObject(env, xd.pointer);
  MdsFree1Dx(&xd, 0);
  return retObj;
}

#define MAX_CALLBACKS 512
static struct {
  jobject object;
  char *callbackDescr;
} callbackInfo[MAX_CALLBACKS];

static char *getCallbackDesc(jobject obj, JNIEnv * env)
{
  int i;
  for (i = 0; i < MAX_CALLBACKS; i++) {
    if (callbackInfo[i].object && (*env)->IsSameObject(env, obj, callbackInfo[i].object)) {
      callbackInfo[i].object = 0;
      return callbackInfo[i].callbackDescr;
    }
  }
  return 0;
}

static void setCallbackDesc(jobject object, char *callbackDescr)
{
  int i;
  for (i = 0; i < MAX_CALLBACKS; i++) {
    if (!callbackInfo[i].object) {
      callbackInfo[i].object = object;
      callbackInfo[i].callbackDescr = callbackDescr;
      return;
    }
  }
}

/*
 * Class:     mdstree_MdsTree
 * Method:    registerCallback
 * Signature: (ILmdstree/ActiveTreeNode;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_registerCallback
    (JNIEnv * env, jclass cls, jint nid, jobject object) {
  int status;
  jobject refObj = (*env)->NewGlobalRef(env, object);
  char *callbackDescr = RTreeSetCallback(nid, (void *)refObj, callback);
  RTreeSetWarm(nid, 1);

  if (jvm == 0) {
    status = (*env)->GetJavaVM(env, &jvm);
    if (status)
      printf("GetJavaVM error %d\n", status);
  }
  setCallbackDesc(refObj, callbackDescr);
}

/*
 * Class:     mdstree_MdsTree
 * Method:    unregisterCallback
 * Signature: (ILmdstree/ActiveTreeNode;)V
 */
JNIEXPORT void JNICALL Java_mdstree_MdsTree_unregisterCallback
    (JNIEnv * env, jclass cls, jint nid, jobject object) {
  char *callbackDescr = getCallbackDesc(object, env);
  if (callbackDescr)
    RTreeClearCallback(nid, callbackDescr);
}

static void callback(int nid, void *obj)
{
  jmethodID mid;
  JNIEnv *env;
  jclass cls;
  jvalue args[4];

  env = getJNIEnv();

  cls = (*env)->GetObjectClass(env, (jobject) obj);
  if (!cls)
    printf("Error getting class for ActiveTreeNode\n");
  mid = (*env)->GetMethodID(env, cls, "callback", "()V");
  if (!mid)
    printf("Error getting method callback for ActiveTreeNode\n");
  (*env)->CallVoidMethodA(env, obj, mid, args);
  releaseJNIEnv();
}
